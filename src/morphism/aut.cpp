#include <morphism/aut.h>
#include <iostream>
#include "../BAD_TIME.hpp"
/* #include <bliss/graph.hh> */
/* #include <cassert> */
/* #include <set> */
/* #include <iostream> */

namespace kface {
	volatile bool cont = true;
	
} /* namespace kface */

namespace morphism {
	
std::vector<size_t> permute_trace(const Perm& trace, const Perm& aut) {
	Perm out(trace.size());
	for (size_t i = 0; i < trace.size(); ++i) {
		out[i] = aut[trace[i]];
	}
	return out;
}

Perm permute(const Perm& src, const Perm& tar) {
	assert(src.size() == tar.size());
	Perm out(src.size());
	for (size_t i = 0; i< src.size(); i++) {
		out[src[i]] = tar[i];
	}
	return out;
}

std::vector<Perm> get_all_permutations(const std::vector<Perm> &auts, size_t n) {
	/* for (const auto& a : auts) { */
	/* 	for (auto p : a) { */
	/* 		std::cout << p << " "; */
	/* 	} */
	/* 	std::cout << std::endl; */
	/* } */
	if (auts.size() == 0) { 
		if (n == 0) {
			return std::vector<Perm>(); 
		} else {
			Perm trivial;
			for (size_t i = 0; i < n; ++i) {
				trivial.push_back(i);
			}
			return std::vector<Perm>({trivial});
		}
	}
	std::set<Perm> perms;
	Perm trivial;
	for (size_t i = 0; i < auts[0].size(); ++i) {
		trivial.push_back(i);
	}
	perms.insert(trivial);
	/* for (auto &aut : auts) { */
	/* 	perms.insert(aut); */
	/* } */
	size_t old_size = 0;
	while (perms.size() != old_size) {
		/* std::cout << perms.size() << std::endl; */
		old_size = perms.size();
		std::set<Perm> new_perms;
		for (auto& aut : auts) {
			for (auto& perm : perms) {
				new_perms.insert(permute_trace(aut, perm));
			}
			if (!kface::cont) {
				return std::vector<Perm> ();
			}
			/* if (new_perms.size() > 400000) { */
			/* 	throw 0; */
			/* } */
		}
		perms.insert(new_perms.begin(), new_perms.end());
	}
	/* do { */
	/* 	old_size = perms.size(); */
	/* 	std::set<Perm> tmp_perms; */
	/* 	for (auto &aut : auts) { */
	/* 		for (auto &perm : perms) { */
	/* 			tmp_perms.insert(permute(aut, perm)); */
	/* 		} */
	/* 	} */
	/* 	perms.insert(tmp_perms.begin(), tmp_perms.end()); */
	/* } while (old_size != perms.size()); */
	std::vector<Perm> out;
	for (auto &perm : perms) {
		out.push_back(perm);
	}
	return out;
}

Group perm2group(const Perm& p) {
	Group g;
	std::vector<bool> vis(p.size(), false);
	for (size_t i = 0; i < p.size(); ++i) {
		if (p[i] == i || vis[p[i]]) { continue; }
		g.push_back(std::vector<size_t>());

		size_t cur = p[i];
		while (!vis[cur]) {
			vis[cur] = true;
			g.back().push_back(cur);
			cur = p[cur];
		}
		/* g.push_back(std::make_pair(i, p[i])); */
	}
	return g;
}

} /* namespace morphism */
