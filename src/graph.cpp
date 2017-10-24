#include <dtrace/graph.h>
#include <fstream>
#include <exception>
#include <iostream>

namespace dtrace {

std::ifstream & expect(std::ifstream &in, std::string word) {
	std::string tok;
	if (!(in >> tok) || tok != word) {
		throw std::runtime_error("Expected word: " + word);
	}
	return in;
}

void create_edge_indexes(Graph &g) {
	size_t idx = 0;
	for (auto ei = edges(g); ei.first != ei.second; ++ei.first) {
		g[*ei.first].idx = idx;
		++idx;
	}
}

Graph read_graph_dimacs(std::string path) {
	std::ifstream in(path);
	if(!in.is_open()) {throw std::runtime_error("Could not open specified file.");}

	std::string tok;
	int vertices, edges;
	expect(in, "p") >> vertices >> edges;
	if (vertices <= 0 || edges <= 0) {
		throw std::runtime_error("Vertices or edges cannot be <= 0.");
	}

	Graph g;
	for (int i = 0; i < vertices; i++) {add_vertex(g);}
	while ((in >> tok)) {
		if (tok == "e") {
			Vertex src, tar;
			in >> src >> tar;
			auto r = add_edge(src, tar, g);
			if (!r.second) {
				throw std::runtime_error("Could not add edge while parsing.");
			}
		} else {
			throw std::runtime_error("Found unexpected symbol: " + tok);
		}
	}

	in.close();
	if ((int)num_vertices(g) != vertices || (int)num_edges(g) != edges) {
		/* std::cout << "Created: " << num_vertices(g) << " " << num_edges(g); */
		/* std::cout << "\nFound: " << vertices << " " << edges << std::endl; */
		throw std::runtime_error("Mismatch on vertex/edge count.");
	}
	create_edge_indexes(g);
	return g;
}

} /* namespace dtrace */
