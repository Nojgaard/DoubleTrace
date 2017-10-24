#ifndef KFACE_UTIL_HPP
#define KFACE_UTIL_HPP
#include <iostream>
#include <vector>
#include <kface/Graph.hpp>

namespace kface {

template<typename T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& vec) {
	for (const auto& v : vec) {
		os << v << " ";
	}
	return os;
}

inline void print_permutation(const std::vector<size_t>& walk, const std::vector<size_t>& perm
		, std::ostream& os = std::cout) {
	for (size_t w : walk) {
		os << perm[w] << " ";
	}
	std::cout << std::endl;
}

inline bool valid_euler(int faces, int edges, int vertices) {
	return ((faces + edges + vertices) % 2) == 0;
}

inline std::vector<size_t> build_edge_walk(const Graph& g, const std::vector<size_t>& vwalk) {
	std::vector<size_t> walk;
	for (size_t i = 0; i < vwalk.size() - 1; ++i) {
		size_t j = i + 1;
		Edge e = edge(vwalk[i], vwalk[j], g).first;
		walk.push_back(g[e].id);
	}
	Edge e = edge(vwalk.back(), vwalk.front(), g).first;
	walk.push_back(g[e].id);
	return walk;
}

} /* namespace kface */
#endif /* ifndef KFACE_UTIL_HPP */
