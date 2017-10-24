#include "KNone.hpp"
#include "../Util.hpp"
#include <morphism/aut.h>

namespace kface {
namespace prune {

using namespace morphism;

KNone::KNone(const Graph& g, const std::vector<size_t>& walk, const std::vector<size_t>& fidx):
	m_walk(walk), m_fidx(fidx), m_auts(get_all_permutations(find_automorphisms(g))) { }

bool KNone::is_canonical(size_t v) const { return true; }

bool KNone::is_canonical_auts(const std::vector<size_t>& aut) const {
	for (auto v : m_walk) {
		if (v > aut[v]) { return false; }
		else if (v < aut[v]) { return true; }
	}
	return true;
}

size_t KNone::find_min_rev(const std::vector<size_t>& aut, size_t f) const {
	size_t fsize = ((f == m_fidx.size() - 1) ? m_walk.size() : m_fidx[f + 1]) - m_fidx[f];
	size_t min_idx = 0;
	for (size_t i = 1; i < fsize; ++i) {
		size_t src = aut[m_walk[m_fidx[f] + i]];
		size_t tar = aut[m_walk[m_fidx[f] + (i - 1)]];
		size_t min_src = aut[m_walk[m_fidx[f] + min_idx]];
		size_t min_tar = aut[m_walk[m_fidx[f] + ((min_idx == 0) ? fsize - 1 : min_idx-1)]];
		/* std::cout << "msrc: " << min_src << " mtar: " << min_tar << std::endl; */
		/* std::cout << "src: " << src << " tar: " << tar << std::endl; */
		if (src < min_src || (src == min_src && tar < min_tar)) {
			min_idx = i;
		}
	}
	return min_idx;
}

bool KNone::is_canonical_revs(const std::vector<size_t>& aut) const {
	size_t aut_widx = -1, aut_fidx = 0;
	for (size_t f = 0; f < m_fidx.size(); ++f) {
		size_t i = find_min_rev(aut, f);
		size_t fend = (f == m_fidx.size() - 1) ? m_walk.size() : m_fidx[f+1];
		size_t fsize = fend - m_fidx[f];
		size_t j = m_fidx[f] + ((i == 0) ? fsize - 1 : i-1);
		if (m_walk[0] == aut[m_walk[m_fidx[f] + i]] && m_walk[1] == aut[m_walk[j]]) {
			aut_widx = i;
			aut_fidx = f;
			break;
		}
	}
	assert(aut_widx != (size_t)-1);
	std::cout << "IMHER " << aut_widx << " af: " << aut_fidx << "\n";
	size_t k = 0, i = 0, j = aut_widx, f = 0, af = aut_fidx;
	size_t afsize = ((af == m_fidx.size() - 1) ? m_walk.size() : m_fidx[af + 1]) - m_fidx[af];
	j = afsize - 1 - aut_widx;
	/* while (k < m_walk.size()) { */
	while (f < m_fidx.size()) {
		size_t fsize = ((f == m_fidx.size() - 1) ? m_walk.size() : m_fidx[f + 1]) - m_fidx[f];
		afsize = ((af == m_fidx.size() - 1) ? m_walk.size() : m_fidx[af + 1]) - m_fidx[af];
		/* std::cout << "j: " << j << std::endl; */

		if (k == fsize || k == afsize) {
			std::cout << "FACE DONE!\n";
			if (fsize < afsize) { return true; }
			else if (fsize > afsize) { return false; }

			++f; af = (af == 0) ? m_fidx.size() - 1 : af - 1;
			if (f == m_fidx.size()) { return true; }
			fsize = ((f == m_fidx.size() - 1) ? m_walk.size() : m_fidx[f + 1]) - m_fidx[f];
			afsize = ((af == m_fidx.size() - 1) ? m_walk.size() : m_fidx[af + 1]) - m_fidx[af];
			j = afsize - 1 - find_min_rev(aut, af);
			k = 0;
		}

		size_t ik = m_fidx[f] + k;
		size_t jk = m_fidx[af] + (afsize - 1 - ((j + k) % afsize));
		std::cout << "afs: " << afsize << " k: " << k << std::endl;
		std::cout << "af: " << af << " ik: " << ik << " jk: " << jk << std::endl;

		if (m_walk[ik] == aut[m_walk[jk]]) { ++k; }
		else if (m_walk[ik] < aut[m_walk[jk]]) { return true; }
		else { return false; }

	}
	return true;
}

size_t KNone::find_min_shift(const std::vector<size_t>& aut, size_t f) const {
	size_t fsize = ((f == m_fidx.size() - 1) ? m_walk.size() : m_fidx[f + 1]) - m_fidx[f];
	size_t min_idx = 0;
	for (size_t i = 1; i < fsize; ++i) {
		size_t src = aut[m_walk[m_fidx[f] + i]];
		size_t tar = aut[m_walk[m_fidx[f] + ((i + 1) % fsize)]];
		size_t min_src = aut[m_walk[m_fidx[f] + min_idx]];
		size_t min_tar = aut[m_walk[m_fidx[f] + ((min_idx + 1) % fsize)]];
		if (src < min_src || (src == min_src && tar < min_tar)) {
			min_idx = i;
		}
	}
	return min_idx;
}

bool KNone::is_canonical_shifts(const std::vector<size_t>& aut) const {
	/* std::cout << "SHIFTING\n"; */
	size_t aut_widx = 0, aut_fidx = 0;
	for (size_t f = 0; f < m_fidx.size(); ++f) {
		size_t i = find_min_shift(aut, f);
		size_t fend = (f == m_fidx.size() - 1) ? m_walk.size() : m_fidx[f+1];
		size_t fsize = fend - m_fidx[f];
		size_t j = m_fidx[f] + ((i + 1) % fsize);
		if (m_walk[0] == aut[m_walk[m_fidx[f] + i]] && m_walk[1] == aut[m_walk[j]]) {
			aut_widx = i;
			aut_fidx = f;
			break;
		}
	}
	/* std::cout << "IMHER " << aut_widx << " af: " << aut_fidx << "\n"; */
	
	size_t k = 0, i = 0, j = aut_widx, f = 0, af = aut_fidx;
	while (f < m_fidx.size()) {
		size_t fsize = ((f == m_fidx.size() - 1) ? m_walk.size() : m_fidx[f + 1]) - m_fidx[f];
		size_t afsize = ((af == m_fidx.size() - 1) ? m_walk.size() : m_fidx[af + 1]) - m_fidx[af];

		if (k == fsize || k == afsize) {
			if (fsize < afsize) { return true; }
			else if (fsize > afsize) { return false; }

			++f; af = (af + 1) % m_fidx.size();
			if (f == m_fidx.size()) { return true; }
			j = find_min_shift(aut, af);
			fsize = ((f == m_fidx.size() - 1) ? m_walk.size() : m_fidx[f + 1]) - m_fidx[f];
			afsize = ((af == m_fidx.size() - 1) ? m_walk.size() : m_fidx[af + 1]) - m_fidx[af];
			/* std::cout << "STARTING NEW FACE f: " << f << " af: " << af << " fs " << fsize << " afs: " << afsize << "\n"; */
			k = 0;
		}

		size_t ik = m_fidx[f] + ((i + k) % fsize);
		size_t jk = m_fidx[af] + ((j + k) % afsize);
		/* std::cout << "afs: " << afsize << " k: " << k << std::endl; */
		/* std::cout << "af: " << af << " ik: " << ik << " jk: " << jk << std::endl; */

		if (m_walk[ik] == aut[m_walk[jk]]) { ++k; }
		else if (m_walk[ik] < aut[m_walk[jk]]) { return true; }
		else { return false; }

	}
	return true;
}

bool KNone::is_canonical() const { 
	std::cout << "fidx: " << m_fidx << std::endl;
	std::cout << "walk: " << m_walk << std::endl;
	for (const auto& aut : m_auts) {
		std::cout << "perm: "; print_permutation(m_walk, aut);
		if (!is_canonical_auts(aut) || !is_canonical_shifts(aut) || !is_canonical_revs(aut)) { 
			return false; 
		}
	}
	return true; 
}

void KNone::prune(size_t v) { }
void KNone::backtrack() { }

} /* namespace prune */
} /* namespace kface */
