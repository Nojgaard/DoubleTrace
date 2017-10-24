#include "None.hpp"
#include <morphism/aut.h>

namespace kface {
namespace prune {
using namespace morphism;

	
None::None(const Graph& g, const std::vector<size_t>& walk): m_walk(walk)
	, m_auts(get_all_permutations(find_automorphisms(g))) {
		/* std::cout << m_auts.size() << std::endl; */
		if (m_auts.size() == 0) {
			std::vector<size_t> id(num_vertices(g));
			for (size_t i = 0; i < num_vertices(g); ++i) {
				id[i] = i;
			}
			m_auts.push_back(id);
		}
	/* , m_valid_auts(Dummy()) { */
	/* for (size_t i = 0; i < m_auts.size(); ++i) { */
	/* 	if (i > 0) { */
	/* 		m_valid_auts.push_back(i); */
	/* 	} */
	/* } */
	/* prune(m_walk[0]); */
	/* prune(m_walk[1]); */
}

bool None::is_canonical(size_t v) const { 
	/* for (size_t i : m_valid_auts) { */
	/* 	if (m_auts[i][v] < v) { return false; } */
	/* } */
	return true; 
}

bool None::is_canonical() const {
	for (auto& perm : m_auts) {
		for (size_t i = 0; i < m_walk.size(); ++i) {
			if (m_walk[i] < perm[m_walk[i]]) {
				break;
			} else if (m_walk[i] > perm[m_walk[i]]) {
				return false;
			}
		}
		size_t i = 0, j = 0, k = 0;
		while (i < m_walk.size() && j < m_walk.size() && k < m_walk.size()) {
			size_t ik = k;
			size_t jk = (j + k) % m_walk.size();
			if (m_walk[ik] == perm[m_walk[jk]]) { ++k; } 
			else if (m_walk[ik] > perm[m_walk[jk]]) {
				return false;
			} else {
				j = j + k + 1;
				if (j <= i) { j = i + 1; }
				k = 0;
			}
		}
		i = 0, j = 0, k = 0;
		while (i < m_walk.size() && j < m_walk.size() && k < m_walk.size()) {
			size_t ik = k;
			size_t jk = m_walk.size() - 1 - ((j + k) % m_walk.size());
			if (m_walk[ik] == perm[m_walk[jk]]) { ++k; } 
			else if (m_walk[ik] > perm[m_walk[jk]]) {
				return false;
			} else {
				j = j + k + 1;
				if (j <= i) { j = i + 1; }
				k = 0;
			}
		}
	}
	return true;
}

void None::prune(size_t v) { /*m_valid_auts.prune(v, [&](size_t v, size_t i) { return m_auts[i][v] != v;});*/}
void None::backtrack() { /*m_valid_auts.backtrack(); */}

} /* namespace prune */
} /* namespace kface */
