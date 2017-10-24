#include <dtrace/benchmark/bench.h>
#include <iostream>
/* #include <boost/chrono.hpp> */
#include <dtrace/traversor.h>
#include <dtrace/graph/base.h>
#include <dtrace/bruteforce/face_trace.h>

namespace dtrace {
namespace benchmark {

void bench_graph(const Graph& g) {
	/* auto start_brute = boost::chrono::system_clock::now(); */
	/* auto brute_res = bruteforce::find_two_face_embeddings(g); */
	/* boost::chrono::duration<double> dur_brute = boost::chrono::system_clock::now() - start_brute; */
	std::cout << "Num edges: " << num_edges(g) << std::endl;
	Traversor t(g);
	auto trav_res = t.find_two_face_embeddings();
	/* std::cout << "\t brute: "; */
	/* std::cout << "dur: " << dur_brute.count() << ", "; */
	/* std::cout << "count: " << brute_res.size() << std::endl; */

	const Stats& stats = t.get_stats();
	auto total = stats.dur_traversal + stats.dur_face_construction;
	std::cout << "\t trave: ";
	std::cout << "tr_dur: " << stats.dur_traversal.count() << ", ";
	std::cout << "fc_dur: " << stats.dur_face_construction.count() << ", ";
	std::cout << "dur: " << total.count() << ", ";
	std::cout << "tr_count: " << stats.num_traces << ", ";
	std::cout << "cemb_count: " << stats.num_cand_embeddings << ", ";
	std::cout << "emb_count: " << stats.num_embeddings << std::endl;
	
}

void bench_graphs(std::vector<Graph>& gs) {
	size_t i = 1;
	for (Graph& g : gs) {
		for (auto ei = edges(g); ei.first != ei.second; ++ei.first) {
			g[*ei.first].req_dir = dtrace::EdgeDirection::ANTI;
		}
		std::cout << "graph " << i << std::endl;
		bench_graph(g);
		++i;
	}
}
	
} /* namespace benchmark */
} /* namespace dtrace */
