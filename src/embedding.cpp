#include <dtrace/embedding.h>
#include <dtrace/traversor.h>
#include <dtrace/util.h>
#include <dtrace/canon/trace.h>
#include <dtrace/canon/embedding.h>
#include <unordered_map>
#include <iostream>

namespace dtrace {

void set_anti_edges(Graph &g) {
	for (auto ei = edges(g); ei.first != ei.second; ++ei.first) {
		g[*ei.first].req_dir = EdgeDirection::ANTI;
	}
}


std::unordered_map<canon::Trace, std::vector<Vertex>> find_one_face_embeddings(Graph &g) {
	Traversor t(g);
	return t.find_strong_traces();
}


/* std::vector<std::pair<Edge,Edge>> find_corners(Vertex v, std::vector<Vertex> &trace, const Graph &g) { */
/* 	std::vector<std::pair<Edge, Edge>> corners; */
/* 	for (size_t i = 0; i < trace.size(); ++i) { */
/* 		if (v != trace[i]) { continue; } */
/* 		Vertex from = (i == 0) ? trace.back() : trace[i-1]; */
/* 		Vertex to = (i == trace.size()-1) ? trace[0] : trace[i+1]; */
/* 		Edge efrom = boost::edge(from, v, g).first, eto = boost::edge(to, v, g).first; */
/* 		corners.push_back(std::make_pair(efrom, eto)); */
/* 	} */
/* 	return corners; */
/* } */

std::vector<size_t> find_corners(Vertex v, std::vector<Vertex> &trace) {
	std::vector<size_t> corners;
	for (size_t i = 0; i < trace.size(); ++i) {
		if (v != trace[i]) { continue; }
		corners.push_back(((i==0)?trace.size()-1:i-1));
	}
	return corners;
}

void print_trace(const std::vector<Vertex> &trace) {
	for (Vertex v : trace) {
		std::cout << v << " ";
	}
	std::cout << "-";
	std::cout << std::endl;
}

std::vector<Edge> find_boundaries(const std::vector<Edge>& face, const Graph& g) {
	std::vector<int> num_traversed(num_edges(g), 0);


	/* std::cout << "WHATA\n"; */
	/* 	for (auto pe = edges(g); pe.first != pe.second; ++pe.first) { */
	/* 		Edge e = *pe.first; */
	/* 		print_edge(std::cout, e, g); */
	/* 		std::cout << ": " << g[e].idx << std::endl; */
	/* 	} */
	/* 	std::cout << "~~~~\n"; */
	for (const Edge& e : face) {
		print_edge(std::cout, e, g);
		/* std::cout << ": " << g[boost::edge(source(e, g), target(e, g), g).first].idx << std::endl; */
		/* ++num_traversed[g[e].idx]; */
		++num_traversed[g[boost::edge(source(e, g), target(e, g), g).first].idx];
	}
	std::vector<Edge> out;
	for (const Edge &e : face) {
		/* if (num_traversed[g[e].idx] == 1) { */
		if (num_traversed[g[boost::edge(source(e, g), target(e, g), g).first].idx] == 1) {
			out.push_back(e);
		}
	}
	return out;
}

void reinsert_edge(Edge e, size_t cu, size_t cw
		, std::vector<Edge> &trace, const Graph &g
		, std::unordered_map<canon::Trace , std::pair<std::vector<Vertex>,std::vector<Vertex>>> &out) {

	/* std::cout << "cu: " << cu << ", " << "cw: " << cw << std::endl; */
	std::vector<Edge> two_trace;
	size_t tmp_cu, tmp_cw;
	for (size_t i = 0; i < trace.size(); ++i) {
		two_trace.push_back(trace[i]);
		if (i == cu || i == cw) {
			if (cu == i) {tmp_cu = two_trace.size(); }
			else {tmp_cw = two_trace.size(); }
			two_trace.push_back(e);
		}
	}
	cu = tmp_cu; cw = tmp_cw;
	/* std::cout << "cu: " << cu << ", " << "cw: " << cw << std::endl; */
	/* std::cout << "Two Face: " << std::endl; */
	/* for (auto e : two_trace) { */
	/* 	print_edge(std::cout, e, g); */
	/* 	std::cout << " "; */
	/* } */
	/* std::cout << std::endl; */
	std::vector<Edge> uface, wface;
	for (size_t i = 0; i < two_trace.size(); ++i) {
		size_t idx = (i+cu+1)%two_trace.size();
		uface.push_back(two_trace[idx]);
		/* print_edge(std::cout, uface.back(), g); */
		/* std::cout << " "; */
		if (two_trace[idx] == e) {
			break;
		}
	}
	/* std::cout << " | "; */
	for (size_t i = 0; i < two_trace.size(); ++i) {
		size_t idx = (i+cw+1)%two_trace.size();
		wface.push_back(two_trace[idx]);
		/* print_edge(std::cout, wface.back(), g); */
		/* std::cout << " "; */
		if (two_trace[idx] == e) {
			break;
		}
	}
	/* std::cout << std::endl; */


	auto bedges = find_boundaries(uface, g);
	std::cout << "Boundaries: " << bedges.size() << std::endl;
	bool found = false;
	for (Edge& be : bedges) {
		std::rotate(uface.begin(), std::find(uface.begin(), uface.end(), be), uface.end());
		std::rotate(wface.begin(), std::find(wface.begin(), wface.end(), be), wface.end());
		std::vector<Vertex> vaface = edge_to_vertex(g, uface);
		std::vector<Vertex> weface = edge_to_vertex(g, wface);
		vaface.insert(vaface.end(), weface.begin(), weface.end());
		canon::Trace canon(num_vertices(g), vaface);
		/* print_trace(vaface); */
		std::cout << "(" << source(be, g) << ", " << target(be, g) << ")" << std::endl;
		if (out.find(canon) != out.end()) {
			std::cout << "Exists\n";
			found = true;

			std::cout << "**\n";
			print_trace(vaface);
			auto res =  out.find(canon);
			std::cout << res->second.first << res->second.second << std::endl;
			std::cout << "**\n";
			break;
		}

		/* std::cout << "------\n"; */
		/* print_trace(trace::edge_to_vertex(g, uface)); */
		/* std::cout << "| "; */
		/* print_trace(trace::edge_to_vertex(g, wface)); */
		/* std::cout << std::endl; */
		auto ruface = uface, rwface = wface;
		std::reverse(ruface.begin(), ruface.end());
		std::reverse(rwface.begin(), rwface.end());
		std::rotate(ruface.begin(), std::find(ruface.begin(), ruface.end(), be), ruface.end());
		std::rotate(rwface.begin(), std::find(rwface.begin(), rwface.end(), be), rwface.end());
		vaface = edge_to_vertex(g, ruface);
		weface = edge_to_vertex(g, rwface);
		vaface.insert(vaface.end(), weface.begin(), weface.end());
		/* print_trace(trace::edge_to_vertex(g, ruface)); */
		/* std::cout << "| "; */
		/* print_trace(trace::edge_to_vertex(g, rwface)); */
		/* print_trace(vaface); */
		canon::Trace rcanon(num_vertices(g), vaface);
		if (out.find(rcanon) != out.end()) {
			std::cout << "Exists\n";
			found = true;

			std::cout << "**\n";
			print_trace(vaface);
			auto res =  out.find(rcanon);
			std::cout << res->second.first << res->second.second << std::endl;
			std::cout << "**\n";
			break;
		} else {
		}
	}
		print_trace(edge_to_vertex(g, uface));
		std::cout << "| ";
		print_trace(edge_to_vertex(g, wface));
	if (!found) {
			std::cout << "Doesn't exist\n";
			for (auto &kv : out) {
				std::cout << kv.second.first << "| ";
				std::cout << kv.second.second << std::endl;
			}
		/* std::cout << "Found new one\n"; */
		std::vector<Vertex> vaface = edge_to_vertex(g, uface);
		std::vector<Vertex> ueface = edge_to_vertex(g, uface);
		std::vector<Vertex> weface = edge_to_vertex(g, wface);
		vaface.insert(vaface.end(), weface.begin(), weface.end());
		canon::Trace canon(num_vertices(g), vaface);
		std::pair<std::vector<Vertex>, std::vector<Vertex>> p(ueface, weface);
		out[canon] = p;
	}

	/* std::vector<Vertex> ueface = trace::edge_to_vertex(g, uface); */
	/* auto vtwo_trace = trace::edge_to_vertex(g, two_trace); */
	/* auto weface = trace::edge_to_vertex(g, wface); */

	/* std::vector<Vertex> vaface = ueface; */
	/* std::rotate(weface.begin(), weface.end()-1, weface.end()); */
	/* vaface.insert(vaface.end(), weface.begin(), weface.end()); */
	/* canon::trace canon(num_vertices(g), vaface); */
	/* if (out.find(canon) == out.end()) { */
	/* 	std::pair<std::vector<Vertex>, std::vector<Vertex>> p(ueface, weface); */
	/* 	/1* out[ce] = p; *1/ */
	/* 	out[canon] = p; */
	/* } */
	/* bool found = false; */
	/* for (size_t i = 0; i < weface.size(); ++i) { */
	/* 	std::vector<Vertex> vaface = ueface; */
	/* 	std::rotate(weface.begin(), weface.begin()+1, weface.end()); */
	/* 	vaface.insert(vaface.end(), weface.begin(), weface.end()); */
	/* 	/1* print_trace(ueface); *1/ */
	/* 	/1* print_trace(weface); *1/ */
	/* 	/1* CanonEmbedding ce(ueface, weface, g); *1/ */
	/* 	canon = canon::trace(num_vertices(g), vaface); */

	/* 	if (out.find(canon) == out.end()) { */
	/* 		/1* std::pair<std::vector<Vertex>, std::vector<Vertex>> p(ueface, weface); *1/ */
	/* 		/1* out[ce] = p; *1/ */
	/* 		/1* out[canon] = p; *1/ */
	/* 	} else { */
	/* 		found = true; */
	/* 		break; */
	/* 	} */
	/* } */
	/* if (!found) { */
	/* 	std::pair<std::vector<Vertex>, std::vector<Vertex>> p(ueface, weface); */
	/* 	out[canon] = p; */
	/* } else { */
	/* 	std::cout << "WHAHHAHAHA\n"; */
	/* } */
}

void reinsert_edge(Edge e, std::vector<Vertex> &trace,  const Graph &g, 
	std::unordered_map<canon::Trace , std::pair<std::vector<Vertex>,std::vector<Vertex>>> &out ) {
	/* std::cout << "Vertex trace: " << std::endl; */
	for (auto v : trace) {
		/* std::cout << v << " "; */
	}
	/* std::cout << std::endl; */
	/* std::cout << "Inserting: "; */
	/* print_edge(std::cout, e, g); */
	/* std::cout << std::endl; */
	Vertex u = source(e, g), w = target(e, g);
	auto cu = find_corners(u, trace);
	auto cw = find_corners(w, trace);
	std::vector<Edge> etrace = vertex_to_edge(g, trace);
	/* std::cout << "Edge trace: " << std::endl; */
	/* for (auto e : etrace) { */
	/* 	print_edge(std::cout, e, g); */
	/* 	std::cout << " "; */
	/* } */
	/* std::cout << std::endl; */
	for (size_t ucorner : cu) {
		for (size_t wcorner : cw) {
			reinsert_edge(e, ucorner, wcorner, etrace, g, out);
		}
	}

}

std::unordered_map<canon::Trace, std::pair<std::vector<Vertex>,std::vector<Vertex>>> find_two_face_embeddings(const Graph &org_g) {
	std::unordered_map<canon::Trace, std::pair<std::vector<Vertex>,std::vector<Vertex>>> out;
	Graph g = org_g;
	size_t i = 0;

	for (auto ei = edges(org_g); ei.first != ei.second; ++ei.first) {
		auto be = g[*ei.first];
			std::cout << "REM Graph Test ( " << be.idx << ")\n";
			print_edge(std::cout, *ei.first, g);
		for (auto pe = edges(g); pe.first != pe.second; ++pe.first) {
			print_edge(std::cout, *pe.first, g);
			std::cout << ": " << g[*pe.first].idx << std::endl;
		}
		size_t idx = g[*ei.first].idx;
		auto src = source(*ei.first, g), tar = target(*ei.first, g);
		remove_edge(src, tar, g);
		/* Edge e = boost::edge(1, 4, g).first; */
		/* remove_edge(e, g); */
		auto embeddings = find_one_face_embeddings(g);

		auto pe = add_edge(src, tar, be, g);

		i = 0;
		for (auto ei1 = edges(g); ei1.first != ei1.second; ++ei1.first) {
			g[*ei1.first].idx = i;
			++i;
		}
		assert(pe.second);
		/* g[pe.first].req_dir = EdgeDirection::ANTI; */
		/* g[pe.first].idx = idx; */
			std::cout << "Graph Test\n";
		for (auto pe = edges(g); pe.first != pe.second; ++pe.first) {
			Edge e = *pe.first;
			print_edge(std::cout, e, g);
			std::cout << ": " << g[e].idx << std::endl;
		}
		std::cout << "~~~~\n";
		/* std::cout << idx << std::endl; */

		/* std::cout << "Edges: " << num_edges(g) << std::endl; */
		/* std::cout << "Embeddings: " << embeddings.size() << std::endl; */
		if (embeddings.size() > 0) {
			/* std::cout << "Graph has a two gace embedding!\n"; */
			for (auto &emb : embeddings) {
				reinsert_edge(*ei.first, emb.second, g, out);
				/* reinsert_edge(e, emb.second, g, out); */
			}
			/* ++i; if (out.size() >= 15) { break; } */
			/* break; */
			/* return; */
		}

	}
	std::cout << "Unique Faces (" << out.size() << "): \n";
	for (auto &kv : out) {
		std::cout << kv.second.first << "| ";
		std::cout << kv.second.second << std::endl;
	}
	std::vector<std::vector<Vertex>> embs;
	for (auto &fs : out) {
		embs.push_back(fs.second.first);
		/* embs.push_back(fs.second.second); */
	}
	/* return embs; */
	return out;
}

void find_face_embeddings(Graph &g) {
	set_anti_edges(g);
	auto embeddings = find_one_face_embeddings(g);
	if (embeddings.size() > 0) {
		std::cout << "Graph has a one face embedding!\n";
		return;
	}
	find_two_face_embeddings(g);
}

} /* namespace dtrace */

