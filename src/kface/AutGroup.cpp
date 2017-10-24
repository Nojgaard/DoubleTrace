#include "AutGroup.hpp"
#include <morphism/aut.h>
#include "Util.hpp"
#include <cassert>

namespace kface {

using namespace morphism;

PruneAuts::PruneAuts(const std::vector<std::vector<size_t>>& a): m_auts(a){}
bool PruneAuts::operator () (size_t v, size_t i) { return m_auts[i][v] != v; }

PruneShifts::PruneShifts(const std::vector<std::vector<size_t>>& a,const std::vector<size_t>& w)
		: m_auts(a), m_walk(w) {}
bool PruneShifts::operator () (size_t v, std::pair<size_t, size_t>& ik) {
	const auto& aut = m_auts[ik.first];
	size_t& k = ik.second;

	if (m_walk[k] == aut[v]) { ++k; }
	else if (k > 1 && aut[v] > m_walk[k]) { return true; }
	else { k = 0; }
	return false;
}

PruneReverse::PruneReverse(const std::vector<std::vector<size_t>>& a,const std::vector<size_t>& w)
		: m_auts(a), m_walk(w) {}

bool PruneReverse::operator () (size_t v, std::pair<size_t, size_t>& ik) {
	const auto& aut = m_auts[ik.first];
	size_t& k = ik.second;
	if (k > 0) { return false; }
	else if (aut[v] == m_walk[0] && aut[m_walk.back()] == m_walk[1]) {
		for (size_t i = 2; i < m_walk.size(); ++i) {
			size_t j = m_walk.size() - i;
			assert(m_walk[i] <= aut[m_walk[j]]);
			if (m_walk[i] < aut[m_walk[j]]) { 
				return true; }
		}
		if (v < aut[m_walk[0]]) { return true; }
		k = m_walk.size();
	}
	return false;
}

AutGroup::AutGroup(const Graph& g, const std::vector<size_t>& walk)
                                  : m_walk(walk)
                                  , m_auts(get_all_permutations(find_automorphisms(g), num_vertices(g)))
                                  , m_valid_auts(PruneAuts(m_auts))
                                  , m_valid_shifts(PruneShifts(m_auts, m_walk))
                                  , m_valid_revs(PruneReverse(m_auts, m_walk))	{
	for (size_t i = 0; i < m_auts.size(); ++i) {
		if (i > 0) {
			m_valid_auts.push_back(i);
			size_t k = 0;
			if (m_walk[0] == m_auts[i][m_walk[1]]) { ++k;}
			m_valid_shifts.push_back(std::make_pair(i, k));
		}
		size_t k = 0;
		if (m_auts[i][m_walk[0]] == m_walk[1] && m_auts[i][m_walk[1]] == m_walk[0]) { ++k; }
		m_valid_revs.push_back(std::make_pair(i, k));

	}
	prune_auts(m_walk[0]);
	prune_auts(m_walk[1]);
	/* m_valid_auts.prune(m_walk[0]); */
	/* m_valid_auts.prune(m_walk[1]); */
}

bool AutGroup::is_min(size_t v) const {
	for (size_t i : m_valid_auts) {
		if (m_auts[i][v] < v) { return false; }
	}
	return true;
}

bool AutGroup::is_canon_shift_partial(size_t v) const {
	/* std::cout << "WALK: " << m_walk << std::endl; */
	for (const auto& ik : m_valid_shifts) {
		const auto& aut = m_auts[ik.first];
		size_t k = ik.second;
		/* if (k == 0 && aut[m_walk.back()] == m_walk[0]) { ++k; } */
			/* std::cout << "aut: " << aut << std::endl; */
			/* std::cout << "k: " << k << std::endl; */
		/* std::cout << "k: " << k << " v: " << aut[v] << std::endl; */
		/* std::cout << "walk: " << m_walk << std::endl; */
		/* std::cout << "perm: "; print_permutation(m_walk, aut); */
		if (m_walk[k] > aut[v]) { 
			/* std::cout << "PRUNED (" << v << ")\n"; */

			return false; 
		}
	}
	return true;
}

bool AutGroup::is_canon_shift() const {
	for (const auto& ik : m_valid_shifts) {
		const auto& aut = m_auts[ik.first];
		size_t k = ik.second;
		size_t i = 0, j = m_walk.size() - k;
		/* std::cout << "walk: " << m_walk << std::endl; */
		/* std::cout << "perm: "; */
		/* print_permutation(m_walk, aut); */
		/* std::cout << "k: " << k << "j: " << j; */
		/* std::cout << std::endl; */
		while (k < m_walk.size()) {
			size_t ik = k;
			size_t jk = (j + k) % m_walk.size();
			if (m_walk[ik] == aut[m_walk[jk]]) { ++k; } 
			/* else { return m_walk[ik] < aut[m_walk[jk]]; } */
			else if (m_walk[ik] > aut[m_walk[jk]]) {
				return false;
			} else {
				break;
			}
		}
		/* i = 0, j = 1, k = 0; */
		/* while (j < m_walk.size() && k < m_walk.size()) { */
		/* 	size_t ik = k; */
		/* 	size_t jk = (j + k) % m_walk.size(); */
		/* 	if (m_walk[ik] == aut[m_walk[jk]]) { ++k; } */ 
		/* 	else if (m_walk[ik] > aut[m_walk[jk]]) { */
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

bool AutGroup::is_canon_reverse_partial(size_t v) const {
	for (auto& ik : m_valid_revs) {
		const auto& aut = m_auts[ik.first];
		if (aut[v] == m_walk[0] && aut[m_walk.back()] == m_walk[1]) {
			for (size_t i = 2; i < m_walk.size(); ++i) {
				size_t j = m_walk.size() - i;
				/* assert(m_walk[i] >= aut[m_walk[j]]); */
				if (m_walk[i] > aut[m_walk[j]]) { return false; }
				else if (m_walk[i] < aut[m_walk[j]]) { break; }
				else if (i == m_walk.size() - 1 && v > aut[m_walk[0]]) { return false; }
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

bool AutGroup::is_canon_reverse() const {
	size_t i = 0, j = 0, k = 0;
	for (const auto& ik : m_valid_revs) {
		const auto& aut = m_auts[ik.first];
		i = 0, j = m_walk.size() - 1 - ik.second, k = ik.second + 1;
		assert(ik.second > 0 || (aut[m_walk.back()] == m_walk[1] && aut[m_walk[0]] == m_walk[0]));
		size_t oj = j, ok = k;
		while (k < m_walk.size()) {
			size_t ik = k;
			size_t jk = m_walk.size() - 1 - ((j + k) % m_walk.size());
			if (m_walk[ik] == aut[m_walk[jk]]) { ++k; } 
			else if (m_walk[ik] > aut[m_walk[jk]]) {
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

bool AutGroup::is_canonical(size_t v) const {
	return (is_canon_shift_partial(v) && is_canon_reverse_partial(v));
}

bool AutGroup::is_canonical() const {
	return (is_canon_shift() && is_canon_reverse());
}

void AutGroup::prune_auts(size_t v) {
	m_valid_auts.prune(v, [&](size_t v, size_t i) { return m_auts[i][v] != v; });
}

void AutGroup::prune_shifts(size_t v) {
	m_valid_shifts.prune(v, [&](size_t v, std::pair<size_t, size_t>& ik) { 
			const auto& aut = m_auts[ik.first];
			size_t& k = ik.second;

			if (m_walk[k] == aut[v]) { ++k; }
			else if (k > 1 && aut[v] > m_walk[k]) { return true; }
			else { k = 0; }
			return false;
			});
}

void AutGroup::prune_revs(size_t v) {
	m_valid_revs.prune(v);
}

void AutGroup::prune(size_t v) {
	prune_auts(v);
	prune_shifts(v);
	prune_revs(v);
}

void AutGroup::backtrack() {
	for (auto& ik : m_valid_shifts) {
		const auto& aut = m_auts[ik.first];
		size_t& k = ik.second;
		/* if (k > 0 && m_walk[k-1] != aut[m_walk[m_walk.size() - 1]]) { */
		/* 	std::cout << "k: " << k  << std::endl; */
		/* 	std::cout << "walk: " << m_walk << std::endl; */
		/* 	std::cout << "perm: "; print_permutation(m_walk, aut); */
		/* } */
		/* assert(k == 0 || m_walk[k-1] == aut[m_walk[m_walk.size() - 1]]); */
		if (k == 0 && aut[m_walk[m_walk.size() - 2]] == m_walk[0]) { ++k; }
		else if (k > 0) { --k; }
	}
	for (auto& ik : m_valid_revs) {
		const auto& aut = m_auts[ik.first];
		size_t& k = ik.second;
		if (aut[m_walk.back()] == m_walk[0] && aut[m_walk[m_walk.size() - 2]] == m_walk[1]) {
			k = 0;
		}
	}
	m_valid_auts.backtrack();
	m_valid_shifts.backtrack();
	m_valid_revs.backtrack();
}

const std::vector<std::vector<size_t>>& AutGroup::perms() const { return m_auts; }
	
} /* namespace kface */
