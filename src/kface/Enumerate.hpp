#ifndef KFACE_ENUMERATE_HPP
#define KFACE_ENUMERATE_HPP
#include <kface/Graph.hpp>
#include <vector>

namespace kface {

struct NoCallback {
	void operator () () {}
};

template <typename CallbackFound, typename Trace>
void extend(Trace& t, CallbackFound& cb_found) {
	auto cb_always = [](const std::vector<Vertex>&){ return true; };
	extend(t, cb_found, cb_always);
}

template <typename CallbackContinue, typename CallbackFound, typename Trace, typename CallbackMiss = NoCallback>
void extend(Trace& t,  CallbackFound& cb_found, CallbackContinue cb_cont, CallbackMiss cb_miss = CallbackMiss{}) {
	/* if (!t.is_canonical()) { return; } */
	/* std::cout << t << std::endl;; */
	if (!cb_cont(t.walk())) { return; }
	if (t.is_partial()) {
		for (auto it = adjacent_vertices(t.back(), t.g()); it.first != it.second; ++it.first) {
			Vertex v = *it.first;
			if (!t.is_feasible(v) || !t.is_canonical(v)) { continue; }

			t.extend(v);
			extend(t, cb_found, cb_cont);
			t.backtrack();
		}
	} else {
		if (!t.is_canonical()) { return; }
		/* std::cout << "Solution!!\n"; */
		cb_found(t.walk());
	}
}

template <typename CallbackContinue, typename CallbackFound, typename Trace, typename CallbackMiss>
void extend_edge(Trace& t,  CallbackFound& cb_found, CallbackContinue cb_cont, CallbackMiss cb_miss) {
	if (!cb_cont(t.walk())) { return; }
	if (t.is_partial()) {
		for (auto it = out_edges(t.back(), t.g()); it.first != it.second; ++it.first) {
			Edge e = *it.first;
			if (!t.is_feasible(e) || !t.is_canonical(e)) { continue; }

			t.extend(e);
			extend_edge(t, cb_found, cb_cont, cb_miss);
			t.backtrack();
		}
	} else {
		if (!t.is_feasible() || !t.is_canonical()) { 
			cb_miss();
			return; 
		}
		/* std::cout << "Solution!!\n"; */
		cb_found(t.walk());
	}
}

template <typename CallbackContinue, typename CallbackFound, typename Trace>
void extend_iter(Trace& t,  CallbackFound& cb_found, CallbackContinue cb_cont) {
	/* if (!t.is_canonical()) { return; } */
	/* std::cout << t << std::endl;; */
	/* std::vector<size_t> next(num_vertices(t.g()), 0); next[0] = 1; */
	std::vector<size_t> next(num_edges(t.g())*2+1,0);
	/* next.reserve(num_edges(t.g())*2+1); */
	/* next.push_back(1);next.push_back(0); */
	while (t.walk().size() > 2 || degree(t.back(),t.g()) > next[1]) {
		if (!cb_cont(t.walk())) { return; }
		else if (!t.is_partial()) {
			if (t.is_canonical()) { cb_found(t.walk()); }
			t.backtrack();
		} else if (degree(t.back(), t.g()) == next[t.walk().size()-1]) {
			t.backtrack();
		} else {
			Vertex v = *(adjacent_vertices(t.back(), t.g()).first + next[t.walk().size()-1]);
			++next[t.walk().size()-1];
			if (t.is_feasible(v) && t.is_canonical(v)) { 
				next[t.walk().size()] = 0;
				t.extend(v);
			}
		} 
	}
}
	
} /* namespace kface */
#endif /* ifndef KFACE_ENUMERATE_HPP */
