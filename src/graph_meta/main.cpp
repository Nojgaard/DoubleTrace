#include <tclap/CmdLine.h>
#include <dtrace/graph.h>
#include <dtrace/io.h>
#include <vector>
#include <fstream>
#include <cassert>
#include <boost/multiprecision/cpp_int.hpp>

using namespace boost::multiprecision;

template <typename T>
void print_col(T c, int w = 10) {
	std::cout << std::setw(w) << std::left << c;
}

int256_t factorial(int128_t i) {
	int256_t res = 1;
	while (i > 1) { res = res * i; --i; }
	return res;
}

using namespace dtrace;

int256_t count_all_embeddings(const Graph& g) {
	int256_t count = 1;
	for (auto vi = vertices(g); vi.first != vi.second; ++vi.first) {
		auto v = *vi.first;
		count *= factorial(degree(v, g) - 1);
	}
	return count;
}

bool has_degree(const Graph& g, unsigned long d = 1) {
	for (auto vi = vertices(g); vi.first != vi.second; ++vi.first) {
		auto v = *vi.first;
		if (degree(v, g) == d) { return true; }
	}
	return false;
}

int main(int argc, const char *argv[]) {
	std::string input_path, output_path, group_path;
	try {
		using namespace TCLAP;
		CmdLine cmd("Command description", ' ', "0.9");
		ValueArg<std::string> input_arg("i", "input", "Input path", true, "", "string", cmd);
		ValueArg<std::string> group_arg("g", "group-path", "Group Path", false, "", "string", cmd);
		/* ValueArg<std::string> output_arg("o", "output", "Output path", true, "", "string", cmd); */
		cmd.parse(argc, argv);

		input_path = input_arg.getValue();
		group_path = group_arg.getValue();
		/* output_path = output_arg.getValue(); */
	} catch(TCLAP::ArgException& e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}

	using namespace dtrace;

	std::vector<Graph> gs;
	read_graphs(input_path, gs);

	std::vector<std::pair<int, int>> group_ids;
	if (group_path != "") {
		std::ifstream gfile(group_path);
		if (!gfile.is_open()) {
			std::cerr << "group-path does not exist\n";
			return 0;
		}
		while (!gfile.eof()) {
			int orbit_size, orbit_id; gfile >> orbit_size; gfile >> orbit_id;
			group_ids.push_back(std::make_pair(orbit_size, orbit_id));
		}
		group_ids.pop_back();
		/* std::cout << group_ids.size() << " " << gs.size(); */
		assert(group_ids.size() == gs.size());
	}

	size_t graph_len = gs[0][boost::graph_bundle].name.size() + 5;

	print_col("graph", graph_len);
	print_col("verts");
	print_col("edges");
	if (!group_path.empty()) {
		print_col("orbit-size", 15);
		print_col("orbit-id", 15);
	}
	print_col("degree-one", 15);
	print_col("all-embeddings", 15);
	std::cout << std::endl;

	size_t i = 0;
	for (auto& g : gs) {
		print_col(g[boost::graph_bundle].name, graph_len);
		print_col(num_vertices(g));
		print_col(num_edges(g));

		if (!group_path.empty()) {
			auto& gp = group_ids[i++];
			print_col(gp.first, 15);
			print_col(gp.second, 15);
		}
		print_col(has_degree(g,1), 15);
		print_col(count_all_embeddings(g), 15);
		
		std::cout << std::endl;
	}

	return 0;
}
