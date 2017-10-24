/* #include <dtrace/graph/base.h> */
#include <dtrace/bruteforce/face_trace.h>
#include <dtrace/bruteforce/embedding.h>
#include <dtrace/canon/emb_brute.h>
#include <dtrace/canon/trace.h>
#include <dtrace/stat_printer.h>
#include <dtrace/util.h>
#include <set>
#include <iostream>
/* #include <boost/chrono.hpp> */
#include <chrono>

namespace dtrace {
namespace bruteforce {

std::vector<std::vector<std::pair<Vertex, Vertex>>> trace_faces(const Embedding &e, const Graph &g, size_t nfaces) {
	std::set<std::pair<Vertex, Vertex>> edge_set;
	std::vector<std::pair<Vertex, Vertex>> path;
	std::vector<std::vector<std::pair<Vertex, Vertex>>> faces;
	for (auto pe = edges(g); pe.first != pe.second; ++pe.first) {
		Vertex src = source(*pe.first, g), tar = target(*pe.first, g);
		edge_set.insert(std::make_pair(src, tar));
		edge_set.insert(std::make_pair(tar, src));
	}
	for (auto edge : edge_set) {
		path.push_back(edge);
		edge_set.erase(edge);
		break;
	}

	while (!edge_set.empty()) {
		const std::vector<Vertex> &adj = e.v_embedding(path.back().second);
		assert(adj.size() > 0);
		size_t from_idx = std::find(adj.begin(), adj.end(), path.back().first)-adj.begin();
		Vertex next_vertex = adj.at((from_idx + 1)%adj.size());
		std::pair<Vertex, Vertex> tup(path.back().second, next_vertex);
		if (tup == path.at(0)) {
			faces.push_back(path);
			path.clear();
			for (auto edge : edge_set) {
				path.push_back(edge);
				edge_set.erase(edge);
				break;
			}
			if (nfaces == faces.size() && edge_set.size() > 0) {
				faces.clear();
				break;
			}
		} else {
			path.push_back(tup);
			edge_set.erase(tup);
		}
	}
	if (path.size() > 0) { faces.push_back(path); }
	if (nfaces == faces.size() && edge_set.size() > 0) { faces.clear(); }
	return faces;
}

void generate_vertex_embedding(Vertex v, Embedding &e, const Graph &g
		, std::vector<Embedding> &out, Stats& stats, StatPrinter& sp, size_t nfaces) {
	sp.update();
	if (e.is_partial()) {
		auto adj_iter = adjacent_vertices(v, g);
		std::vector<Vertex> adj_vec(adj_iter.first, adj_iter.second);
		Vertex v_fixed = adj_vec.back();
		adj_vec.pop_back();
		std::sort(adj_vec.begin(), adj_vec.end());
		do {
			adj_vec.push_back(v_fixed);
			e.embed_vertex(v, adj_vec);
			generate_vertex_embedding(v + 1, e, g, out, stats, sp, nfaces);
			e.unembed_vertex(v);
			adj_vec.pop_back();
		} while (std::next_permutation(adj_vec.begin(), adj_vec.end()));
	} else {
		++stats.num_cand_embeddings;
		if (trace_faces(e, g, nfaces).size() == nfaces) {
			++stats.num_traces;
			out.push_back(e);
		}
	}
}

std::vector<Embedding> generate_embeddings(const Graph &g, Stats& stats, StatPrinter& sp, int nfaces) {
	std::vector<Embedding> out;
	Embedding e(g);
	generate_vertex_embedding(0, e, g, out, stats, sp, nfaces);
	return out;
}

void print_embeddings(std::vector<Embedding> &embs) {
	for (auto &emb : embs) {
		std::cout << emb << std::endl;
	}
}

/* std::vector<std::vector<Face>> find_two_face_embeddings(const Graph &g) { */
/* 	std::vector<Embedding> embeddings = generate_embeddings(g); */
/* 	/1* print_embeddings(embeddings); *1/ */
/* 	std::vector<std::vector<std::vector<std::pair<Vertex, Vertex>>>> two_face_embeds; */
/* 	for (const Embedding &e : embeddings) { */
/* 		auto faces = trace_faces(e, g); */
/* 		if (faces.size() == 2) { */
/* 			two_face_embeds.push_back(faces); */
/* 		} */
/* 	} */
/* 	return two_face_embeds; */
/* } */

/* std::vector<std::vector<Face>> find_n_face_embeddings(const Graph &g, size_t num_faces) { */
/* 	std::vector<Embedding> embeddings = generate_embeddings(g); */
/* 	/1* print_embeddings(embeddings); *1/ */
/* 	std::vector<std::vector<std::vector<std::pair<Vertex, Vertex>>>> two_face_embeds; */
/* 	for (const Embedding &e : embeddings) { */
/* 		auto faces = trace_faces(e, g); */
/* 		if (faces.size() == num_faces) { */
/* 			two_face_embeds.push_back(faces); */
/* 		} */
/* 	} */
/* 	return two_face_embeds; */
/* } */

std::vector<Vertex> to_trace(const std::vector<std::pair<Vertex, Vertex>>& pt) {
	std::vector<Vertex> out;
	for (auto& p : pt) {
		out.push_back(p.first);
	}
	return out;
}

std::unordered_set<canon::Trace> find_one_face_embeddings(const Graph &g, Stats& stats) {
	StatPrinter sp(stats, 0.5, 2, 60, stats.enable_print);
	size_t num_faces = 1;
	stats.num_vertices = num_vertices(g);
	stats.num_edges = num_edges(g);
	std::unordered_set<canon::Trace> out;
	auto start = std::chrono::system_clock::now();
	if (!valid_euler(num_faces, num_vertices(g), num_edges(g))) {
		stats.time = std::chrono::system_clock::now() - start;
		sp.print_stats();
		return out;
	}
	std::vector<Embedding> embeddings = generate_embeddings(g, stats, sp, num_faces);
	/* print_embeddings(embeddings); */
	std::vector<std::vector<std::vector<std::pair<Vertex, Vertex>>>> two_face_embeds;
	for (const Embedding &e : embeddings) {
		sp.update();
		auto faces = trace_faces(e, g, num_faces);
		if (faces.size() == num_faces) {
			two_face_embeds.push_back(faces);
		}
	}
	auto auts = morphism::find_automorphisms(g);
	stats.gen_auts = auts.size();
	auts = morphism::get_all_permutations(auts);
	stats.auts = auts.size();
	for (auto& emb : two_face_embeds) {
		sp.update();
		std::vector<std::vector<Vertex>> ff;
		for (auto& f : emb) {
			ff.push_back(to_trace(f));
		}
		canon::Trace canon(num_vertices(g), ff[0]);
		if (out.find(canon) == out.end()) {
			out.insert(canon);
			++stats.num_embeddings;
		}
	}
	stats.time = std::chrono::system_clock::now() - start;
	sp.print_stats();
	return out;
}

std::unordered_set<canon::EmbBrute> find_n_face_embeddings(const Graph &g, size_t num_faces, Stats& stats) {
	StatPrinter sp(stats, 0.5, 2, 60, stats.enable_print);
	stats.num_vertices = num_vertices(g);
	stats.num_edges = num_edges(g);
	std::unordered_set<canon::EmbBrute> out;
	auto start = std::chrono::system_clock::now();
	if (!valid_euler(num_faces, num_vertices(g), num_edges(g))) {
		stats.time = std::chrono::system_clock::now() - start;
		sp.print_stats();
		return out;
	}
	std::vector<Embedding> embeddings = generate_embeddings(g, stats, sp, num_faces);
	/* print_embeddings(embeddings); */
	std::vector<std::vector<std::vector<std::pair<Vertex, Vertex>>>> two_face_embeds;
	for (const Embedding &e : embeddings) {
		sp.update();
		auto faces = trace_faces(e, g, num_faces);
		if (faces.size() == num_faces) {
			two_face_embeds.push_back(faces);
		}
	}
	auto auts = morphism::find_automorphisms(g);
	stats.gen_auts = auts.size();
	auts = morphism::get_all_permutations(auts);
	stats.auts = auts.size();
	for (auto& emb : two_face_embeds) {
		sp.update();
		std::vector<std::vector<Vertex>> ff;
		for (auto& f : emb) {
			ff.push_back(to_trace(f));
		}
		canon::EmbBrute canon(ff, auts);
		if (out.find(canon) == out.end()) {
			out.insert(canon);
			/* out.insert(canon::EmbBrute(ff, auts)); */
			++stats.num_embeddings;
		}
	}
	std::cout << out.size() << std::endl;
	stats.time = std::chrono::system_clock::now() - start;
	sp.print_stats();
	return out;
}

void print_face_embedding(const std::vector<Face> &emb) {
	for (const Face &f : emb) {
		for (const DirEdge &edge : f) {
			/* std::cout << "(" << edge.first << ", " << edge.second << ") "; */
			std::cout << edge.first << " ";
		}
		std::cout << "| ";
	}
	std::cout << std::endl;
}

} /* namespace benchmark */
} /* namespace dtrace */
