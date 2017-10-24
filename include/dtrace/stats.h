#ifndef DTRACE_STATS_H
#define DTRACE_STATS_H

/* #include <boost/chrono.hpp> */
#include <chrono>

namespace dtrace {

class Stats {
public:
	Stats(bool enable_print = true): enable_print(enable_print) {};
	int num_traces = 0, num_embeddings = 0, num_cand_embeddings = 0, auts = 0, gen_auts = 0;
	int num_edges = 0, num_vertices = 0;
	std::chrono::duration<double> dur_traversal, dur_face_construction, time;
	bool enable_print;
};
	
} /* namespace dtrace */

#endif /* ifndef DTRACE_STATS_H */
