#include <dtrace/util.h>

namespace dtrace {

std::ostream & operator << (std::ostream &os, const std::vector<Vertex> &trace) {
	for (Vertex v : trace) {
		os << v << " ";
	}
	return os;
}

std::vector<Edge> vertex_to_edge(const Graph &g, const std::vector<Vertex> &trace) {
	/* assert(trace.size() == num_edges(g) * 2); */

	std::vector<Edge> etrace;
	for (size_t i = 1; i < trace.size(); ++i) {
		auto pe = boost::edge(trace[i-1], trace[i], g);
		assert(pe.second);
		etrace.push_back(pe.first);
	}
	etrace.push_back(boost::edge(trace.back(), trace.front(), g).first);
	assert(etrace.size() == trace.size());
	return etrace;
}

std::vector<Vertex> edge_to_vertex(const Graph &g, const std::vector<Edge> &trace) {
	/* assert(trace.size() == num_edges(g) * 2); */
	assert(trace.size() > 1);

	std::vector<Vertex> vtrace;
	Vertex start = target(trace[0], g);
	if (source(trace[1], g) == start || target(trace[1], g) == start) {
		start = source(trace[0], g);
	}
	vtrace.push_back(start);
	for (Edge e : trace) {
		Vertex next = opposite(e, vtrace.back(), g);
		vtrace.push_back(next);
	}
	vtrace.pop_back();
	assert(vtrace.size() == trace.size());
	return vtrace;
}

} /* namespace dtrace */
