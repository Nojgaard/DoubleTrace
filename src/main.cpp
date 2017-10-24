#include <dtrace/graph.h>
#include <dtrace/bruteforce/face_trace.h>
#include <dtrace/traversor.h>
#include <iostream>
#include <exception>
#include <tclap/CmdLine.h>
#include <dtrace/io.h>
#include <dtrace/bruteforce/face_trace.h>
#include <dtrace/util.h>

void print(const std::vector<size_t>& vec) {
	for (size_t v : vec) {
		std::cout << v << " ";
	}
}


void print(const std::vector<dtrace::Face>& fs) {
	for (const auto& f : fs) {
		print(f);
		std::cout << "| ";
	}
}

int main(int argc, const char *argv[]) {
	std::string input_path, output_path;
	bool only_anti;
	bool use_bruteforce;
	bool canon, print_progress, print_header;
	int nfaces;
	try {
		using namespace TCLAP;
		CmdLine cmd("Command description", ' ', "0.9");
		ValueArg<std::string> inputArg("i", "input", "File input path", true, "", "string", cmd);
		ValueArg<std::string> outputArg("o", "output", "File output path", false, "", "string", cmd);
		SwitchArg antiArg("a", "anti", "Only find anti-parallel traces", cmd, true);
		SwitchArg compArg("c", "canon", "for bruteforce, use the gap representation", cmd, false);
		SwitchArg bruteArg("b", "brute", "Find with brute force", cmd, false);
		SwitchArg printArg("p", "print-progress", "Prints progress in intervals", cmd, false);
		SwitchArg headerArg("d", "print-header", "Prints the header of data collected", cmd, false);
		ValueArg<int> facesArg("f", "nfaces", "Number of faces on embedding",  false, 2, "int", cmd);
		cmd.parse(argc, argv);

		input_path = inputArg.getValue();
		only_anti = antiArg.getValue();
		nfaces = facesArg.getValue();
		use_bruteforce = bruteArg.getValue();
		output_path = outputArg.getValue();
		canon = compArg.getValue();
		print_progress = printArg.getValue();
		print_header = headerArg.getValue();

	} catch (TCLAP::ArgException &e) {  // catch any exceptions  
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
		return 0;
	}

	if (print_header) {
		dtrace::Stats stats;
		dtrace::StatPrinter(stats, 0, 0, 0);
		return 0;
	}

	if (nfaces < 1 || nfaces > 2) {
		std::cerr << "-f input must be 1 or 2" << std::endl;
		return 0;
	}

	dtrace::Graph g;
	try {
		if (print_progress) {
			std::cout << "Reading file: " << input_path << std::endl;
		}
		/* g = dtrace::read_graph_dimacs(input_path); */
		dtrace::read_graph(input_path, g);
		if (print_progress) {
			std::cout << "Read graph: v " << num_vertices(g) << " e " << num_edges(g) << std::endl;
		}
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		return 0;
	}
	
	if (only_anti) {
		for (auto ei = edges(g); ei.first != ei.second; ++ei.first) {
			g[*ei.first].req_dir = dtrace::EdgeDirection::ANTI;
		}
	}
	/* auto traces = dtrace::find_strong_traces(g); */
	/* std::cout << "Found " << traces.size() << " unique traces." << std::endl; */
	/* for (auto &t : traces) { */
	/* 	using namespace dtrace; */
	/* 	std::cout << t.second << std::endl; */
	/* } */
	/* auto s = dtrace::embedding::find_two_face_embeddings(g); */
	/* std::cout << s.size() << std::endl; */

	/* if (compare) { */
	/* 	dtrace::Traversor t(g); */
	/* 	if (nfaces == 1) { */
	/* 		t.find_strong_traces(); */
	/* 	} else { */
	/* 		t.find_two_face_embeddings(); */
	/* 	} */
	/* 	return 0; */
	/* } */

	dtrace::Stats stats;
	if (!use_bruteforce) {
		dtrace::Traversor t(g, print_progress);
		if (nfaces == 1) {
			if (!canon) {
				auto res = t.find_strong_traces();
			} else {
				auto res = t.find_strong_traces_brute();
			}
			/* for (auto& kv : res) { */
			/* 	using namespace dtrace; */
			/* 	std::cout << kv.first << std::endl; */
			/* 	std::cout << kv.second << std::endl; */
			/* 	std::cout << "----" << std::endl; */
			/* } */
		} else {
			auto res = t.find_two_face_embeddings();
				/* for (auto& kv : res) { */
				/* 	for (auto& emb : kv.second) { */
				/* 	for (auto& v : emb) { */
				/* 		std::cout << v << " "; */
				/* 	} */
				/* 	std::cout << "| "; */
				/* 	} */
				/* 	std::cout << std::endl; */
				/* } */
		}
		stats = t.get_stats();
	} else {
		stats.enable_print = print_progress;
		if (nfaces == 1 && canon) {
			dtrace::bruteforce::find_one_face_embeddings(g, stats);
		} else {
			dtrace::bruteforce::find_n_face_embeddings(g, nfaces, stats);
		}
	}
	/* if (nfaces == 2) { */
	/* 	if (use_bruteforce) { */
	/* 		dtrace::Stats stats; */
	/* 		auto res = dtrace::bruteforce::find_n_face_embeddings(g, 2, stats); */
	/* 		std::cout << res.size() << std::endl; */
	/* 	} else { */
	/* 		auto test = t.find_two_face_embeddings(); */
	/* 		/1* auto s = dtrace::bruteforce::find_two_face_embeddings(g); *1/ */
	/* 		std::cout << test.size() << std::endl; */
	/* 	} */
	/* } else { */
	/* 	auto test = t.find_strong_traces(); */
	/* 	std::cout << test.size() << std::endl; */
	/* } */

	if (output_path != "") {
		dtrace::write_stats(output_path, stats);
	} else if (!print_progress) {
		stats.enable_print = true;
		dtrace::StatPrinter sp(stats, 0,0,0,false, std::cout);
		sp.print_stats();
	}
	
	return 0;
}
