#ifndef DTRACE_TRAVERSOR_H
#define DTRACE_TRAVERSOR_H
#include <dtrace/graph/base.h>
#include <dtrace/perm_list.h>
#include <dtrace/local_trace.h>
#include <dtrace/canon/embedding.h>
#include <dtrace/canon/trace.h>
#include <dtrace/stats.h>
#include <dtrace/stat_printer.h>
#include <unordered_map>
#include <unordered_set>
#include <dtrace/canon/emb_brute.h>

namespace dtrace {

class Traversor {
public:
	Traversor(const Graph &g, bool print_progress = true);

	std::unordered_map<canon::Embedding, std::vector<Face>> find_two_face_embeddings();
	std::unordered_map<canon::Trace, std::vector<Vertex>> find_strong_traces();
	std::unordered_map<canon::EmbBrute, std::vector<Vertex>> find_strong_traces_brute();
	const Stats& get_stats() const;
private:
	bool is_partial() const;
	bool is_walked(Edge e) const;
	bool is_full(Edge e) const;
	bool check_direction(Vertex v, Edge e) const;
	size_t full_trace_size() const;
	std::vector<std::vector<Face>> construct_faces(const std::vector<Vertex>& trace) const;
	std::vector<std::vector<Face>> construct_faces2(const std::vector<Vertex>& trace) const;

	void walk_vertex_gap(Vertex v);
	void backtrack_vertex_gap(Vertex v);

	bool walk_edge(Edge e, bool allow_exc = false);
	void backtrack_edge(Edge e);

	bool valid_cycle();
	bool valid_gap();
	
	void traverse_dfs( Edge e, const PermList &p_fixed_auts
						  , std::unordered_map<canon::Trace, std::vector<Vertex>> &out);
	void traverse_dfs_brute( Edge e, const PermList &p_fixed_auts
						  , std::unordered_map<canon::EmbBrute, std::vector<Vertex>> &out);
	std::vector<std::pair<Vertex, Vertex>> get_min_edges(const PermList& auts);

	const Graph &graph;
	size_t full_size;
	int rep_exc = 0;
	std::vector<Vertex> trace;
	std::vector<int> num_walked;
	std::vector<std::pair<Vertex, Vertex>> dir_walked;
	std::vector<LocalTrace> local_traces;
	std::vector<std::vector<size_t>> last_seen;
	std::vector<size_t> gaps_f, gaps_b;
	std::vector<std::pair<Vertex, Vertex>> min_edges;
	Stats stats;
	StatPrinter statPrinter;
	std::unordered_set<canon::EmbBrute> seen_cycle;
	std::vector<morphism::Perm> auts;
};

} /* namespace dtrace */
#endif /* ifndef DTRACE_TRACE_TRAVERSOR */
