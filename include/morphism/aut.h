#ifndef DTRACE_AUT_H
#define DTRACE_AUT_H
#include <vector>
#include <cstddef>
#include <bliss/graph.hh>
#include <set>

namespace morphism {
	
using Perm = std::vector<size_t>;
using Group = std::vector<std::vector<size_t>>;

inline void callback(void *user_param, unsigned int n, const unsigned int *aut) {
	std::vector<std::vector<size_t>> *raut = 
		static_cast<std::vector<std::vector<size_t>> *>(user_param);
	raut->push_back(std::vector<size_t>(n));
	for (size_t i = 0; i < n; i++) {
		raut->back()[i] = aut[i];
	}
}

template<typename Graph>
void convert_to_bliss_graph(const Graph &g, bliss::Graph &bg) {
	for (auto vi = vertices(g); vi.first != vi.second; ++vi.first) {
		auto bv = bg.add_vertex();
		assert(bv == *vi.first);
	}
	for (auto ei = edges(g); ei.first != ei.second; ++ei.first) {
		auto src = source(*ei.first, g);
		auto tar = target(*ei.first, g);
		bg.add_edge(src, tar);
	}
}

std::vector<size_t> permute_trace(const Perm& trace, const Perm& aut);

Perm permute(const Perm& src, const Perm& tar);

std::vector<Perm> get_all_permutations(const std::vector<Perm> &auts, size_t n = 0);

Group perm2group(const Perm& p);

template<typename Graph>
std::vector<std::vector<size_t>> find_automorphisms(const Graph &g) {
	bliss::Graph bg;
	convert_to_bliss_graph(g, bg);
	std::vector<std::vector<size_t>> auts;
	bliss::Stats stats;
	bg.find_automorphisms(stats, callback, &auts);
	return auts;
}

} /* namespace morphism */
#endif /* ifndef DTRACE_AUT_H */
