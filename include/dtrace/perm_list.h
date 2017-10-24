#ifndef DTRACE_AUT_LIST_H
#define DTRACE_AUT_LIST_H

#include <morphism/aut.h>
#include <dtrace/graph.h>

namespace dtrace {

using namespace morphism;
class PermList {
public:
	PermList(Vertex fix_v, const std::vector<Perm> &perms);
	PermList(const std::vector<Perm> &perms);
	PermList(Vertex fix_v, const PermList &pl);

	std::pair<Vertex, Vertex> get_min(const Edge& e, const Graph& g) const;
	void fix(Vertex v);
	const std::vector<Perm>& get_perms() const;
	bool is_min_perm(Vertex v) const;
	bool empty() const;
	friend std::ostream & operator << (std::ostream &os, const PermList &pl);

private:
	const std::vector<Perm> &perms;
	std::vector<size_t> perms_idx;
};

} /* namespace dtrace */
#endif /* ifndef DTRACE_AUT_LIST_H */
