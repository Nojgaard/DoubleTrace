#ifndef DTRACE_UTIL_H
#define DTRACE_UTIL_H
#include <vector>
#include <stddef.h>
#include <dtrace/graph/base.h>
#include <boost/graph/graph_utility.hpp>

namespace dtrace {
std::ostream & operator << (std::ostream &os, const std::vector<Vertex> &trace);
std::vector<Edge> vertex_to_edge(const Graph &g, const std::vector<Vertex> &trace);
std::vector<Vertex> edge_to_vertex(const Graph &g, const std::vector<Edge> &trace);

template <typename T>
size_t min_expression(const std::vector<T> &vec) {
	size_t i = 0, j = 1, k = 0;
	while (i < vec.size() && j < vec.size() && k < vec.size()) {
		size_t ik = (i + k) % vec.size();
		size_t jk = (j + k) % vec.size();
		if (vec[ik] == vec[jk]) { ++k; } 
		else if (vec[ik] > vec[jk]) {
			i = i + k + 1;
			if (i <= j) { i = j + 1; }
			k = 0;
		} else {
			j = j + k + 1;
			if (j <= i) { j = i + 1; }
			k = 0;
		}
	}
	size_t min_pos = std::min(i, j);
	return min_pos;
}

template <typename T>
bool is_lex_min( const std::vector<T>& vec1, const std::vector<T>& vec2
		         , size_t shift1, size_t shift2) {
	assert(vec1.size() == vec2.size());
	for (size_t i = 0; i < vec1.size(); ++i) {
		size_t i1 = (i + shift1) % vec1.size();
		size_t i2 = (i + shift2) % vec2.size();
		if (vec1[i1] < vec2[i2]) {
			return true;
		} else if (vec1[i1] > vec2[i2]) {
			return false;
		}
	}
	return true;
}

inline bool valid_euler(int faces, int edges, int vertices) {
	return ((faces + edges + vertices) % 2) == 0;
}

} /* namespace dtrace */
#endif /* ifndef DTRACE_UTIL_H */
