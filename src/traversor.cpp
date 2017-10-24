#include <dtrace/traversor.h>
#include <boost/graph/graph_utility.hpp>
#include <dtrace/util.h>
#include <queue>
/* #include <boost/chrono.hpp> */
#include <chrono>

namespace dtrace {

Traversor::Traversor(const Graph &g, bool print_progress):
	  graph(g), num_walked(num_edges(g), 0), dir_walked(num_edges(g))
	, last_seen(num_vertices(g)), gaps_f(full_trace_size())
	, gaps_b(full_trace_size()) 
	, stats(Stats(print_progress))
	, statPrinter(stats, 0.5, 2, 60, print_progress) {
		for (auto vi = vertices(g); vi.first != vi.second; ++vi.first) {
			local_traces.push_back(LocalTrace(*vi.first, graph));
		}
		std::vector<Perm> raw_auts = morphism::find_automorphisms(g);
		std::vector<Perm> auts = morphism::get_all_permutations(raw_auts);
		stats.auts = auts.size();
		stats.gen_auts = raw_auts.size();
		stats.num_vertices = num_vertices(g);
		stats.num_edges = num_edges(g);
}

bool Traversor::is_partial() const {
	return trace.size() < full_trace_size();
}

bool Traversor::is_walked(Edge e) const {
	return num_walked[graph[e].idx] > 0;
}

bool Traversor::is_full(Edge e) const {
	return num_walked[graph[e].idx] == 2;
}

size_t Traversor::full_trace_size() const {
	return num_edges(graph) * 2;
}

void Traversor::walk_vertex_gap(Vertex v) {
	if (last_seen[v].size() > 0) {
		Vertex last_seen_v = last_seen[v].back();
		Vertex first_seen_v = last_seen[v].front();
		
		gaps_f[last_seen_v] = trace.size() - last_seen_v;
		gaps_f[trace.size()] = full_trace_size() - trace.size() + first_seen_v;

		size_t start_b = full_trace_size() - 1;
		gaps_b[start_b - trace.size()] = trace.size() - last_seen_v;
		gaps_b[start_b - first_seen_v] = full_trace_size() - trace.size() + first_seen_v;

		assert(gaps_f[last_seen[v].back()] > 0);
		assert(gaps_f[trace.size()] > 0);
	} else {
		size_t start_b = full_trace_size() - 1;
		gaps_b[start_b - trace.size()] = full_trace_size();
		gaps_f[trace.size()] = full_trace_size();
	}
	last_seen[v].push_back(trace.size());
}

bool Traversor::valid_gap() {
	if (trace[0] != trace.back() || last_seen[trace[0]].size() == 2) { return true; }

	size_t v = last_seen[trace[0]][last_seen[trace[0]].size() - 2];
	size_t start_b = full_trace_size() - 1;
	if (gaps_f[v] < gaps_f[0] && gaps_f[v] < gaps_b.back()) { 
		/* std::cout << "WOWA\n"; */
		return false; 
	}
	return true;

}

void Traversor::backtrack_vertex_gap(Vertex v) {
	assert(last_seen[v].size() > 0);
	last_seen[v].pop_back();
	if (last_seen[v].size() > 0) {
		Vertex last_seen_v = last_seen[v].back();
		Vertex first_seen_v = last_seen[v].front();

		gaps_f[last_seen_v] = full_trace_size() - last_seen_v + first_seen_v;

		size_t start_b = full_trace_size() - 1;
		gaps_b[start_b - first_seen_v] = full_trace_size() - last_seen_v + first_seen_v;
	}
}

bool Traversor::check_direction(Vertex v, Edge e) const{
	if (graph[e].req_dir == EdgeDirection::ANY || !is_walked(e)) { return true; }
	Vertex u = trace.back();
	return (v == dir_walked[graph[e].idx].first && u == dir_walked[graph[e].idx].second);
}


bool Traversor::walk_edge(Edge e, bool allow_exc) {
	Vertex v = opposite(e, trace.back(), graph);
	if (is_full(e) || !check_direction(v, e)) { return false; }

	// QUESTIONABLE (RECONSIDER)...
	/* Vertex first_v = (trace[0] == trace[2]?1:0); */
	/* Vertex first_v = 0; */
	/* if (trace.size() > 2 && v == trace[first_v] && trace.size() < 10) { */

	/* 	std::vector<std::vector<Vertex>> faces; */
	/* 	faces.push_back(std::vector<Vertex>(trace.begin() + first_v, trace.end())); */
	/* 	faces[0].push_back(trace[first_v]); */
	/* 	canon::EmbBrute canon(faces,this->auts); */
	/* 	if (seen_cycle.find(canon) != seen_cycle.end()) { */
	/* 		return false; */
	/* 	} else { */
	/* 		seen_cycle.insert(canon); */
	/* 	} */
	/* } */

	for (size_t i = 0; i < trace.size(); ++i) {
		size_t j = trace.size()- 1 - i;
		if (trace[i] < trace[j]) { break; }
		else if (trace[i] > trace[j]) { return false; }
	}

	auto &local_trace = local_traces[trace.back()];
	/* bool allow_exc = rep_exc == 0; */
	/* std::cout << ((allow_exc) ? "A" : "N") << ":" << trace[trace.size()-2] << ", " << v <<  ", " << local_trace.num_exc() << std::endl; */
	/* std::cout << v << "| " << trace << std::endl; */
	if (trace.size() > 1 && !local_trace.walk_vertex(trace[trace.size()-2], v,allow_exc)) {
		return false;
	} else if (allow_exc && local_trace.num_exc() == 1) {
		++rep_exc;
		/* std::cout << "EXC!\n"; */
	}
	/* std::cout << "WA " << local_trace << std::endl; */

	if (!is_walked(e)) {
		dir_walked[graph[e].idx] = std::pair<Vertex,Vertex>(trace.back(), v);
	}

	++num_walked[graph[e].idx];
	walk_vertex_gap(v);
	trace.push_back(v);
	return true;
}

void Traversor::backtrack_edge(Edge e) {
	assert(trace.size() > 1);
	assert(num_walked[graph[e].idx] > 0);

	--num_walked[graph[e].idx];
	backtrack_vertex_gap(trace.back());
	if (trace.size() > 2) {
		Vertex to = trace.back(), from = trace[trace.size() - 3];
		auto& local_trace = local_traces[trace[trace.size() - 2]];
		auto num_exc = local_trace.num_exc();
		local_trace.backtrack_vertex(from, to);
		if (local_trace.num_exc() < num_exc) { 
			/* std::cout << "NON! (" <<  trace << ")"<< rep_exc<< "\n"; */
			assert(rep_exc == 1);
			--rep_exc; 
		}
	}
	trace.pop_back();
	
}

bool Traversor::valid_cycle() {
	assert(!is_partial());
	if (!boost::edge(trace.back(), trace.front(), graph).second) { return false; }

	Vertex from = trace.back();
	Vertex to = trace[1];
	bool allow_exc = rep_exc == 0;
	/* int old = local_traces[trace.front()].num_exc(); */
	bool result = local_traces[trace.front()].walk_vertex(from, to, allow_exc);
	if (result) { 
		/* result = !allow_exc && local_traces[trace.front()].num_exc() == old; */
		/* result = result || (allow_exc && local_traces[trace.front()].num_exc() == 1); */
		local_traces[trace.front()].backtrack_vertex(from, to); 
		/* if (result) { */
		/* 	from = trace[trace.size() - 2]; */
		/* 	to = trace[0]; */
		/* 	old = local_traces[trace.back()].num_exc(); */
		/* 	result = local_traces[trace.back()].walk_vertex(from, to, allow_exc); */
		/* 	if (result) { */
		/* 		result = !allow_exc && local_traces[trace.back()].num_exc() == old; */
		/* 		result = result || (allow_exc && local_traces[trace.back()].num_exc() == 1); */
		/* 		local_traces[trace.back()].backtrack_vertex(from, to); */ 
		/* 	} */
		/* } */
	}
	return result;
}

void Traversor::traverse_dfs( Edge e, const PermList &p_fixed_auts
		, std::unordered_map<canon::Trace, std::vector<Vertex>> &out) {

	statPrinter.update();
	if (!walk_edge(e)) { return; }

	/* if (!valid_gap()) { */
	/* 	backtrack_edge(e); */
	/* 	return; */
	/* } */

	if (is_partial()) {
		const PermList fixed_auts(trace.back(), p_fixed_auts);
		for (auto ai = out_edges(trace.back(), graph); ai.first != ai.second; ++ai.first) {
			Vertex tar = boost::opposite(*ai.first, trace.back(), graph);
			if (fixed_auts.is_min_perm(tar)) { 
				traverse_dfs(*ai.first, fixed_auts, out);
			}
		}
	} else {
		if (valid_cycle()) {
			canon::Trace canon_trace = canon::Trace(gaps_f, gaps_b);
			if (out.find(canon_trace) == out.end()) {
				out[canon_trace] = trace;
				++stats.num_traces;
				/* std::cout << out.size() << std::endl; */
	/* for (Vertex v : trace) { */
	/* 	std::cout << v << " "; */
	/* } */
	/* std::cout << std::endl; */
			}
		}
	}
	backtrack_edge(e);
}

void Traversor::traverse_dfs_brute( Edge e, const PermList &p_fixed_auts
		, std::unordered_map<canon::EmbBrute, std::vector<Vertex>> &out) {

	statPrinter.update();
	if (!walk_edge(e)) { return; }

	if (is_partial()) {
		const PermList fixed_auts(trace.back(), p_fixed_auts);
		for (auto ai = out_edges(trace.back(), graph); ai.first != ai.second; ++ai.first) {
			Vertex tar = boost::opposite(*ai.first, trace.back(), graph);
			if (fixed_auts.is_min_perm(tar)) { 
				traverse_dfs_brute(*ai.first, fixed_auts, out);
			}
		}
	} else {
		if (valid_cycle()) {
			/* canon::Trace canon_trace = canon::Trace(gaps_f, gaps_b); */
			canon::EmbBrute canon_trace(trace, p_fixed_auts.get_perms());
			if (out.find(canon_trace) == out.end()) {
				out[canon_trace] = trace;
				++stats.num_traces;
			}
		}
	}
	backtrack_edge(e);
}

std::vector<std::pair<Vertex, Vertex>> Traversor::get_min_edges(const PermList& auts) {
	std::vector<std::pair<Vertex, Vertex>> min_edges(num_edges(graph));
	for (auto ei = edges(graph); ei.first != ei.second; ++ei.first) {
		Edge e = *ei.first;
		min_edges[graph[e].idx] = auts.get_min(e, graph);
		/* std::cout << source(e, graph) << ", " << target(e, graph) << ": " << min_edges[graph[e].idx].first << ", " << min_edges[graph[e].idx].second << std::endl; */
	}
	return min_edges;
}


std::unordered_map<canon::Embedding, std::vector<Face>> Traversor::find_two_face_embeddings() {
	std::unordered_map<canon::Trace, std::vector<Vertex>> out;
	std::unordered_map<canon::Embedding, std::vector<Face>> face_out;
	auto start_traversal = std::chrono::system_clock::now();

	if (!valid_euler(2, num_edges(graph), num_vertices(graph))) {
		stats.time = std::chrono::system_clock::now() - start_traversal;
		statPrinter.print_stats();
		return face_out;
	}
	stats.num_traces = 0;

	std::vector<Perm> raw_auts = find_automorphisms(graph);
	/* std::cout << "AUTS: " << raw_auts.size() << std::endl; */
	PermList auts(raw_auts);
	min_edges = get_min_edges(auts);
	for (auto ei = edges(graph); ei.first != ei.second; ++ei.first) {
		Edge e = *ei.first;
			
		Vertex src = source(e, graph), tar = target(e, graph);
		if (tar < src) { std::swap(src, tar); }
		if (min_edges[graph[e].idx] != std::make_pair(src, tar)) { continue; }
		/* if (boost::degree(src, graph) < boost::degree(tar, graph)) { std::swap(src, tar); } */
		/* std::cout << "CHECKING: " << src << ", " << tar << "min: " << min_edges[graph[e].idx].first << ", " << min_edges[graph[e].idx].second <<  std::endl; */
		walk_vertex_gap(src);
		trace.push_back(src);

		PermList fixed_auts(src, auts);
		fixed_auts.fix(tar);
		walk_edge(e, false);
		walk_edge(e, true);
		for (auto ei = out_edges(src, graph); ei.first != ei.second; ++ei.first) {
			traverse_dfs(*ei.first, fixed_auts, out);
		}
		backtrack_edge(e);
		backtrack_edge(e);

		backtrack_vertex_gap(src);
		trace.pop_back();
	}

	stats.dur_traversal = std::chrono::system_clock::now() - start_traversal;
	stats.num_traces = out.size();

	auto start_face_constr = std::chrono::system_clock::now();
	/* std::cout << "SORTING FACES " << out.size() << "\n"; */
	size_t count = 0;
	stats.num_cand_embeddings = 0;
	stats.num_embeddings = 0;
	for (auto& kv : out) {
		statPrinter.update();
		auto cand_faces = construct_faces2(kv.second);
		/* count += cand_faces.size(); */
		stats.num_cand_embeddings += cand_faces.size();
		for (const auto& faces : cand_faces) {
			/* NautCanon nc(faces); */
			canon::Embedding nc(faces, graph);
			if (face_out.find(nc) == face_out.end()) {
				face_out[nc] = faces;
				++stats.num_embeddings;
			}
			/* } else { */
			/* 	std::cout << "WAAIT\n"; */
			/* 	for (auto& f : faces) { */
			/* 		std::cout << f << "| "; */
			/* 	} */
			/* 	std::cout << std::endl; */
			/* 	for (auto& f : face_out[nc]) { */
			/* 		std::cout << f << "| "; */
			/* 	} */
			/* 	std::cout << std::endl; */
			/* } */
		}
	}
	/* std::cout << "DONE! " << count << ", " << face_out.size() <<"\n"; */
	/* stats.num_cand_embeddings = count; */
	stats.num_embeddings = face_out.size();
	stats.dur_face_construction = std::chrono::system_clock::now() - start_face_constr;
	stats.time = std::chrono::system_clock::now() - start_traversal;

	statPrinter.print_stats();
	return face_out;
}

struct MinEdge {
	MinEdge(Edge e, std::pair<Vertex, Vertex> min_edge, const Graph& g) 
		: e(e), min_edge(min_edge) {
		src = source(e, g), tar = target(e, g);
		if (src > tar) { std::swap(src, tar); }
	}

