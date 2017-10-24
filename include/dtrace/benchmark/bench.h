#ifndef DTRACE_BENCHMARK_BENCH_H
#define DTRACE_BENCHMARK_BENCH_H
#include <dtrace/graph/base.h>
#include <vector>

namespace dtrace {
namespace benchmark {

void bench_graph(const Graph& g);
void bench_graphs(std::vector<Graph>& graphs);
	
} /* namespace benchmark */
	
} /* namespace dtrace */
#endif /* ifndef DTRACE_BENCHMARK_BENCH_H */
