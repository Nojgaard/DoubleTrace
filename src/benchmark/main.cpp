#include <iostream>
#include <iomanip>
#include <fstream>
#include <graphio/g6.h>
#include <kface/Graph.hpp>
#include <kface/Find.hpp>
#include <chrono>
#include <tclap/CmdLine.h>

using namespace kface;

enum class Count { all, optimal, misses };
std::ostream& operator << (std::ostream& os, Count c) {
	switch(c) {
		case Count::all: os << "ALL"; break;
		case Count::optimal: os << "OPTIMAL"; break;
		case Count::misses: os << "MISSES"; break;
	}
	return os;
}

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

void print_header(std::ostream& os, Count count, size_t graph_len = 10) {
	print_col("graph", graph_len, os);
	print_col("embeddings", 16, os);
	switch (count) {
		case Count::optimal: print_col("max_gap", 12, os); break;
		case Count::misses: print_col("misses", 12, os); break;
		default: break;
	}
	/* if (use_fitness) { */
	/* 	print_col("max_gap", 12, os);; */
	/* } */
	print_col("duration", 10, os);
	print_col("timeout", 8, os);
	os << std::endl;
}

void bench_graph(const Graph& g, std::ostream& os, int timeout,
		kface::Algorithm alg, kface::GapRepr gap_repr , size_t min_gap, size_t max_gap, 
		size_t graph_len = 10) {
	auto start = std::chrono::system_clock::now();
	std::pair<unsigned long, bool> res;
	if (min_gap > 0 || max_gap < 1000) {
		res = count_one_face(g, timeout, alg, min_gap, max_gap, gap_repr);
	} else {
		res = count_one_face(g, timeout, alg);
	}
	std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
	print_col(g[boost::graph_bundle].name, graph_len, os);
	print_col(res.first, 16, os);
	print_col(dur.count(), 10, os);
	print_col(res.second, 8, os);
	os << std::endl;
}

void bench_graph_miss(const Graph& g, std::ostream& os, int timeout, kface::Algorithm alg,
		size_t graph_len) {
	auto start = std::chrono::system_clock::now();
	unsigned long num_misses, num_found;
	bool success;
	std::tie(num_found, num_misses, success) = count_one_face_and_misses(g, alg, timeout);
	std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
	print_col(g[boost::graph_bundle].name, graph_len, os);
	print_col(num_found, 16, os);
	print_col(num_misses, 16, os);
	print_col(dur.count(), 10, os);
	print_col(success, 8, os);
	os << std::endl;
}

void bench_graph_opt(const Graph& g, std::ostream& os, int timeout,
		kface::Algorithm alg, size_t graph_len, GapRepr gap_repr) {
	auto start = std::chrono::system_clock::now();
	unsigned long walks;
	size_t max_gap;
	bool sucess;
	std::tie(walks, max_gap, sucess) = count_max_embeddings(g, alg, timeout, gap_repr);
	std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
	print_col(g[boost::graph_bundle].name, graph_len, os);
	print_col(walks, 16, os);
	print_col(max_gap, 12, os);
	print_col(dur.count(), 10, os);
	print_col(sucess, 8, os);
	os << std::endl;
}

