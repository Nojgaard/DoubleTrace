#ifndef KFACE_FEASIBLE_GAPDIST_HPP
#define KFACE_FEASIBLE_GAPDIST_HPP
#include <kface/Graph.hpp>
#include "../RotationSystem.hpp"

namespace kface {
namespace feasible {

class GapDist {
public:
	GapDist(const Graph& g, const std::vector<Vertex>& walk): m_g(g), full_walk_size(num_edges(g)*2)
																				 , last_seen(num_vertices(g))
																				 , fgap(full_walk_size, full_walk_size)
																				 , rgap(full_walk_size, full_walk_size)
																				 , m_walk(walk)
																				 , m_rotation(g, walk) {
		min_gap = 0;
		m_max_gap = full_walk_size;
		check_fgap = 1;
		check_rgap = 1;
		last_seen[walk[0]].push_back(0);
		last_seen[walk[1]].push_back(1);
	}

	void bounds(size_t min, size_t max) {
		min_gap = min;
		m_max_gap = max;
	}

	inline std::pair<size_t,size_t> compute_gap(Vertex v) const {
		if (degree(v, m_g) == 1) { return std::make_pair(full_walk_size, full_walk_size); }
		assert(last_seen[v].size() > 0);
		Vertex last_seen_v = last_seen[v].back();
		Vertex first_seen_v = last_seen[v].front();
		if (last_seen[v].size() == 0) { return std::make_pair(0,0); }

		return std::make_pair(m_walk.size() - last_seen_v,
				full_walk_size - m_walk.size() + first_seen_v);
	}

	inline bool is_valid_gaps(Vertex v) const {
		if (degree(v, m_g) > 1 && last_seen[v].size() == 0) { return true; }
		auto gaps = compute_gap(v);
		if (gaps.first < min_gap 
				|| (degree(v, m_g)-1 == last_seen[v].size() && gaps.second < min_gap)) {
			return false;
		} else if (gaps.first > m_max_gap 
				|| (degree(v, m_g)-1 == last_seen[v].size() && gaps.second > m_max_gap)) {
			return false;
		} else {
			return true;
		}
	}


	inline bool is_valid() const {
		return true;
	}

	inline size_t max_gap(Vertex v) const {
		if (degree(v, m_g) > 1 && last_seen[v].size() == 0) { return 0; }
		auto gaps = compute_gap(v);
		if (degree(v, m_g) - 1 == last_seen[v].size()) { 
			return std::max(gaps.first, gaps.second); 
		} else { 
			return gaps.first; 
		}
	}

	inline size_t max_gap() const {
		return 0;
	}

	inline size_t max_gap(Edge e) const {
		return max_gap(target(e, m_g));
	}

	inline bool is_valid(Vertex v, Vertex from, Vertex to) const {
		return m_rotation.is_valid(v, from, to) && is_valid_gaps(v);
	}

	inline bool is_valid(Vertex v) const {
		return m_rotation.is_valid(v) && is_valid_gaps(v);
	}

	inline bool is_valid(Edge e) const {
		return is_valid(target(e, m_g));
	}

	inline void fix(Vertex v) {
		/* if (last_seen[v].size() > 0) { */
		/* 	Vertex last_seen_v = last_seen[v].back(); */
		/* 	Vertex first_seen_v = last_seen[v].front(); */

		/* 	auto gaps = compute_gap(v); */
		/* 	fgap[last_seen_v] = gaps.first; */
		/* 	fgap[m_walk.size()] = gaps.second; */

		/* 	size_t start_b = full_walk_size - 1; */
		/* 	rgap[start_b - m_walk.size()] = gaps.first; */
		/* 	rgap[start_b - first_seen_v] = gaps.second; */

		/* 	assert(fgap[last_seen[v].back()] > 0); */
		/* 	assert(fgap[m_walk.size()] > 0); */
		/* } */ 
		last_seen[v].push_back(m_walk.size());

		m_rotation.fix(v);
	}

	inline void fix(Edge e) {
		fix(target(e, m_g));
	}

	inline void backtrack() {
		Vertex v = m_walk.back();
		last_seen[v].pop_back();
		/* if (last_seen[v].size() > 0) { */
		/* 	Vertex last_seen_v = last_seen[v].back(); */
		/* 	Vertex first_seen_v = last_seen[v].front(); */

		/* 	fgap[last_seen_v] = full_walk_size - last_seen_v + first_seen_v; */

		/* 	size_t start_b = full_walk_size - 1; */
		/* 	rgap[start_b - first_seen_v] = full_walk_size - last_seen_v + first_seen_v; */
		/* } */

		/* m_rotation.backtrack(v, from, to); */
		m_rotation.backtrack();
	}

private:
	const Graph& m_g;
	size_t check_fgap, check_rgap, min_gap, m_max_gap;
	size_t full_walk_size;
	std::vector<std::vector<size_t>> last_seen;
	std::vector<unsigned int> fgap, rgap;
	const std::vector<Vertex>& m_walk;
	RotationSystem m_rotation;
};
	
} /* namespace feasible */
} /* namespace kface */

#endif /* ifndef KFACE_FEASIBLE_GAPDIST_HPP */
