#ifndef KFACE_PRUNE_ORDERLY_GEN2_HPP
#define KFACE_PRUNE_ORDERLY_GEN2_HPP
#include <vector>
#include "../PruneVector.hpp"
#include "../Util.hpp"
#include <kface/Graph.hpp>
#include <morphism/aut.h>

namespace kface {
namespace prune {
using namespace morphism;
	
size_t idx(size_t r, size_t c, size_t rn) { return (r) + c; }
struct PruneAuts {
	PruneAuts(const std::vector<size_t>& a, size_t rn): m_auts(a), rn(rn){}
	bool operator () (size_t v, size_t i) { return m_auts[idx(i,v, rn)] != v; }
	/* PruneAuts(const std::vector<std::vector<size_t>>& a); */
private:
	const std::vector<size_t>& m_auts;
	size_t rn;
	/* const std::vector<std::vector<size_t>>& m_auts; */
};

struct PruneShifts {
	/* PruneShifts(const std::vector<std::vector<size_t> >& a,const std::vector<size_t>& w); */
PruneShifts(const std::vector<size_t>& a,const std::vector<size_t>& w, size_t rn)
		: m_auts(a), m_walk(w), rn(rn) {}
bool operator () (size_t v, std::pair<size_t, size_t>& ik) {
	/* const auto& aut = m_auts[ik.first]; */
	size_t r = ik.first;
	size_t& k = ik.second;

	if (m_walk[k] == m_auts[idx(r, v, rn)]) { ++k; }
	else if (k > 1 && m_auts[idx(r,v,rn)] > m_walk[k]) { return true; }
	else { k = 0; }
	return false;
}

private:
	const std::vector<size_t>& m_auts;
	/* const std::vector<std::vector<size_t>>& m_auts; */
	const std::vector<size_t>& m_walk;
	size_t rn;
};

struct PruneReverse {
	/* PruneReverse(const std::vector<std::vector<size_t>>& a,const std::vector<size_t>& w); */
PruneReverse(const std::vector<size_t>& a,const std::vector<size_t>& w, size_t rn)
		: m_auts(a), m_walk(w), rn(rn) {}

bool operator () (size_t v, std::pair<size_t, size_t>& ik) {
	/* const auto& aut = m_auts[ik.first]; */
	size_t r = ik.first;
	size_t& k = ik.second;
	if (k > 0) { return false; }
	else if (m_auts[idx(r,v,rn)] == m_walk[0] && m_auts[idx(r,m_walk.back(),rn)] == m_walk[1]) {
		for (size_t i = 2; i < m_walk.size(); ++i) {
			size_t j = m_walk.size() - i;
			assert(m_walk[i] <= idx(r,m_walk[j],rn));
			if (m_walk[i] < m_auts[idx(r, m_walk[j],rn)]) { return true; }
		}
		if (v < m_auts[idx(r, m_walk[0], rn)]) { return true; }
		k = m_walk.size();
	}
	return false;
}

private:
	const std::vector<size_t>& m_auts;
	/* const std::vector<std::vector<size_t>>& m_auts; */
	const std::vector<size_t>& m_walk;
	size_t rn;
};
	
class OOrderlyGen {
public:
	OOrderlyGen(const Graph& g, const std::vector<size_t>& m_walk);
	/* bool is_canonical(size_t v) const; */
	/* bool is_canonical() const; */
	bool is_canonical(size_t v) const {
		return is_min(v) && is_canon_shift_partial(v) && is_canon_reverse_partial(v);
	}

