#ifndef GRAPHIO_CSV_H
#define GRAPHIO_CSV_H
#include <iostream>
#include <stdexcept>
#include "util.h"
namespace graphio {
namespace csv {

template<typename Graph>
void read_line(int lineno, std::string& s, Graph& g) {
	std::vector<std::string> elems = split(s, ',');
	if (elems.size() < 2) {
		throw std::runtime_error(std::to_string(lineno) + ": Badly formed line\n");
	}

	int src = std::stoi(elems[0]);
	if (src >= num_vertices(g)) {
		throw std::runtime_error(std::to_string(lineno) + ": Badly formed line\n");
	}
	for (size_t i = 1; i < elems.size(); ++i) {
		int tar = std::stoi(elems[i]);
		if (tar >= num_vertices(g)) {
			throw std::runtime_error(std::to_string(lineno) + ": Badly formed line\n");
		}
		add_edge(src, tar, g);
	}
}

template<typename Graph>
void read(std::istream& s, Graph& g){
	std::string line;
	int n;
	if (!std::getline(s, line)) {
		throw std::runtime_error("First line must number of vertices in the graph.\n");
	}
	n = std::stoi(line);
	for (int i = 0; i < n; ++i) { add_vertex(g); }
	
	int lineno = 2;
	while (std::getline(s, line)) {
		read_line(lineno, line, g);
		++lineno;
	}
}

template<typename Graph>
void write(std::ostream& os, const Graph& g) {
	os << num_vertices(g) << std::endl;
	for (auto vi = vertices(g); vi.first != vi.second; ++vi.first) {
		auto src = *vi.first;
		os << src;
		for (auto ni = adjacent_vertices(src, g); ni.first != ni.second; ++ni.first) {
			auto tar = *ni.first;
			os << ", " << tar;
		}
		os << std::endl;
	}
}

} /* namespace csv */
} /* namespace graphio */
#endif /* ifndef GRAPHIO_CSV_H */
