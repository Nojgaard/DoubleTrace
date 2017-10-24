#ifndef KFACE_TRACE_HPP
#define KFACE_TRACE_HPP

/* #include "LocalRotation.hpp" */
#include "RotationSystem.hpp"
#include <kface/Graph.hpp>
#include <vector>

namespace kface {
	
template<typename EqClass, typename Feasible>
class Trace {
public:
	Trace(const Edge& e, const Graph& g);
	Trace(const Edge& e, const Graph& g, bool& complete);

	bool is_feasible(Vertex v) const;
	bool is_canonical(Vertex v);
	bool is_feasible(Edge e) const;
	bool is_feasible() const;
	bool is_canonical(Edge e);
	bool is_canonical();
	bool is_partial() const;
	void extend(Vertex v);
	void extend(Edge e);
	void backtrack();
	size_t max_gap() const;

	Vertex back() const;

	const std::vector<Vertex>& walk() const;
	const Graph& g() const;
	void bounds(size_t min, size_t max) { m_feasible.bounds(min, max); }

private:
	/* data */
	const Graph& m_g;
	/* std::vector<LocalRotation> m_rotation; */
	/* RotationSystem m_rotation; */
	std::vector<Vertex> m_walk;
	Feasible m_feasible;
	EqClass group;
};

template<typename EC, typename F>
Trace<EC,F>::Trace(const Edge& e, const Graph& g): m_g(g), m_walk({source(e, g), target(e, g)})
	                                        , m_feasible(m_g, m_walk), group(g, m_walk) {
	/* for (size_t i = 0; i < num_vertices(g); ++i) { */
	/* 	m_rotation.push_back(LocalRotation(i, g)); */
	/* } */
}

template<typename EC, typename F>
Vertex Trace<EC,F>::back() const { return m_walk.back(); }

template<typename EC, typename F>
const std::vector<Vertex>& Trace<EC,F>::walk() const {
	return m_walk;
}

template<typename EC, typename F>
const Graph& Trace<EC,F>::g() const {
	return m_g;
}

template<typename EC, typename F>
bool Trace<EC,F>::is_partial() const {
	return m_walk.size() < (num_edges(m_g) * 2);
}

template<typename EC, typename F>
bool Trace<EC,F>::is_canonical() {
	return group.is_canonical();
}

template<typename EC, typename F>
bool Trace<EC,F>::is_feasible(Vertex v) const {
	/* Vertex from = m_walk[m_walk.size() - 2]; */
	/* Vertex to = v; */
	/* return m_feasible.is_valid(m_walk.back(), from, to) */ 
	/* 	&& !(m_walk[0] == m_walk.back() && m_walk[1] == v); */
	return m_feasible.is_valid(v) 
		&& !(m_walk[0] == m_walk.back() && m_walk[1] == v);
	/* return m_rotation[m_walk.back()].is_valid(from, to) */ 
	/* 	&& !(m_walk[0] == m_walk.back() && m_walk[1] == v); */
	/* bool res = m_rotation[m_walk.back()].is_valid(from, to); */

	/* //Don't start a new face */
	/* if (res && m_walk.size() == full_size() - 1) { */
	/* 	return (m_rotation[v].is_valid(m_walk.back(), m_walk[0]) */
	/* 	     && m_rotation[m_walk[0]].is_valid(v, m_walk[1])); */
	/* } */
	/* return res; */
}

template <typename EC, typename F>
bool Trace<EC,F>::is_feasible(Edge e) const {
	Vertex v = target(e,m_g);
	return m_feasible.is_valid(e) 
		&& !(m_walk[0] == m_walk.back() && m_walk[1] == v);
}

template <typename EC, typename F>
bool Trace<EC,F>::is_feasible() const {
	return m_feasible.is_valid() ;
}

template<typename EC, typename F>
bool Trace<EC,F>::is_canonical(Vertex v) {
	return group.is_canonical(v);
}

template<typename EC, typename F>
bool Trace<EC,F>::is_canonical(Edge e) {
	return is_canonical(target(e,m_g));
}

template<typename EC, typename F>
void Trace<EC,F>::extend(Vertex v) {
	assert(is_partial());
	/* m_rotation[m_walk.back()].fix(m_walk[m_walk.size() - 2], v); */
	/* m_feasible.fix(m_walk.back(), m_walk[m_walk.size() - 2], v); */
	m_feasible.fix(v);
	group.prune(v);
	m_walk.push_back(v);
}

template<typename EC, typename F>
void Trace<EC,F>::extend(Edge e) {
	Vertex v = target(e, m_g);
	/* m_feasible.fix(m_walk.back(), m_walk[m_walk.size() - 2], v); */
	m_feasible.fix(e);
	group.prune(v);
	m_walk.push_back(v);
}

template<typename EC, typename F>
void Trace<EC,F>::backtrack() {
	assert(m_walk.size() > 2);
	/* Vertex v = m_walk[m_walk.size() - 2]; */
	/* Vertex from = m_walk[m_walk.size() - 3]; */
	/* Vertex to = m_walk.back(); */
	/* m_rotation[v].backtrack(from, to); */
	/* m_feasible.backtrack(v, from, to); */
	m_feasible.backtrack();
	group.backtrack();
	m_walk.pop_back();
}

template<typename EC, typename F>
size_t Trace<EC,F>::max_gap() const {
	return m_feasible.max_gap();
}

template<typename EC, typename F>
inline std::ostream& operator << (std::ostream& os, const Trace<EC,F>& t) {
	for (Vertex v : t.walk()) {
		os << v << " ";
	}
	return os;
}

} /* namespace kface */

#endif /* ifndef KFACE_TRACE_HPP */