	bool is_canonical() const {
		return (is_canon_shift() && is_canon_reverse());
	}
	void prune(size_t v);
	void backtrack();
	const std::vector<std::vector<size_t>>& perms() const;

private:
	size_t aut(size_t i, size_t v) const;
	bool is_min(size_t v) const {
		for (size_t i : m_valid_auts) {
			if (aut(i,v) < v) { return false; }
		}
		return true;
	}
bool is_canon_shift() const {
	for (const auto& ik : m_valid_shifts) {
		size_t r = ik.first;
		size_t k = ik.second;
		size_t j = m_walk.size() - k;
		while (k < m_walk.size()) {
			size_t ik = k;
			size_t jk = (j + k) % m_walk.size();
			if (m_walk[ik] == aut(r,m_walk[jk])) { ++k; } 
			else if (m_walk[ik] > aut(r,m_walk[jk])) {
				return false;
			} else {
				break;
			}
		}
		/* size_t i = 0; */
	  	/* j = 1, k = 0; */
		/* while (j < m_walk.size() && k < m_walk.size()) { */
		/* 	size_t ik = k; */
		/* 	size_t jk = (j + k) % m_walk.size(); */
		/* 	if (m_walk[ik] == aut(r, m_walk[jk])) { ++k; } */ 
		/* 	else if (m_walk[ik] > aut(r, m_walk[jk])) { */
		/* 		std::cout << "ALEEEERT\n"; */
		/* 		return false; */
		/* 	} else { */
		/* 		j = j + k + 1; */
		/* 		if (j <= i) { j = i + 1; } */
		/* 		k = 0; */
		/* 	} */
		/* } */
	}
	return true;
}
	bool is_canon_shift_partial(size_t v) const {
		for (const auto& ik : m_valid_shifts) {
			size_t r = ik.first;
			size_t k = ik.second;
			if (m_walk[k] > aut(r,v)) { return false; }
		}
		return true;
	}
	bool is_canon_reverse() const;
	bool is_canon_reverse_partial(size_t v) const;
	/* data */
	const std::vector<size_t>& m_walk;
	size_t rn;
	/* std::vector<std::vector<size_t>> m_auts; */
	std::vector<size_t> m_auts;
	PruneVector<size_t, PruneAuts>  m_valid_auts;
	PruneVector<std::pair<size_t, size_t>, PruneShifts>  m_valid_shifts;
	PruneVector<std::pair<size_t, size_t>, PruneReverse>  m_valid_revs;
};
OOrderlyGen::OOrderlyGen(const Graph& g, const std::vector<size_t>& walk): m_walk(walk)
                                  , rn(num_vertices(g))
                                  /* , m_auts(get_all_permutations(find_automorphisms(g))) */
                                  , m_valid_auts(PruneAuts(m_auts,rn))
                                  , m_valid_shifts(PruneShifts(m_auts, walk,rn))
                                  , m_valid_revs(PruneReverse(m_auts, walk,rn)) {
	size_t nauts = 0;
	 for (const auto& row :get_all_permutations(find_automorphisms(g))) {
		 m_auts.insert(m_auts.end(), row.begin(), row.end());
		 ++nauts;
	 }
	for (size_t i = 0; i < nauts; ++i) {
		if (i > 0) {
			m_valid_auts.push_back(i * rn);
			size_t k = 0;
			if (m_walk[0] == aut(i*rn,m_walk[1])) { ++k;}
			m_valid_shifts.push_back(std::make_pair(i * rn, k));
		}
		size_t k = 0;
		if (aut(i*rn,m_walk[0]) == m_walk[1] && aut(i*rn,m_walk[1]) == m_walk[0]) { ++k; }
		m_valid_revs.push_back(std::make_pair(i * rn, k));

	}
	m_valid_auts.prune(m_walk[0]);
	m_valid_auts.prune(m_walk[1]);
}




bool OOrderlyGen::is_canon_reverse_partial(size_t v) const {
	for (auto& ik : m_valid_revs) {
		size_t r = ik.first;
		if (aut(r,v) == m_walk[0] && aut(r,m_walk.back()) == m_walk[1]) {
			for (size_t i = 2; i < m_walk.size(); ++i) {
				size_t j = m_walk.size() - i;
				if (m_walk[i] > aut(r,m_walk[j])) { return false; }
				else if (m_walk[i] < aut(r, m_walk[j])) { break; }
				else if (i == m_walk.size() - 1 && v > aut(r,m_walk[0])) { return false; }
			}
		}
	}
	/* for (const auto& aut : m_auts) { */
	/* 	if (aut[v] != m_walk[0]) { continue; } */
	/* 	for (size_t i = 1; i < m_walk.size() - 1; ++i) { */
	/* 		size_t j = m_walk.size() - i; */
	/* 		if (m_walk[i] < aut[m_walk[j]]) { break; } */
	/* 		else if (m_walk[i] > aut[m_walk[j]]) { */ 
	/* 			std::cout << "walk: " << m_walk << std::endl; */
	/* 			std::cout << "perm: "; print_permutation(m_walk, aut); */
	/* 			return false; */ 
	/* 		} */
	/* 	} */
	/* } */
	return true;
}

bool is_canon_rev_brute(const std::vector<size_t>& walk, const std::vector<size_t>& perm) {
	size_t i = 0, j = 0, k = 0;
	while (i < walk.size() && j < walk.size() && k < walk.size()) {
		size_t ik = k;
		size_t jk = walk.size() - 1 - ((j + k) % walk.size());
		if (walk[ik] == perm[walk[jk]]) { ++k; } 
		else if (walk[ik] > perm[walk[jk]]) {
			return false;
		} else {
			j = j + k + 1;
			if (j <= i) { j = i + 1; }
			k = 0;
		}
	}
	return true;
}

bool OOrderlyGen::is_canon_reverse() const {
	size_t j = 0, k = 0;
	for (const auto& ik : m_valid_revs) {
		/* const auto& aut = m_auts[ik.first]; */
		size_t r = ik.first;
		j = m_walk.size() - 1 - ik.second, k = ik.second + 1;
		assert(ik.second > 0 || (aut(r,m_walk.back()) == m_walk[1] && aut(r,m_walk[0]) == m_walk[0]));
		while (k < m_walk.size()) {
			size_t ik = k;
			size_t jk = m_walk.size() - 1 - ((j + k) % m_walk.size());
			if (m_walk[ik] == aut(r,m_walk[jk])) { ++k; } 
			else if (m_walk[ik] > aut(r,m_walk[jk])) {
				return false;
			} else { 
				break; 
			}
		}
	}
	/* for (const auto& ik : m_valid_revs) { */
	/* for (const auto& perm : m_auts) { */
	/* 	/1* const auto& perm = m_auts[ik.first]; *1/ */
	/* 	i = 0, j = 0, k = 0; */
	/* 	while (i < m_walk.size() && j < m_walk.size() && k < m_walk.size()) { */
	/* 		size_t ik = k; */
	/* 		size_t jk = m_walk.size() - 1 - ((j + k) % m_walk.size()); */
	/* 		if (m_walk[ik] == perm[m_walk[jk]]) { ++k; } */ 
	/* 		else if (m_walk[ik] > perm[m_walk[jk]]) { */
	/* 				std::cout << "walk: " << m_walk << std::endl; */
	/* 				std::cout << "perm: "; */
	/* 				print_permutation(m_walk, perm); */
	/* 				std::cout << "k: " << k << " j: " << j << " jk: " << jk; */
	/* 				std::cout << std::endl; */
	/* 			return false; */
	/* 		} else { */
	/* 			j = j + k + 1; */
	/* 			if (j <= i) { j = i + 1; } */
	/* 			k = 0; */
	/* 		} */
	/* 	} */
	/* } */
	return true;
}


void OOrderlyGen::prune(size_t v) {
	m_valid_revs.prune(v);
	m_valid_shifts.prune(v);
	m_valid_auts.prune(v);
}

void OOrderlyGen::backtrack() {
	for (auto& ik : m_valid_shifts) {
		/* const auto& aut = m_auts[ik.first]; */
		size_t r = ik.first;
		size_t& k = ik.second;
		/* if (k > 0 && m_walk[k-1] != aut[m_walk[m_walk.size() - 1]]) { */
		/* 	std::cout << "k: " << k  << std::endl; */
		/* 	std::cout << "walk: " << m_walk << std::endl; */
		/* 	std::cout << "perm: "; print_permutation(m_walk, aut); */
		/* } */
		/* assert(k == 0 || m_walk[k-1] == aut[m_walk[m_walk.size() - 1]]); */
		if (k == 0 && aut(r,m_walk[m_walk.size() - 2]) == m_walk[0]) { ++k; }
		else if (k > 0) { --k; }
	}
	for (auto& ik : m_valid_revs) {
		/* const auto& aut = m_auts[ik.first]; */
		size_t r = ik.first;
		size_t& k = ik.second;
		if (aut(r,m_walk.back()) == m_walk[0] && aut(r,m_walk[m_walk.size() - 2]) == m_walk[1]) {
			k = 0;
		}
	}
	m_valid_auts.backtrack();
	m_valid_shifts.backtrack();
	m_valid_revs.backtrack();
}

inline size_t OOrderlyGen::aut(size_t i, size_t v) const {
	return m_auts[i + v];
}

/* const std::vector<std::vector<size_t>>& OOrderlyGen::perms() const { return m_auts; } */

} /* namespace prune */
} /* namespace kface */
#endif /* ifndef KFACE_PRUNE_ORDERLY_GEN2_HPP */
