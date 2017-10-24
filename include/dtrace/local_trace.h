#ifndef DTRACE_LOCAL_TRACE_H
#define DTRACE_LOCAL_TRACE_H
#include <dtrace/graph/base.h>
#include <iostream>

namespace dtrace {

class LocalTrace {
public:
	LocalTrace(Vertex v, const Graph &g);
	bool walk_vertex(Vertex from, Vertex to, bool allow_exc = false);
	void backtrack_vertex(Vertex from, Vertex to);
	int num_exc() const;
	friend std::ostream& operator << (std::ostream& os, const LocalTrace& l);
private:
	bool is_connected(Vertex v, Vertex u) const;
	int full_walks, degree, _num_exc = 0;
	Vertex v_exc, ov;
	bool can_ignore = false;
	std::vector<std::vector<Vertex>> local_walk;
};

} /* namespace dtrace */
#endif /* ifndef DTRACE_TRACE_LOCAL */
