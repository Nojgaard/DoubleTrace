#include "Group.hpp"
#include <morphism/aut.h>
#include "Util.hpp"
#include <cassert>

namespace kface {

using namespace morphism;

/* Group::Group(const std::vector<std::vector<size_t>>& perms): perms(perms) { */
/* 	for (size_t i = 0; i < perms.size(); ++i) { */
/* 		valid_perms.push_back(i); */
/* 	} */
/* } */

Group::Group(const Graph& g): m_perms(get_all_permutations(find_automorphisms(g))) {
	/* std::cout << "TEST:\n"; */
	/* for (auto& p : find_automorphisms(g)) { */
	/* 	std::cout << p << std::endl; */
	/* } */
	/* std::cout << "------\n"; */
	for (size_t i = 0; i < m_perms.size(); ++i) {
		valid_perms.push_back(i);
	}
}

const std::vector<std::vector<size_t>>& Group::perms() const {
	return m_perms;
}

bool Group::is_min(size_t v) const {
	for (size_t i : valid_perms) {
		if (m_perms[i][v] < v) { return false; }
	}
	return true;
}

void Group::stabilize(size_t v) {
	checkpoint.push_back(discard_perms.size());

	for (size_t i = 0; i < valid_perms.size(); ++i) {
		if (m_perms[valid_perms[i]][v] == v) { continue; }
		
		discard_perms.push_back(valid_perms[i]);
		std::swap(valid_perms[i], valid_perms.back());
		valid_perms.pop_back();
		--i;
	}
}

void Group::backtrack() {
	assert(checkpoint.size() > 0);
	valid_perms.insert(valid_perms.end(), discard_perms.begin()+checkpoint.back(), discard_perms.end());
	discard_perms.erase(discard_perms.begin() + checkpoint.back(), discard_perms.end());
	checkpoint.pop_back();
}

std::ostream& operator << (std::ostream& os, const Group& g) {
	for (size_t i : g.valid_perms) {
		for (size_t v : g.m_perms[i]) {
			os << v << " ";
		}
		os << std::endl;
	}
	return os;
}

} /* namespace kface */
