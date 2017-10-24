#include <kface/Find.hpp>
#include "prune/None.hpp"
#include "prune/KNone.hpp"
#include "prune/Gap.hpp"
#include "prune/All.hpp"
#include "Trace.hpp"
#include "KTrace.hpp"
#include "prune/OOrderlyGen.hpp"
#include "Enumerate.hpp"
#include "Util.hpp"
#include <vector>
#include <iostream>
#include <signal.h>
#include "AutGroup.hpp"
#include <dtrace/canon/trace.h>
#include "RotationSystem.hpp"
#include "feasible/GapDist.hpp"
#include "feasible/EdgeGap.hpp"
#include "feasible/BioGap.hpp"
#include "feasible/MaxGap.hpp"
#include <kface/GapHist.hpp>
#include <kface/Gap.hpp>
#include "../BAD_TIME.hpp"

namespace kface {

#define CALL(fun, alg, feasible, ...) \
	switch(alg) { \
		case Brute  : return fun<prune::None,feasible>(__VA_ARGS__);\
		case Orderly: return fun<prune::OOrderlyGen,feasible>(__VA_ARGS__);\
		case Gap: return fun<prune::Gap,feasible>(__VA_ARGS__);\
		case All: return fun<prune::All,feasible>(__VA_ARGS__);\
	}

/* volatile bool cont = true; */ //located in morphism (JESUS)
extern volatile bool cont;
void handle(int) { cont = false; }

void set_alarm(int timeout) {
	signal(SIGALRM, &handle);
	alarm(timeout);
}

void cancel_alarm() {
	alarm(0);
}

template<typename Trace, typename Callback, typename CallbackMiss = NoCallback>
void generate_embeddings(const Graph& g, int timeout, Trace& t, Callback& cb
		, CallbackMiss cb_miss = CallbackMiss{}) {
	/* cont = true; */
	/* if (!valid_euler(1, num_edges(g), num_vertices(g))) { return; } */
	/* set_alarm(timeout); */
	auto should_continue = [&](const std::vector<Vertex>&) { return cont; };
	extend_edge(t, cb, should_continue, cb_miss);
	cancel_alarm();
}

/* template<typename Trace, typename Callback> */
/* void generate_embeddings_edgewise(const Graph& g, int timeout, Trace& t, Callback& cb) { */
/* 	cont = true; */
/* 	if (!valid_euler(1, num_edges(g), num_vertices(g))) { return; } */
/* 	set_alarm(timeout); */
/* 	auto should_continue = [&](const std::vector<Vertex>&) { return cont; }; */
/* 	extend_edge(t, cb, should_continue); */
/* 	cancel_alarm(); */
/* } */

template<typename PruneAlg, typename Feasible=RotationSystem>
std::pair<unsigned long, bool> count_one_face(const Graph& g, int timeout) {
	/* try { */
	cont = true;
	if (!valid_euler(1, num_edges(g), num_vertices(g))) { std::make_pair(0, true); }
	set_alarm(timeout);
	Trace<PruneAlg,Feasible> t(*out_edges(0, g).first, g);
	unsigned long walks = 0;
	auto count = [&] (const std::vector<Vertex>&) { ++walks; };
	generate_embeddings(g, timeout, t, count);
	return std::make_pair(walks, cont);
	/* } catch (int) { */
	/* 	return std::make_pair(0, false); */
	/* } */
}

template<typename PruneAlg,typename Feasible>
std::pair<unsigned long, bool> count_one_face(const Graph& g, int timeout, size_t min_gap, size_t max_gap) {
	cont = true;
	if (!valid_euler(1, num_edges(g), num_vertices(g))) { std::make_pair(0, true); }
	set_alarm(timeout);
	Trace<PruneAlg,Feasible> t(*out_edges(0, g).first, g);
	t.bounds(min_gap, max_gap);
	unsigned long walks = 0;
	auto count = [&] (const std::vector<Vertex>&) { ++walks; };
	generate_embeddings(g, timeout, t, count);
	return std::make_pair(walks, cont);
}

/* template<typename PruneAlg, typename Feasible> */
/* std::pair<unsigned long, bool> count_embeddings_egap(const Graph& g, int */
/* 		timeout, size_t min_gap, size_t max_gap) { */
/* 	Trace<PruneAlg,Feasible> t(*out_edges(0, g).first, g); */
/* 	t.bounds(min_gap, max_gap); */
/* 	unsigned long walks = 0; */
/* 	auto count = [&] (const std::vector<Vertex>&) { ++walks; }; */
/* 	generate_embeddings_edgewise(g, timeout, t, count); */
/* 	return std::make_pair(walks, cont); */
/* } */

template<typename PruneAlg, typename Feasible>
std::tuple<unsigned long,size_t, bool> count_max_embeddings(const Graph& g, int timeout) {
	cont = true;
	if (!valid_euler(1, num_edges(g), num_vertices(g))) { std::make_pair(0, true); }
	set_alarm(timeout);
	Trace<PruneAlg, feasible::MaxGap<Feasible>> t(*out_edges(0, g).first, g);
	unsigned long walks = 0;
	size_t max_gap = num_edges(g) * 2+1;
	t.bounds(0,max_gap);
	auto count = [&] (const std::vector<Vertex>&) { 
		auto bgap = build_bio_gap(g, t.walk());
		auto max = *(std::max_element(bgap.begin(), bgap.end()));
		if (t.max_gap() < max_gap) {
			walks = 0;
			max_gap = t.max_gap();
			t.bounds(0, t.max_gap());
		}
		++walks; 
	};
	generate_embeddings(g, timeout, t, count);
	return std::make_tuple(walks, max_gap, cont);
}

template<typename PruneAlg, typename Feasible>
std::tuple<unsigned long, unsigned long, bool> count_one_face_and_misses(const Graph& g, int timeout) {
	cont = true;
	if (!valid_euler(1, num_edges(g), num_vertices(g))) { std::make_pair(0, true); }
	set_alarm(timeout);
	Trace<PruneAlg,Feasible> t(*out_edges(0, g).first, g);
	unsigned long num_walks = 0, num_misses = 0;
	auto count_found = [&] (const std::vector<Vertex>&) { ++num_walks; };
	auto count_misses = [&] () { ++num_misses; };
	generate_embeddings(g, timeout, t, count_found, count_misses);
	return std::make_tuple(num_walks, num_misses, cont);
}

std::tuple<unsigned long, unsigned long, bool> count_one_face_and_misses(const Graph& g, 
		Algorithm alg, int timeout) {
	CALL(count_one_face_and_misses, alg, RotationSystem, g, timeout);
}

/* template<typename PruneAlg> */
/* std::tuple<unsigned long, size_t, bool> count_max_embeddings_edge(const Graph& g, int timeout) { */
/* 	Trace<PruneAlg, feasible::MaxGap<feasible::EdgeGap>> t(*out_edges(0, g).first, g); */
/* 	unsigned long walks = 0; */
/* 	size_t max_gap = num_edges(g) * 2+1; */
/* 	t.bounds(0,max_gap); */
/* 	auto count = [&] (const std::vector<Vertex>&) { */ 
/* 		if (t.max_gap() < max_gap) { */
/* 			walks = 0; */
/* 			max_gap = t.max_gap(); */
/* 			t.bounds(0, t.max_gap()); */
/* 		} */
/* 		++walks; */ 
/* 	}; */
/* 	generate_embeddings_edgewise(g, timeout, t, count); */
/* 	return std::make_tuple(walks, max_gap, cont); */
/* } */

std::tuple<unsigned long, size_t, bool> count_max_embeddings(const Graph& g, Algorithm
		alg, int timeout, GapRepr gr) {
	switch (gr) {
		case GapRepr::None: CALL(count_max_embeddings, alg, feasible::GapDist, g, timeout);
		case GapRepr::Vertex: CALL(count_max_embeddings, alg, feasible::GapDist, g, timeout);
		case GapRepr::Edge: CALL(count_max_embeddings, alg, feasible::EdgeGap, g, timeout);
		case GapRepr::Bio: CALL(count_max_embeddings, alg, feasible::BioGap, g, timeout);
	};
	/* if (use_edge_gap) { */
	/* 	CALL(count_max_embeddings, alg, feasible::EdgeGap, g, timeout); */
	/* } else { */
	/* } */
}

template<typename PruneAlg, typename Feasible>
std::vector<std::vector<Vertex>> find_one_face(const Graph& g) {
	std::vector<std::vector<Vertex>> embeddings;
	Trace<PruneAlg,Feasible> t(*out_edges(0, g).first, g);
	auto collect = [&] (const std::vector<Vertex>& walk) { embeddings.push_back(walk); };
	generate_embeddings(g, 0, t, collect);
	/* extend(t, collect); */
	return embeddings;
}

template<typename PruneAlg, typename Feasible>
std::vector<std::vector<Vertex>> find_one_face(const Graph& g, size_t min_gap, size_t max_gap) {
	std::vector<std::vector<Vertex>> embeddings;
	Trace<PruneAlg, Feasible> t(*out_edges(0, g).first, g);
	t.bounds(min_gap, max_gap);
	auto collect = [&] (const std::vector<Vertex>& walk) { embeddings.push_back(walk); };
	generate_embeddings(g, 0, t, collect);
	return embeddings;
}

/* template<typename PruneAlg> */
/* std::vector<std::vector<Vertex>> find_one_face_egap(const Graph& g, size_t min_gap, size_t max_gap) { */
/* 	std::vector<std::vector<Vertex>> embeddings; */
/* 	Trace<PruneAlg,feasible::EdgeGap> t(*out_edges(0, g).first, g); */
/* 	t.bounds(min_gap, max_gap); */
/* 	auto collect = [&] (const std::vector<Vertex>& walk) { embeddings.push_back(walk); }; */
/* 	generate_embeddings_edgewise(g, 0, t, collect); */
/* 	return embeddings; */
/* } */

template<typename PruneAlg, typename Feasible>
std::pair<GapHist,bool> find_gap_hist(const Graph& g, int timeout, GapRepr gap_repr) {
	GapHist gh(g, gap_repr);
	Trace<PruneAlg, Feasible> t(*out_edges(0, g).first, g);
	auto collect = [&] (const std::vector<Vertex>& walk) { gh.add_walk(walk); };
	generate_embeddings(g, timeout, t, collect);
	return std::make_pair(gh, cont);
}

std::pair<GapHist,bool> find_gap_hist(const Graph& g, Algorithm alg, GapRepr gap_repr, int timeout) {
	CALL(find_gap_hist, alg, RotationSystem, g, timeout, gap_repr);
}

std::vector<std::vector<Vertex>> find_one_face(const Graph& g, Algorithm alg) {
	CALL(find_one_face, alg, RotationSystem, g);
}

std::vector<std::vector<Vertex>> find_one_face(const Graph& g, Algorithm alg,
		size_t min_gap, size_t max_gap, GapRepr gap_repr) {
	switch(gap_repr) {
		case GapRepr::None: CALL(find_one_face, alg, feasible::GapDist, g, min_gap, max_gap);
		case GapRepr::Vertex: CALL(find_one_face, alg, feasible::GapDist, g, min_gap, max_gap);
		case GapRepr::Edge: CALL(find_one_face, alg, feasible::EdgeGap, g, min_gap, max_gap);
		case GapRepr::Bio: CALL(find_one_face, alg, feasible::BioGap, g, min_gap, max_gap);
	}
	/* if (use_edge_gap) { */
	/* 	CALL(find_one_face, alg, feasible::EdgeGap, g, min_gap, max_gap); */
	/* } else { */
	/* 	CALL(find_one_face, alg, feasible::GapDist, g, min_gap, max_gap); */
	/* } */
	/* switch(alg) { */
	/* 	case Brute  : return find_one_face<prune::None>(g, min_gap, max_gap); */
	/* 	case Orderly: return find_one_face<prune::OOrderlyGen>(g, min_gap, max_gap); */
	/* 	case Gap: return find_one_face<prune::Gap>(g, min_gap, max_gap); */
	/* 	case All: return find_one_face<prune::All>(g, min_gap, max_gap); */
	/* } */
}

std::pair<unsigned long, bool> count_one_face(const Graph& g, int timeout) {
	return count_one_face<prune::OOrderlyGen>(g, timeout);
}

std::pair<unsigned long, bool> count_one_face(const Graph& g, int timeout,
		Algorithm alg, size_t min_gap, size_t max_gap, GapRepr gap_repr) {
	/* if (timeout == 0) { return std::make_pair(count_one_face(g, alg, min_gap, max_gap), true); } */

	switch (gap_repr) {
		case GapRepr::None: CALL(count_one_face, alg, feasible::GapDist, g, timeout, min_gap, max_gap);
		case GapRepr::Vertex: CALL(count_one_face, alg, feasible::GapDist, g, timeout, min_gap, max_gap);
		case GapRepr::Edge: CALL(count_one_face, alg, feasible::EdgeGap, g, timeout, min_gap, max_gap);
		case GapRepr::Bio: CALL(count_one_face, alg, feasible::BioGap, g, timeout, min_gap, max_gap);
	}
	/* if (use_edge_gap) { */
	/* 	CALL(count_one_face, alg, feasible::EdgeGap, g, timeout, min_gap, max_gap); */
	/* } else { */
	/* 	CALL(count_one_face, alg, feasible::GapDist, g, timeout, min_gap, max_gap); */
	/* 	/1* CALL(alg, count_one_face, g, timeout, min_gap, max_gap); *1/ */
	/* } */
	/* switch(alg) { */
	/* 	case Brute  : return count_one_face<prune::None>(g, timeout, min_gap, max_gap); */
	/* 	case Orderly: return count_one_face<prune::OOrderlyGen>(g, timeout, min_gap, max_gap); */
	/* 	case Gap: return count_one_face<prune::Gap>(g, timeout, min_gap, max_gap); */
	/* 	case All: return count_one_face<prune::All>(g, timeout, min_gap, max_gap); */
	/* } */
}

std::pair<unsigned long, bool> count_one_face(const Graph& g, int timeout, Algorithm alg) {
	if (timeout == 0) { return std::make_pair(count_one_face(g, alg), true); }

	switch(alg) {
		case Brute  : return count_one_face<prune::None>(g, timeout);
		case Orderly: return count_one_face<prune::OOrderlyGen>(g, timeout);
		case Gap: return count_one_face<prune::Gap>(g, timeout);
		case All: return count_one_face<prune::All>(g, timeout);
	}
}

template<typename PruneAlg>
unsigned long count_one_face(const Graph& g) {
	if (!valid_euler(1,num_edges(g), num_vertices(g))) { return 0; }

	Trace<PruneAlg, RotationSystem> t(*out_edges(0, g).first, g);
	unsigned long walks = 0;
	auto count = [&] (const std::vector<Vertex>&) { ++walks; };
	extend(t, count);
	return walks;
}

template<typename PruneAlg>
unsigned long count_one_face(const Graph& g, size_t min_gap, size_t max_gap) {
	if (!valid_euler(1,num_edges(g), num_vertices(g))) { return 0; }

	Trace<PruneAlg,feasible::GapDist> t(*out_edges(0, g).first, g);
	t.bounds(min_gap, max_gap);
	unsigned long walks = 0;
	auto count = [&] (const std::vector<Vertex>&) { ++walks; };
	extend(t, count);
	return walks;
}

unsigned long count_one_face(const Graph& g, Algorithm alg, size_t min_gap, size_t max_gap) {
	switch(alg) {
		case Brute  : return count_one_face<prune::None>(g, min_gap, max_gap);
		case Orderly: return count_one_face<prune::OOrderlyGen>(g, min_gap, max_gap);
		case Gap: return count_one_face<prune::Gap>(g, min_gap, max_gap);
		case All: return count_one_face<prune::All>(g, min_gap, max_gap);
	}
}

unsigned long count_one_face(const Graph& g, Algorithm alg) {
	switch(alg) {
		case Brute  : return count_one_face<prune::None>(g);
		case Orderly: return count_one_face<prune::OOrderlyGen>(g);
		case Gap: return count_one_face<prune::Gap>(g);
		case All: return count_one_face<prune::All>(g);
	}
}

unsigned long count_one_face(const Graph& g) {
	return count_one_face(g, Algorithm::Orderly);
}

} /* namespace kface */