	bool operator > (const MinEdge& rhs) const {
		if ( min_edge > rhs.min_edge ) { return true; }
		else if (min_edge == rhs.min_edge) { 
			if (src > rhs.src || (src == rhs.src && tar > rhs.tar)) {
					return true; 
			}
		}
		return false;
	}

	Edge e;
	Vertex src, tar;
	std::pair<Vertex, Vertex> min_edge;
};

std::vector<std::vector<Face>> Traversor::construct_faces2(const std::vector<Vertex>& trace) const {
	std::vector<std::vector<Face>> embs;
	std::priority_queue<MinEdge, std::vector<MinEdge>, std::greater<MinEdge>> border_edges;
	std::vector<Edge> etrace = vertex_to_edge(graph, trace);
	std::vector<size_t> num_visits(num_edges(graph), 0);

	/* std::cout << trace << std::endl; */
	std::vector<size_t> cuts;
	border_edges.push(MinEdge(etrace[0], min_edges[graph[etrace[0]].idx], graph));
	for (size_t i = 2; i < etrace.size(); ++i) {
		Vertex inv = trace[(i + 1)%trace.size()];
		++num_visits[graph[etrace[i]].idx];
		if (num_visits[graph[etrace[i]].idx] == 1) {
			/* std::cout << "PUSHING: " << etrace[i] << " "; */
			border_edges.push(MinEdge(etrace[i], min_edges[graph[etrace[i]].idx], graph));
			/* std::cout << "TOP: " << border_edges.top().e << std::endl; */
		} else if (border_edges.top().e == etrace[i]) {
			/* std::cout << "POPPING: " << etrace[i] << std::endl; */
			while (num_visits[graph[border_edges.top().e].idx] == 2) {
			/* std::cout << "XPOPPING: " << border_edges.top().e  << std::endl; */
				border_edges.pop();
			}
			/* std::cout << "NEW TOP: " << border_edges.top().e  << std::endl; */
			/* std::cout << "LE: " << (border_edges.top() > me?"G":"LE") << std::endl; */
		}
		/* if (inv == trace[1]) { */
		/* 	std::cout << border_edges.top().e << " " << i+1 << " " << border_edges.size() << std::endl; */
		/* } */
		if (inv == trace[1] && border_edges.top().e == etrace[0]) { 
			cuts.push_back(i+1);
		}
	}
	/* std::cout << trace << std::endl; */
	for (size_t cut : cuts) {
		/* if (cut > trace.size() - cut + 2) { continue; } */
		/* std::cout << "(" << cut << ") "; */
		std::vector<Face> emb;
		emb.push_back(Face(trace.begin() + 1, trace.begin() + cut));
		/* std::cout << emb.back() << "| "; */
		emb.push_back(Face(trace.begin() + cut, trace.end()));
		emb.back().push_back(trace[0]);
		/* std::cout << emb.back() << "| "; */
		/* std::cout << std::endl; */
		embs.push_back(emb);
	}
	return embs;
}

std::vector<std::vector<Face>> Traversor::construct_faces(const std::vector<Vertex>& trace) const {
	assert(!is_partial());
	assert(rep_exc == 1);

	std::vector<std::vector<Face>> cand_faces;
	int cut1 = -1;
	std::vector<int> cuts;
	for (size_t i = 0; i < trace.size(); ++i) {
		size_t from = ((i == 0) ? trace.size() - 1 : i - 1);
		size_t to = (i + 1) % trace.size();
		if (trace[from] == trace[to] && degree(trace[i], graph) > 1) {
			cut1 = i;
			break;
		}
	}
	assert(cut1 != -1);
	
	bool seen_start = false;
	int tmp = (cut1 + 1) % trace.size();
	while (tmp != cut1) {
		if (!seen_start && trace[tmp] == trace[cut1]) {
			cuts.push_back(tmp);
		}
		tmp = (tmp + 1) % trace.size();
	}
	assert(cuts.size() > 0);

	for (int cut2 : cuts) {
		std::vector<Face> faces;
		Face u, w;
		int i = (cut1 + 1) % trace.size();
		u.push_back(trace[cut1]);
		while (i != cut2) {
			u.push_back(trace[i]);
			i = (i + 1) % trace.size();
		}
		while (cut1 != i) {
			w.push_back(trace[i]);
			i = (i + 1) % trace.size();
		}
		faces.push_back(u);
		faces.push_back(w);
		cand_faces.push_back(faces);
	}

	assert(cand_faces.size() > 0);
	return cand_faces;
}

bool anti_par(const Graph& g) {
	for (auto ei = edges(g); ei.first != ei.second; ++ei.first) {
		if (g[*ei.first].req_dir == EdgeDirection::ANY) {
			return false;
		}
	}
	return true;
}

std::unordered_map<canon::EmbBrute, std::vector<Vertex>> Traversor::find_strong_traces_brute() {
	auto start_traversal = std::chrono::system_clock::now();
	std::unordered_map<canon::EmbBrute, std::vector<Vertex>> out;

	if (anti_par(graph) && !valid_euler(1, num_edges(graph), num_vertices(graph))) {
		stats.time = std::chrono::system_clock::now() - start_traversal;
		statPrinter.print_stats();
		return out;
	}

	Vertex start = 0;
	walk_vertex_gap(start);
	trace.push_back(start);

	std::vector<Perm> raw_auts = morphism::find_automorphisms(graph);
	this->auts = morphism::get_all_permutations(raw_auts);
	PermList auts(start, this->auts);
	
	Edge e = *out_edges(start, graph).first;
	traverse_dfs_brute(e, auts, out);

	backtrack_vertex_gap(start);
	trace.pop_back();
	stats.num_embeddings = out.size();
	stats.time = std::chrono::system_clock::now() - start_traversal;
	statPrinter.print_stats();
	return out;
}

std::unordered_map<canon::Trace, std::vector<Vertex>> Traversor::find_strong_traces() {
	auto start_traversal = std::chrono::system_clock::now();
	std::unordered_map<canon::Trace, std::vector<Vertex>> out;

	if (anti_par(graph) && !valid_euler(1, num_edges(graph), num_vertices(graph))) {
		stats.time = std::chrono::system_clock::now() - start_traversal;
		statPrinter.print_stats();
		return out;
	}

	Vertex start = 0;
	walk_vertex_gap(start);
	trace.push_back(start);

	std::vector<Perm> raw_auts = morphism::find_automorphisms(graph);
	this->auts = morphism::get_all_permutations(raw_auts);
	/* PermList auts(start, raw_auts); */
	PermList auts(start, this->auts);
	
	Edge e = *out_edges(start, graph).first;
	traverse_dfs(e, auts, out);
	/* for (auto ai = out_edges(trace.back(), graph); ai.first != ai.second; ++ai.first) { */
	/* 	Vertex tar = boost::opposite(*ai.first, trace.back(), graph); */
	/* 	if (auts.is_min_perm(tar)) { */ 
	/* 		traverse_dfs(*ai.first, auts, out); */
	/* 	} */
	/* } */

	backtrack_vertex_gap(start);
	trace.pop_back();
	stats.num_embeddings = out.size();
	stats.time = std::chrono::system_clock::now() - start_traversal;
	statPrinter.print_stats();
	return out;
}

const Stats& Traversor::get_stats() const {
	return stats;
}
	
} /* namespace dtrace */