int main(int argc, const char *argv[]) {
	std::string alg_str;
	std::string input_path, output_path;
	size_t min_gap, max_gap;
	int timeout;
	bool use_fitness;
	std::string gap_repr_str, count_str;
	size_t min_degree, max_degree;

	try {
		using namespace TCLAP;
		CmdLine cmd("Command description", ' ', "0.9");
		ValueArg<std::string> inputArg("i", "input", "Input path", true, "", "string", cmd);
		ValueArg<std::string> algArg("a", "alg", "Pruning algorithm", false, "algebraic"
				, "[brute, algebraic, gap]", cmd);
		ValueArg<int> timeoutArg("t", "timeout", "timeout duration for each graph (0 for inf)"
				, false, 20, "int", cmd);
		ValueArg<std::string> outputArg("o", "output", "Output Path", false, "", "string", cmd);
		ValueArg<size_t>  minArg("n", "min", "minimim gap", false, 0, "uint", cmd);
		ValueArg<size_t>  maxArg("x", "max", "minimim gap", false, 1000, "uint", cmd);
		/* SwitchArg egapArg("e", "edge-gap", "use and print the edge gap representation", cmd, false); */
		ValueArg<std::string> gapArg("g", "gap-repr", "gap representation to use", false, "none"
				, "[none, edge, vertex, bio]", cmd);
		/* SwitchArg fitArg("f", "fitness", "only count embeddings with the smallest max-gap", cmd, false); */
		ValueArg<std::string> countArg("c", "count", "specify what to count", false, "all"
				, "[all, opt, miss]", cmd);
		ValueArg<size_t> mdegreeArg("d", "min-degree", "minumum degree", false, 0, "int", cmd);
		ValueArg<size_t> mxdegreeArg("e", "max-degree", "maximum degree", false, 0, "int", cmd);
		cmd.parse(argc, argv);

		input_path = inputArg.getValue();
		output_path = outputArg.getValue();
		alg_str = algArg.getValue();
		timeout = timeoutArg.getValue();
		min_gap = minArg.getValue();
		max_gap = maxArg.getValue();
		gap_repr_str = gapArg.getValue();
		min_degree = mdegreeArg.getValue();
		max_degree = mxdegreeArg.getValue();
		/* use_egap = egapArg.getValue(); */
		/* use_fitness = fitArg.getValue(); */
		count_str = countArg.getValue();
	} catch(TCLAP::ArgException& e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
		return 0;
	}
	kface::Algorithm alg;
	if (alg_str == "brute") {
		alg = kface::Algorithm::Brute;
	} else if (alg_str == "algebraic") {
		alg = kface::Algorithm::Orderly;
	} else if (alg_str == "gap") {
		alg = kface::Algorithm::Gap;
	} else if (alg_str == "all") {
		alg = kface::Algorithm::All;
	} else {
		std::cerr << "error: '" << alg_str << "' is an invalid alg argument, see help for valid arguments.";
		std::cerr << std::endl;
		return 0;
	}

	Count count;
	if (count_str == "all") {
		count = Count::all;
	} else if (count_str == "opt") {
		count = Count::optimal;
	} else if (count_str == "miss") {
		count = Count::misses;
	} else {
		std::cerr << "error: '" << count_str << "' is an invalid --count argument, see help for valid arguments.";
		std::cerr << std::endl;
		return 0;
	}

	kface::GapRepr gap_repr;
	if (gap_repr_str == "none") {
		gap_repr = kface::GapRepr::None;
	} else if (gap_repr_str == "edge") {
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

	if (timeout < 0) {
		std::cerr << "error: timeout value must be a positive int." << std::endl;
		return 0;
	}

	std::string p(input_path);

	bool use_file = (output_path != "");
	std::ofstream ofile(output_path);

	std::cout << "input path: " << input_path << std::endl;
	std::cout << "output path: ";
	if (use_file) {
		std::cout << output_path << std::endl;
	} else {
		std::cout << "TERMINAL" << std::endl;
	}

	std::cout << "algorithm: " << alg << std::endl;
	std::cout << "gap representation: " << gap_repr << std::endl;
	std::cout << "counting: " << count << std::endl;
	std::string max_degree_str = ((max_degree > 0) ? std::to_string(max_degree) : "INF");
	std::cout << "degree range: [" << min_degree << ", " << max_degree_str << "]" << std::endl;
	std::cout << "minimum gap: " << min_gap << std::endl;
	std::cout << "maximum gap: " << ((max_gap == 1000)?"INF":std::to_string(max_gap)) << std::endl;
	/* std::cout << "fitness func: " << ((!use_fitness)? "NONE" : "MAX_GAP") << std::endl; */
	if (timeout > 0) {
		std::cout << "timeout: " << timeout << " second(s)" << std::endl;
	} else {
		std::cout << "timeout: INF" << std::endl;
	}
	/* std::string op(argv[2]); */
	
	std::ifstream file(p);
	/* std::ofstream ofile(op); */
	std::string g6_graph;
	std::getline(file, g6_graph);
	size_t graph_len = g6_graph.size() + 5;
	if (use_file) {
		print_header(ofile, count, graph_len);
	} else {
		print_header(std::cout, count, graph_len);
	}
	int num_graph = 0;
	do {
		/* std::cerr << "#Graph: " << count++ << std::endl; */
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
		++num_graph;

		size_t id = 0;
		for (auto it = edges(g); it.first != it.second; ++it.first) {
			Edge e = *it.first;
			g[e].id = id++;
		}
		if (use_file) {
			std::cout << "\r" << "finding one face embeddings for graph #" << num_graph << std::flush;
			switch(count) {
				case Count::all: 
					bench_graph(g, ofile, timeout, alg, gap_repr, min_gap, max_gap, graph_len);
					break;
				case Count::optimal:
					bench_graph_opt(g, ofile, timeout, alg, graph_len, gap_repr);
					break;
				case Count::misses:
					bench_graph_miss(g, ofile, timeout, alg, graph_len);
					break;
			}

			/* if (!use_fitness) { */
			/* 	bench_graph(g, ofile, timeout, alg, gap_repr, min_gap, max_gap, graph_len); */
			/* } else { */
			/* 	bench_graph_opt(g, ofile, timeout, alg, graph_len, gap_repr); */
			/* } */
		} else {
			switch(count) {
				case Count::all: 
					bench_graph(g, std::cout, timeout, alg, gap_repr, min_gap, max_gap, graph_len);
					break;
				case Count::optimal:
					bench_graph_opt(g, std::cout, timeout, alg, graph_len, gap_repr);
					break;
				case Count::misses:
					bench_graph_miss(g, std::cout, timeout, alg, graph_len);
					break;
			}
			/* if (!use_fitness) { */
			/* 	bench_graph(g, std::cout, timeout, alg, gap_repr, min_gap, max_gap, graph_len); */
			/* } else { */
			/* 	bench_graph_opt(g, std::cout, timeout, alg, graph_len, gap_repr); */
			/* } */
		}
	} while (std::getline(file, g6_graph));
	std::cout << std::endl;
	ofile.close();
	return 0;
}
