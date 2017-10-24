#ifndef DTRACE_IO_H
#define DTRACE_IO_H
#include <dtrace/graph/base.h>
#include <dtrace/stats.h>
#include <vector>
namespace dtrace {

void read_graph(std::string& p, Graph& g);
void read_graphs(std::string& p, std::vector<Graph>& gs);
void write_stats(std::string& p, const Stats& stats);
	
} /* namespace dtrace */
#endif /* ifndef DTRACE_IO_H */
