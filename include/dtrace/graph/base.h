#ifndef DTRACE_GRAPH_BASE
#define DTRACE_GRAPH_BASE
#include <boost/graph/adjacency_list.hpp>
#include <iostream>

namespace dtrace {
	enum class EdgeDirection {ANY, ANTI};
	struct GraphBundle {
		std::string name;
	};
	struct VertexBundle {};
	struct EdgeBundle {
	public:
		size_t idx;
		EdgeDirection req_dir = EdgeDirection::ANY;
	};
	typedef boost::property<boost::edge_index_t, std::size_t, EdgeBundle> edge_prop;

	using Graph = typename boost::adjacency_list<  boost::vecS
																, boost::vecS
																, boost::undirectedS
																, VertexBundle
																, EdgeBundle
																, GraphBundle>;
	using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
	using Edge = typename boost::graph_traits<Graph>::edge_descriptor;
	
	inline void print_edge (std::ostream &os, const Edge e, const Graph &g) {
		os << "(" << source(e, g) << ", " << target(e, g) << ")";
	}
} /* namespace dtrace */
#endif /* ifndef DTRACE_GRAPH_BASE */
