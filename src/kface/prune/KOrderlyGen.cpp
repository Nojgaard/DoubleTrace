#include "KOrderlyGen.hpp"
#include <morphism/aut.h>

namespace kface {
namespace prune {

using namespace morphism;

inline int KOrderlyGen::PruneAuts::operator () (size_t v, size_t i) const {
	return (int)(v - kg.aut(i, v));
}

inline int KOrderlyGen::PruneShifts::operator () (size_t vp, PruneData& d) {
	size_t w = kg.m_walk[d.k + kg.m_fidx[d.f]];
	size_t v = kg.aut(d.i, vp);
	if (w == v) { 
		if (kg.fsize(d.f) == d.k - 1) { return -1; }
		++d.k; 
	} else if (d.k > 1) {
		return (int)(w - v);
	} else {
		d.k = 0;
	}
	return 0;
}

inline int KOrderlyGen::PruneShifts::operator () (PruneData& d) {
	if (d.f == 0 && (d.k == 0 || (d.k == 1 && kg.m_walk[1] != kg.aut(d.i, kg.m_walk[kg.m_fidx.back()])))) {
		return 0;
	} else if (d.k == 0) {
		return -1;
	}

	size_t size = kg.fsize(kg.m_fidx.back()), j = size - d.k, k = d.k;
	while (k < size) {
		size_t ik = kg.m_fidx[d.f] + k;
		size_t jk = kg.m_fidx.size() + (j + k) % size;
		size_t v = kg.m_walk[ik], w = kg.aut(d.i, kg.m_walk[jk]);
		if (v == w) { ++k; }
		else { return (int)(v - w); }
	}
	if (k == size && size < kg.fsize(kg.m_fidx[d.f])) { return 1; }
	return 0;
}

KOrderlyGen::KOrderlyGen( const Graph& g, const std::vector<size_t>& walk
                        , const std::vector<size_t>& fidx)
                        : m_walk(walk), m_fidx(fidx) 
                        , m_valid_auts(PruneAuts(*this)) 
                        , m_valid_shifts(PruneShifts(*this)) {

	for (const auto& row : get_all_permutations(find_automorphisms(g), num_vertices(g))) {
		m_auts.insert(m_auts.end(), row.begin(), row.end());
	}
}

inline size_t KOrderlyGen::fsize(size_t f) const {
	return ((f == m_fidx.size() - 1) ? m_walk.size() : m_fidx[f + 1]) - m_fidx[f];
}

inline const size_t& KOrderlyGen::aut(size_t i, size_t v) const {
	return m_auts[i + v];
}

void KOrderlyGen::backtrack_shifts(size_t rewind, size_t vp) {
	for (size_t i = 0; i < rewind; ++i) {
		PruneData& d = m_valid_shifts[i];
		size_t v = aut(d.i, vp);
		size_t w = m_walk[m_fidx[d.f]];
		if (d.k == 0 && v == w) { ++d.k; }
		else if (d.k > 0) { --d.k; }
	}
	m_valid_shifts.backtrack();
}

bool KOrderlyGen::is_canonical(size_t v) {
	if (!m_valid_auts.canon_prune(v)) {
		return false;
	} else if (!m_valid_shifts.canon_prune(v)) {
		m_valid_auts.backtrack();
		backtrack_shifts(m_valid_shifts.rewind(), m_walk.back());
	}
	return true;
}

bool KOrderlyGen::is_canonical_face() {
	if (!m_valid_shifts.canon_prune()) {
		m_valid_shifts.backtrack();
		return false;
	}
	return true;
}

bool KOrderlyGen::is_canonical() const {
	return true;
}

void KOrderlyGen::backtrack() {
	m_valid_auts.backtrack();
	backtrack_shifts(m_valid_shifts.size(), m_walk[m_walk.size() - 2]);
}

void KOrderlyGen::prune_face(size_t v, size_t w) {
	m_valid_auts.canon_prune(v);
	m_valid_auts.canon_prune(w);
	for (PruneData& d : m_valid_shifts) {
		assert(d.k != 1);
		if (d.k > 1) { ++d.f; }
		d.k = 0;
	}
}

void KOrderlyGen::backtrack_face() {
}

	
} /* namespace prune */
} /* namespace kface */
