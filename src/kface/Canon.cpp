#include "Canon.hpp"
#include "Util.hpp"

namespace kface {

Canon::Canon(const AutGroup& g, const std::vector<size_t>& walk, size_t full_size): 
	m_group(g), m_walk(walk), hist(g.perms().size()), m_full_size(full_size) {
		for (size_t p = 0; p < hist.size(); ++p) {
			size_t k = 0;
			if (walk[0] == g.perms()[p][walk[1]]) { ++k;}
			hist[p].push_back(0);
			hist[p].push_back(k);
		}
}

bool Canon::extensive_canon(size_t tk, const std::vector<size_t>& perm) const {
	size_t i = 0, j = m_walk.size() - 1 - tk, k = tk;
	while (j < m_walk.size() && k < m_walk.size()) {
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
	/* size_t i = 0, j = 1, k = 0; */
	/* while (i < m_walk.size() && j < m_walk.size() && k < m_walk.size()) { */
	/* 	size_t ik = k; */
	/* 	size_t jk = (j + k) % m_walk.size(); */
	/* 	if (m_walk[ik] == perm[m_walk[jk]]) { ++k; } */ 
	/* 	else if (m_walk[ik] > perm[m_walk[jk]]) { */
	/* 		return false; */
	/* 	} else { */
	/* 		j = j + k + 1; */
	/* 		if (j <= i) { j = i + 1; } */
	/* 		k = 0; */
	/* 	} */
	/* } */
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
	return true;
}

bool Canon::is_canonical() const {
	for (size_t p = 0; p < hist.size(); ++p) {
		auto& perm = m_group.perms()[p];
		/* if (perm[m_walk[0]] != m_walk[0] && perm[m_walk[1]] != m_walk[1]) { continue; } */

		size_t k = hist[p][hist[p].size() - 2];
		if (m_walk[k] > perm[m_walk.back()]) { return false; }

		for (size_t i = 0; i < m_walk.size(); ++i) {
			size_t j = m_walk.size()- 1 - i;
			if (m_walk[i] < perm[m_walk[j]]) { break; }
			else if (m_walk[i] > perm[m_walk[j]]) { return false; }
		}

		if (m_walk.size() == m_full_size && !extensive_canon(k, perm)) {
			return false;
		}
	}
	return true;
}

void Canon::extend(Vertex v) {
	for (size_t p = 0; p < hist.size(); ++p) {
		size_t k = hist[p].back();
		if (m_walk[k] == m_group.perms()[p][v]) { ++k;}
		else { k = 0; }
		hist[p].push_back(k);
	}
}

void Canon::backtrack() {
	for (auto& h : hist) {
		h.pop_back();
	}
}

} /* namespace kface */
