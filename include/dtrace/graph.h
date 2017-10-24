#ifndef DTRACE_GRAPH_H
#define DTRACE_GRAPH_H
#include <dtrace/graph/base.h>

namespace dtrace {

	Graph read_graph_dimacs(std::string path);
} /* namespace dtrace */
#endif /* ifndef DTRACE_GRAPH_H */
