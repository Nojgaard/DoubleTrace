#include <dtrace/perm_list.h>
#include <iostream>

namespace dtrace {
	PermList::PermList(const std::vector<Perm> &perms): perms(perms) { 
		for (size_t i = 0; i < perms.size(); ++i) {
			perms_idx.push_back(i);
		}
	}

	PermList::PermList(Vertex fix_v, const std::vector<Perm> &perms): perms(perms) {
		for (size_t i = 0; i < perms.size(); ++i) {
			if (perms.at(i).at(fix_v) == fix_v){
				perms_idx.push_back(i);
			}
		}
	}

	const std::vector<Perm>& PermList::get_perms() const {
		return perms;
	}

	bool PermList::empty() const {
		return perms_idx.empty();
	}

	PermList::PermList(Vertex fix_v, const PermList &pl): perms(pl.perms) {
		for (size_t i : pl.perms_idx) {
			if (perms.at(i).at(fix_v) == fix_v){
				perms_idx.push_back(i);
			}
		}
	}

	bool PermList::is_min_perm(Vertex v) const {
		for (size_t i : perms_idx) {
			if (perms.at(i).at(v) < v) { return false; }
		}
		return true;
	}

	void PermList::fix(Vertex v) {
		std::vector<size_t> new_pidx;
		for (size_t i : perms_idx) {
			if (perms[i][v] == v) {
				new_pidx.push_back(i);
			}
		}
		perms_idx.swap(new_pidx);
	}

	std::pair<Vertex, Vertex> PermList::get_min(const Edge& e, const Graph& g) const {
		Vertex src = source(e, g), tar = target(e, g);
		if (tar < src) { std::swap(src, tar); }

		Vertex tmp_src = src, tmp_tar = tar;
		do {
			tmp_src = src;
			tmp_tar = tar;
			for (size_t i : perms_idx) {
				if (perms[i][src] < src || (perms[i][src] == src && perms[i][tar] < tar)) {
					src = perms[i][src];
					tar = perms[i][tar];
				}
			}
		} while (src != tmp_src || tmp_tar != tar);
		return std::make_pair(src, tar);
	}

	std::ostream & operator << (std::ostream &os, const PermList &pl) {
		for (size_t idx : pl.perms_idx) {
			for (Vertex v : pl.perms.at(idx)) {
				os << v << " ";
			}
			os << std::endl;
		}
		return os;
	}
} /* namespace dtrace */
