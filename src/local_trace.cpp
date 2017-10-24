#include <dtrace/local_trace.h>
#include <cassert>
#include <iostream>

namespace dtrace {

LocalTrace::LocalTrace(Vertex v, const Graph &g): full_walks(0), degree(boost::degree(v, g)), ov(v)
												  , local_walk(num_vertices(g)) { }

bool LocalTrace::walk_vertex(Vertex from, Vertex to, bool allow_exc) {
	assert(from < local_walk.size() && to < local_walk.size());
	assert(local_walk[from].size() < 2 && local_walk[to].size() < 2);
	if (degree == 1 || can_ignore) { return true; }
	else if (from == to && !allow_exc) { return false; }
	else if (from == to) { 
	  	assert(local_walk[from].size() == 0); 
		--degree;
		++full_walks;
		++_num_exc;
		v_exc = from;
	}

	bool closes_segment = local_walk[from].size() == 1 && local_walk[to].size() == 1;
	if (closes_segment && full_walks+2 < degree) {
		if (allow_exc && (full_walks+2 == degree - 1) && is_connected(from, to)) {
			/* ++_num_exc; */
			/* v_exc = to; */
			/* can_ignore = true; */
			/* std::cout <<  *this  << std::endl; */
		} else if (is_connected(from, to)) { return false; }
	}

	local_walk[from].push_back(to);
	local_walk[to].push_back(from);
	if (from != to) {
		full_walks += local_walk[from].size()/2 + local_walk[to].size()/2;
	}
	return true;
}

void LocalTrace::backtrack_vertex(Vertex from, Vertex to) {
	if (degree == 1 && (num_exc() == 0 || from != v_exc)) { return; }
	if (from == to && can_ignore) { return; }
	/* assert(from != to); */
	assert(full_walks >= 0);
	assert(from < local_walk.size() && to < local_walk.size());
	assert(local_walk[from].size() > 0 && local_walk[to].size() > 0);

	if (local_walk[from].size() == 2 && local_walk[from].at(0) == to) {
		std::swap(local_walk[from][0], local_walk[from][1]);
	}
	if (local_walk[to].size() == 2 && local_walk[to].at(0) == from) {
		std::swap(local_walk[to][0], local_walk[to][1]);
	}
	assert(local_walk[from].back() == to && local_walk[to].back() == from);
	if (from != to) {
		/* if (can_ignore && to == v_exc) { */
		/* 	can_ignore = false; */
		/* 	--_num_exc; */
		/* } */

		full_walks -= local_walk[from].size()/2 + local_walk[to].size()/2;
	} else {
		--_num_exc;
		--full_walks;
		++degree;
	}
	assert(full_walks >= 0);

	local_walk[from].pop_back();
	local_walk[to].pop_back();
}

bool LocalTrace::is_connected(Vertex v, Vertex u) const {
	assert(v != u);
	Vertex curr = v, prev = v;
	while (true) {
		if (curr == u) { return true; }
		bool found = false;
		for (Vertex v : local_walk[curr]) {
			if (v != prev) {
				prev = curr;
				curr = v;
				found = true;
				break;
			}
		}
		if (!found) {return false;}
	}
}

int LocalTrace::num_exc() const {
	return _num_exc;
}

std::ostream& operator << (std::ostream& os, const LocalTrace& l) {
	os << "(d: " << l.degree << ", e: " << l.num_exc() << ", f: " << l.full_walks << ") ";
	for (size_t i = 0; i < l.local_walk.size(); ++i) {
		if (l.local_walk[i].size() == 0) { continue; }
		os << i << ": ";
		for (Vertex v : l.local_walk[i]) {
			os << v << " ";
		}
		os << "| ";
	}
	return os;
}

} /* namespace dtrace */
