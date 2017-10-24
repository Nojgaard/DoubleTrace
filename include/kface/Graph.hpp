#ifndef KFACE_GRAPH_HPP
#define KFACE_GRAPH_HPP
#include <boost/graph/adjacency_list.hpp>
/* #include <iosfwd> */
#include <iostream>

namespace kface {
	struct GraphBundle {
		std::string name;
	};
	struct VertexBundle {};
	struct EdgeBundle {
		size_t id;
	};
	using Graph = typename boost::adjacency_list < boost::vecS
		                                          , boost::vecS
																, boost::undirectedS
																, VertexBundle
																, EdgeBundle
																, GraphBundle>;
	using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
	using Edge = typename boost::graph_traits<Graph>::edge_descriptor;

	inline std::ostream& operator << (std::ostream& os, const Edge& e) {
		os << "(" << e.m_source << ", " << e.m_target << ")";
		return os;
	}

	inline std::ostream& operator << (std::ostream& os, const Graph& g) {
		for (auto it = vertices(g); it.first != it.second; ++it.first) {
			Vertex v = *it.first;
			os << v << ": ";
			for (auto ait = adjacent_vertices(v, g); ait.first != ait.second; ++ait.first) {
				Vertex w = *ait.first;
				if (v < w) { os << w << " "; }
			}
			os << std::endl;
		}
		return os;
	}

	inline void print_graph(const Graph& g, std::ostream& os = std::cout) {
		os << g;
	}
	
} /* namespace kface */
#endif /* ifndef KFACE_GRAPH_HPP */
