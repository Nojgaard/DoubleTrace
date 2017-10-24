#include "Gap.hpp"
#include <morphism/aut.h>

namespace kface {
namespace prune {
using namespace morphism;

std::vector<size_t> Gap::build_gap(bool forward) const {
	std::vector<size_t> gap;
	std::vector<int> last_seen(num_vertices(m_g), -1);
	size_t i = 0, num_assigned = 0;
	gap.resize(m_walk.size(), 0);
	while (num_assigned != m_walk.size()) {
		Vertex v = m_walk[i];
		int j = last_seen[v];
		if (j > -1) {
			size_t k = (forward) ? j : m_walk.size() - 1 - i;
			if (gap[k] == 0) {
				++num_assigned;
			} 
			size_t d = (i > (size_t)j) ? i - j : m_walk.size() - j + i;
			gap[k] = d;
		}
		last_seen[v] = i;
		i = (i + 1) % m_walk.size();
	}
	return gap;
}

size_t Gap::find_min_rotation(const std::vector<size_t>& gap) const {
	size_t i = 0, j = 1, k = 0;
	while (i < gap.size() && j < gap.size() && k < gap.size()) {
		size_t ik = (i + k) % gap.size();
		size_t jk = (j + k) % gap.size();
		if (gap[ik] == gap[jk]) { 
			++k; 
		} else if (gap[ik] > gap[jk]) {
			i = i + k + 1;
			if (i <= j) { i = j + 1; }
			k = 0;
		} else {
			j = j + k + 1;
			if (j <= i) { j = i + 1; }
			k = 0;
		}
	}
	return std::min(i, j);
}

std::vector<size_t> Gap::build_canon_gap(bool forward) const {
	auto gap = build_gap(forward);
	size_t min_rot = find_min_rotation(gap);
	std::rotate(gap.begin(), gap.begin() + min_rot, gap.end());
	return gap;
}

std::vector<size_t> Gap::build_canon_gap() const {
	auto gap_f = build_canon_gap(true);
	auto gap_b = build_canon_gap(false);
	return std::min(gap_f, gap_b);
}

Gap::Gap(const Graph& g, const std::vector<size_t>& walk): m_g(g), m_walk(walk)
																			, m_valid_auts(PruneAuts(*this)) {
	/* auto perms = get_all_permutations(find_automorphisms(g), num_vertices(g)); */
	auto perms = find_automorphisms(g);
	for (const auto& perm : perms) {
		m_auts.insert(m_auts.end(), perm.begin(), perm.end());
	}

	for (size_t i = 0; i < perms.size(); ++i) {
		if (i > 0) {
			m_valid_auts.push_back(i * num_vertices(g));
		}
	}
	prune_auts(m_walk[0]);
	prune_auts(m_walk[1]);
}

inline int Gap::PruneAuts::operator () (size_t v, size_t i) const {
	return (int)(v - og.aut(v,i));
}

inline const size_t& Gap::aut(size_t i, size_t v) const {
	return m_auts[i + v];
}

void Gap::prune_auts(size_t v) {
	m_valid_auts.prune(v);
}

void Gap::prune(size_t v) { }

bool Gap::is_canonical(size_t v) {
	if (!m_valid_auts.canon_prune(v)) {
		m_valid_auts.backtrack();
		return false;
	}
	return true;
}

bool Gap::is_canonical() {
	auto gap = build_canon_gap();
	if (gaps.find(gap) != gaps.end()) { return false; }

	gaps.insert(gap);
	return true;
}

void Gap::backtrack() {
	m_valid_auts.backtrack();
}
	
} /* prune */ 
} /* kface */ 
