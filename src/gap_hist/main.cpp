#include <iostream>
#include <iomanip>
#include <fstream>
#include <graphio/g6.h>
#include <kface/Graph.hpp>
#include <kface/Find.hpp>
#include <kface/GapHist.hpp>
#include <chrono>
#include <tclap/CmdLine.h>

using namespace kface;

void print_col(std::string i, int w, std::ostream& os) {
	os << std::setw(w) << std::left << i;
}

void print_col(int i, int w, std::ostream& os) {
	os << std::setw(w) << std::left << i;
}

void print_col(unsigned long i, int w, std::ostream& os) {
	os << std::setw(w) << std::left << i;
}

void print_col(double c, int w, std::ostream& os) {
	os << std::setw(w) << std::left << std::setprecision(4) << c;
}

void print_header(std::ostream& os, size_t graph_len = 10) {
	print_col("graph", graph_len, os);
	print_col("embeddings", 16, os);
	print_col("avg_gap", 16, os);
	print_col("avg_range", 16, os);
	print_col("duration", 10, os);
	print_col("timeout", 8, os);
	os << std::endl;
}

std::ostream& operator << (std::ostream& os, const std::vector<unsigned int>& vs) {
	for (auto v : vs) {
		os << v << " ";
	}
	return os;
}

void gap_hist(std::ostream& gen_os, std::ostream& min_hist_os, std::ostream&
		max_hist_os, std::ostream& all_hist_os, const Graph& g, int timeout, kface::Algorithm alg, size_t graph_len, GapRepr gap_repr) {
	auto start = std::chrono::system_clock::now();
	auto res = find_gap_hist(g, alg, gap_repr, timeout);
	std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
	print_col(g[boost::graph_bundle].name, graph_len, gen_os);
	print_col(res.first.num_walks(), 16, gen_os);
	print_col(res.first.average(), 16, gen_os);
	print_col(res.first.average_range(), 16, gen_os);
	print_col(dur.count(), 10, gen_os);
	print_col(res.second, 8, gen_os);
	gen_os << std::endl;
	min_hist_os << res.first.min_hist() << std::endl;
	max_hist_os << res.first.max_hist() << std::endl;
	all_hist_os << res.first.all_hist() << std::endl;
}

int main(int argc, const char *argv[]) {
	std::string input_path, output_path;
	int timeout;
	bool use_egap;
	size_t min_degree, max_degree;
	std::string gap_repr_str;
	try {
		using namespace TCLAP;
		CmdLine cmd("Command description", ' ', "0.9");
		ValueArg<std::string> inputArg("i", "input", "Input path", true, "", "string", cmd);
		ValueArg<int> timeoutArg("t", "timeout", "timeout duration for each graph (0 for inf)"
				, false, 0, "int", cmd);
		ValueArg<std::string> outputArg("o", "output", "Output Path", false, "", "string", cmd);
		/* SwitchArg egapArg("e", "edge-gap", "use the edge gap representation", cmd, false); */
		ValueArg<std::string> gapArg("g", "gap-repr", "gap representation to use", false, "vertex"
				, "[none, edge, vertex, bio]", cmd);
		ValueArg<size_t> mdegreeArg("d", "min-degree", "minumu degree", false, 0, "int", cmd);
		ValueArg<size_t> mxdegreeArg("e", "max-degree", "maximum degree", false, 0, "int", cmd);
		cmd.parse(argc, argv);

		input_path = inputArg.getValue();
		output_path = outputArg.getValue();
		timeout = timeoutArg.getValue();
		gap_repr_str = gapArg.getValue();
		min_degree = mdegreeArg.getValue();
		max_degree = mxdegreeArg.getValue();
		/* use_egap = egapArg.getValue(); */
	} catch(TCLAP::ArgException& e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
		return 0;
	}

	kface::GapRepr gap_repr;
	 if (gap_repr_str == "edge") {
		gap_repr = kface::GapRepr::Edge;
	} else if (gap_repr_str == "vertex") {
		gap_repr = kface::GapRepr::Vertex;
	} else if (gap_repr_str == "bio") {
		gap_repr = kface::GapRepr::Bio;
	} else {
		std::cerr << "error: '" << gap_repr << "' is an invalid 'gap-repr' argument, see help for valid arguments.";
		std::cerr << std::endl;
		return 0;
	}

	bool use_file = (output_path != "");
	std::ofstream ofile(output_path + ".dat"), ohnfile(output_path +
			"_min_hist.dat"), ohxfile(output_path + "_max_hist.dat");
	std::ofstream ohafile(output_path + "_all_hist.dat");

	std::cout << "input path: " << input_path << std::endl;
	std::cout << "output path: ";
	if (use_file) {
		std::cout << output_path << std::endl;
	} else {
		std::cout << "TERMINAL" << std::endl;
	}
	std::string max_degree_str = ((max_degree > 0) ? std::to_string(max_degree) : "INF");
	std::cout << "degree range: [" << min_degree << ", " << max_degree_str << "]" << std::endl;
	std::ifstream file(input_path);
	std::string g6_graph;
	std::getline(file, g6_graph);
	size_t graph_len = g6_graph.size() + 5;
	if (use_file) {
		print_header(ofile, graph_len);
	} else {
		print_header(std::cout, graph_len);
	}
	int count = 0;
	do {
		Graph g;
		graph_len = g6_graph.size() + 5;
		graphio::g6::read(g6_graph, g);
		if (min_degree > 0 || max_degree > 0) {
			bool is_valid = true;
			for (auto it = vertices(g); it.first != it.second; ++it.first) {
				Vertex v = *it.first;
				if (min_degree > 0 && degree(v, g) < min_degree) {
					is_valid = false; break;
				} else if (max_degree > 0 && degree(v, g) > max_degree) {
					is_valid = false; break;
				}
			}
			if (!is_valid) { continue; }
		}
		++count;

		size_t ec = 0;
		for (auto it = edges(g); it.first != it.second; ++it.first) {
			Edge e = *(it.first);
			g[e].id = ec++;
		}

		if (use_file) {
			std::cout << "\r" << "finding one face embeddings for graph #" << count << std::flush;
			gap_hist(ofile, ohnfile, ohxfile, ohafile, g, timeout, Algorithm::Orderly, graph_len, gap_repr);
		} else {
			gap_hist(std::cout, std::cout, std::cout, std::cout, g, timeout, Algorithm::Orderly, graph_len, gap_repr);
		}
	} while (std::getline(file, g6_graph));
	std::cout << std::endl;
	ofile.close();

	return 0;
}
