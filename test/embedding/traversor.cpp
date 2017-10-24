#include <catch/catch.hpp>
#include <dtrace/bruteforce/face_trace.h>
#include <dtrace/traversor.h>
#include <dtrace/graph/base.h>
#include <dtrace/canon/emb_naive.h>
#include <dtrace/canon/emb_brute.h>
#include <dtrace/canon/trace.h>
#include <dtrace/io.h>
#include <dtrace/util.h>
#include <morphism/aut.h>

namespace dtrace {
namespace test {

 size_t	num_unique(std::vector<std::vector<dtrace::bruteforce::Face>>& embs, dtrace::Graph& g) {
	std::unordered_map<dtrace::canon::EmbNaive, std::vector<dtrace::Face> > m;
	for (auto& emb : embs) {
		std::vector<dtrace::Face> fs;
		for (auto& f : emb) {
			fs.push_back(dtrace::Face());
			for (auto pv : f) {
				fs.back().push_back(pv.first);
			}
		}
		dtrace::canon::EmbNaive tc(fs, g);
		if (m.find(tc) == m.end()) {
			m[tc] = fs;
		}
	}
	return m.size();
}

void print_graph(const dtrace::Graph& g) {
	for (auto vi = vertices(g); vi.first != vi.second; ++vi.first) {
		dtrace::Vertex v = *vi.first;
		std::cout << v << ": ";
		for (auto ai = adjacent_vertices(v, g); ai.first != ai.second; ++ai.first) {
			std::cout << *ai.first << " ";
		}
		std::cout << std::endl;
	}
}

void compare_results(Graph& g, std::unordered_set<canon::EmbBrute>& exp,
		std::unordered_map<canon::Trace, std::vector<Vertex>>& res) {
	print_graph(g);
	std::vector<morphism::Perm> auts = morphism::find_automorphisms(g);
	auts = morphism::get_all_permutations(auts);
	std::unordered_map<canon::EmbBrute, std::vector<Vertex>> out;
	std::cout << "auts:\n";
	for (auto& p : auts) {
		std::cout << p << std::endl;
	}
	for (auto &kv : res) {
		std::vector<std::vector<Vertex>> faces;
		faces.push_back(kv.second);
		canon::EmbBrute canon(faces, auts);
		if (out.find(canon) != out.end()) {
			std::cout << "DUPLICATE!\n";
			std::cout << kv.second << std::endl;
			std::cout << out[canon] << std::endl;

		} else {
			out[canon] = kv.second;
		}
		
	}
}

void compare_results(Graph& g, std::unordered_set<canon::EmbBrute>& exp,
		std::unordered_map<canon::Embedding, std::vector<std::vector<Vertex>>>& res) {
	print_graph(g);
	std::vector<morphism::Perm> auts = morphism::find_automorphisms(g);
	auts = morphism::get_all_permutations(auts);
	std::unordered_map<canon::EmbBrute, std::vector<std::vector<Vertex>>> out;
	std::cout << "auts:\n";
	for (auto& p : auts) {
		std::cout << p << std::endl;
	}
	for (auto &kv : res) {
		canon::EmbBrute canon(kv.second, auts);
		if (out.find(canon) != out.end()) {
			std::cout << "DUPLICATE!\n";
			for (auto& f : kv.second) {
				std::cout << f << "| ";
			}
			std::cout << std::endl;
			for (auto& f : out[canon]) {
				std::cout << f << "| ";
			}
			std::cout << std::endl;

		} else {
			out[canon] = kv.second;
		}
		
	}
}

void test_graph(Graph& g, int nfaces) {
	size_t i = 0;
	for (auto ei = edges(g); ei.first != ei.second; ++ei.first) {
		g[*ei.first].idx = i++;
	}
	for (auto ei = edges(g); ei.first != ei.second; ++ei.first) {
		g[*ei.first].req_dir = dtrace::EdgeDirection::ANTI;
	}
	Stats stats;
	auto exp_res = bruteforce::find_n_face_embeddings(g, nfaces, stats);
	Traversor t(g);
	if (nfaces == 2) {
		auto found_res = t.find_two_face_embeddings();
		if (exp_res.size() != found_res.size()) {
			compare_results(g, exp_res, found_res);
		}
		REQUIRE(exp_res.size() == found_res.size());
	} else {
		auto found_res = t.find_strong_traces();
		if (exp_res.size() != found_res.size()) {
			compare_results(g, exp_res, found_res);
		}
		REQUIRE(exp_res.size() == found_res.size());
	}
	/* size_t exp_num = exp_res.size() / 2; */
	/* if (has_auts) { */
	/* 	exp_num = num_unique(exp_res, g); */
	/* } */
	/* size_t exp_num = exp_res.size(); */
	
}

void test_graphs(std::vector<Graph>& gs, int nfaces) {
	for (Graph& g : gs) {
		test_graph(g, nfaces);
	}
}

TEST_CASE("Non Aut Graphs, Size: 6", "[naut_6]") {
	std::string path = "../data/simple/aut_graph6c.g6";
	std::vector<Graph> gs;
	dtrace::read_graphs(path, gs);
	test_graphs(gs, 2);
}

TEST_CASE("Aut Graphs, Size: 7, Faces: 2", "[2f_aut_7][!hide]") {
	std::string path = "../data/simple/graph7c.g6";
	std::vector<Graph> gs;
	dtrace::read_graphs(path, gs);
	test_graphs(gs, 2);
}

TEST_CASE("Non Aut Graphs, Size: 7, Faces: 2", "[2f_naut_7][!hide]") {
	std::string path = "../data/simple/aut_graph7c.g6";
	std::vector<Graph> gs;
	dtrace::read_graphs(path, gs);
	test_graphs(gs, 2);
}

TEST_CASE("Aut Graphs, Size: 7, Faces: 1", "[1f_aut_7][!hide]") {
	std::string path = "../data/simple/graph7c.g6";
	std::vector<Graph> gs;
	dtrace::read_graphs(path, gs);
	test_graphs(gs, 1);
}

TEST_CASE("Aut Graphs, Size: 5", "[aut_5]") {
	std::string path = "../data/simple/graph5c.g6";
	std::vector<Graph> gs;
	dtrace::read_graphs(path, gs);
	test_graphs(gs, 2);
}

TEST_CASE("Aut Graphs, Size: 5, nface: 1", "[f1_aut_5]") {
	std::string path = "../data/simple/graph5c.g6";
	std::vector<Graph> gs;
	dtrace::read_graphs(path, gs);
	test_graphs(gs, 1);
}

TEST_CASE("Non Aut Graphs, Size: 6, nface: 1", "[f1_naut_6]") {
	std::string path = "../data/simple/aut_graph6c.g6";
	std::vector<Graph> gs;
	dtrace::read_graphs(path, gs);
	test_graphs(gs, 1);
}
} /* namespace test */
} /* namespace dtrace */

