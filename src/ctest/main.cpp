#include <iostream>
#include <iomanip>
#include <fstream>
#include <graphio/g6.h>
#include <dtrace/bruteforce/face_trace.h>
/* #include <kface/Graph.hpp> */
/* #include <kface/Find.hpp> */

using namespace dtrace;
void print_header(std::string name, std::ostream& os) {
	os << "std::vector<std::pair<std::string, size_t>> " << name << " = { ";
	os << std::endl;
}

void print_tail(std::ostream& os) {
	os << "};" << std::endl;
}

void print_graph(const Graph& g, std::ostream& os) {
	dtrace::Stats stats;
	auto res = dtrace::bruteforce::find_n_face_embeddings(g, 1, stats);
	os << "\tstd::make_pair(\"" << g[boost::graph_bundle].name << "\"," << res.size() << ")," << std::endl;
}

int main(int argc, const char *argv[]) {
	std::string p(argv[1]);
	std::string op(argv[2]);
	
	std::ifstream file(p);
	std::ofstream ofile(op);
	std::string g6_graph;
	print_header("tg5", ofile);
	while (std::getline(file, g6_graph)) {
		Graph g;
		graphio::g6::read(g6_graph, g);
		if (num_edges(g) < 15) {
			print_graph(g, ofile);
		}
	}
	print_tail(ofile);
	return 0;
}
