#include <dtrace/io.h>
#include <dtrace/graph/base.h>
#include <graphio/g6.h>
#include <graphio/csv.h>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <dtrace/stat_printer.h>

namespace dtrace {

bool file_exists(const std::string& p) {
	std::ifstream in(p);
	return in.good();
}

void init_graph(Graph& g) {
	size_t i = 0;
	for (auto ei = edges(g); ei.first != ei.second; ++ei.first) {
		Edge e = *ei.first;
		g[e].idx = i++;
	}
}

void read_graph(std::string& p, Graph& g) {
	std::ifstream in(p);
	/* if(!in.is_open()) { */
	/* 	std::cerr << "Could not open file " << p << std::endl; */
	/* 	exit(0); */
	/* } */
	try {
		if (in.is_open()) {
			graphio::csv::read(in, g);
		} else {
			graphio::g6::read(p, g);
		}
	} catch (std::exception& e) {
		std::cerr << e.what();
		in.close();
		exit(0);
	}
	init_graph(g);
}

void read_graphs(std::string& p, std::vector<Graph>& gs) {
	std::ifstream in(p);
	if(!in.is_open()) {
		std::cerr << "Could not open file " << p << std::endl;
		exit(0);
	}
	try {
		graphio::g6::read(in, gs);
	} catch (std::exception& e) {
		std::cerr << e.what();
		in.close();
		exit(0);
	}
	for (Graph& g : gs) {
		init_graph(g);
	}
}

void write_stats(std::string& p, const Stats& stats) {
	bool write_header = !file_exists(p);
	std::ofstream ofs(p, std::ofstream::app);
	if (!ofs.is_open()) {
		std::cerr << "Could not open file " << p << std::endl;
		exit(0);
	}
	try {
		StatPrinter sp(stats, 0, 0, 0, write_header, ofs);
		sp.print_stats();
	} catch(std::exception& e) {
		std::cerr << e.what() << std::endl;
		ofs.close();
		exit(0);
	}


}
	
} /* namespace dtrace */
