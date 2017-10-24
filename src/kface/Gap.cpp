#include <kface/Graph.hpp>
#include <vector>
#include "Util.hpp"
namespace kface {

std::vector<unsigned int> build_gap(const Graph& g,const std::vector<Vertex>&
		walk, bool forward, bool vgap = true) {
	std::vector<unsigned int> gap(walk.size(), 0);
	std::vector<int> last_seen(((vgap)?num_vertices(g):num_edges(g)), -1);
	size_t i = 0, num_assigned = 0;
	while (num_assigned != walk.size()) {
		Vertex v = walk[i];
		int j = last_seen[v];
		if (j > -1) {
			size_t k = (forward) ? j : walk.size() - 1 - i;
			if (gap[k] == 0) {
				++num_assigned;
			} 
			size_t d = (i > (size_t)j) ? i - j : walk.size() - j + i;
			gap[k] = d;
		}
		last_seen[v] = i;
		i = (i + 1) % walk.size();
	}
	return gap;
}

std::vector<unsigned int> build_bio_gap(const Graph& g, const std::vector<Vertex>& vwalk) {
	std::vector<size_t> walk;
	for (size_t i = 0; i < vwalk.size() - 1; ++i) {
		size_t j = i + 1;
		Edge e = edge(vwalk[i], vwalk[j], g).first;
		walk.push_back(g[e].id);
	}
	Edge e = edge(vwalk.back(), vwalk.front(), g).first;
	walk.push_back(g[e].id);

	std::vector<unsigned int> gap(walk.size(), 0);
	std::vector<int> last_seen(num_edges(g), -1);
	size_t i = 0, num_assigned = 0;
	/* std::cout << "walk: " << walk << std::endl; */
	while (num_assigned != walk.size()) {
		size_t v = walk[i];
		int j = last_seen[v];
		if (j > -1) {
			/* size_t k = j; */
			/* if (gap[k] == 0) { */
			num_assigned += 2;
			/* } */ 
			size_t d = std::min(i - j, walk.size() - i + j);
			/* if ((int)i >= j) { */
			/* 	d = std::min(i - j, walk.size() - j + i); */
			/* } else { */
			/* 	d = std::min(j - i, walk.size() - i + j); */
			/* } */
			gap[j] = d;
			gap[i] = d;
		}
		last_seen[v] = i;
		i = (i + 1) % walk.size();
	}
	return gap;
}

std::vector<unsigned int> build_vertex_gap(const Graph& g,const std::vector<Vertex>& walk, bool forward) {
	return build_gap(g, walk, forward);
	/* std::vector<unsigned int> gap(walk.size()); */
	/* std::vector<int> last_seen(num_vertices(g), -1); */
	/* size_t i = 0, num_assigned = 0; */
	/* while (num_assigned != walk.size()) { */
	/* 	Vertex v = walk[i]; */
	/* 	int j = last_seen[v]; */
	/* 	if (j > -1) { */
	/* 		size_t k = (forward) ? j : walk.size() - 1 - i; */
	/* 		if (gap[k] == 0) { */
	/* 			++num_assigned; */
	/* 		} */ 
	/* 		size_t d = (i > (size_t)j) ? i - j : walk.size() - j + i; */
	/* 		gap[k] = d; */
	/* 	} */
	/* 	last_seen[v] = i; */
	/* 	i = (i + 1) % walk.size(); */
	/* } */
	/* return gap; */
}

/* std::vector<size_t> build_edge_walk(const Graph& g, const std::vector<Vertex>& walk) { */
/* 	std::vector<size_t> ewalk; */
/* 	for (size_t i = 0; i < walk.size() - 1; ++i) { */
/* 		size_t j = i + 1; */
/* 		Edge e = edge(walk[i], walk[j], g).first; */
/* 		ewalk.push_back(g[e].id); */
/* 	} */
/* 	Edge e = edge(walk.back(), walk.front(), g).first; */
/* 	ewalk.push_back(g[e].id); */
/* 	return ewalk; */
/* } */

std::vector<unsigned int> build_edge_gap(const Graph& g, const std::vector<Vertex>& walk, bool forward) {
	auto ewalk = build_edge_walk(g, walk);
	return build_gap(g, ewalk, forward, false);
}
	
} /* namespace kface */
