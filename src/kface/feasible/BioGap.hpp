#ifndef KFACE_FEASIBLE_BIOGAP_HPP
#define KFACE_FEASIBLE_BIOGAP_HPP
#include <kface/Graph.hpp>
#include "../RotationSystem.hpp"
#include <iostream>

namespace kface {
namespace feasible {

class BioGap {
public:
	BioGap(const Graph& g, const std::vector<Vertex>& walk): m_g(g), m_walk(walk), m_rotation(g,walk),
																				 m_last_seen(num_edges(g), NOT_SEEN),
																				 full_walk_size(num_edges(g) * 2) {
		Edge e = boost::edge(m_walk[0], m_walk[1], m_g).first;
		extend_edge_walk(e);
	}

	void bounds(size_t min, size_t max) {
		m_min_gap = min;
		m_max_gap = max;
	}

	inline int& last_seen(Edge e) {
		return m_last_seen[m_g[e].id];
	}

	inline int last_seen(Edge e) const {
		return m_last_seen[m_g[e].id];
	}

	inline size_t compute_gap(Edge e) const {
		size_t fgap = m_ewalk.size() - last_seen(e);
		size_t rgap = full_walk_size - m_ewalk.size() + last_seen(e);
		return std::min(fgap, rgap);
	}

	inline bool is_valid_gaps(Edge e) const {
		if (last_seen(e) == NOT_SEEN) { return true; }

		size_t gap = compute_gap(e);
		return gap >= m_min_gap &&  gap <= m_max_gap;
	}

	inline size_t max_gap(Edge e) const {
		if (last_seen(e) == NOT_SEEN) { return 0; }
		/* std::cout << "(" << m_g[e].id << "): " << m_ewalk << std::endl; */
		/* std::cout << "gap: " << compute_gap(e) << std::endl; */
		/* std::cout << "fw: " << full_walk_size << std::endl; */
		return compute_gap(e);
	}

	inline size_t max_gap() const {
		Edge e = edge(m_walk.back(), m_walk.front(), m_g).first;
		return max_gap(e);
	}

	inline bool is_valid(Edge e) const {
		Vertex v = target(e, m_g);
		return m_rotation.is_valid(v) && is_valid_gaps(e);
	}

	inline bool is_valid() const {
		Edge e = edge(m_walk.back(), m_walk.front(), m_g).first;
		return is_valid_gaps(e);
	}

	inline void fix(Edge e) {
		extend_edge_walk(e);
		m_rotation.fix(target(e,m_g));
	}

	inline void backtrack() {
		m_rotation.backtrack();

		size_t id = m_ewalk.back();
		if (m_last_seen[id] == (int)m_ewalk.size() - 1) {
			m_last_seen[id] = NOT_SEEN;
		}
		m_ewalk.pop_back();
	}
	

private:
	void extend_edge_walk(Edge e) {
		if (last_seen(e) == NOT_SEEN) { 
			last_seen(e) = m_ewalk.size(); 
		}
		m_ewalk.push_back(m_g[e].id);
	}

	int NOT_SEEN = -1;
	const Graph& m_g;
	const std::vector<Vertex>& m_walk;
	std::vector<size_t> m_ewalk;
	RotationSystem m_rotation;
	std::vector<int> m_last_seen;
	size_t m_min_gap, m_max_gap, full_walk_size;
};
	
} /* namespace feasible */
} /* namespace kface */
#endif /* ifndef KFACE_FEASIBLE_BIOGAP_HPP */
