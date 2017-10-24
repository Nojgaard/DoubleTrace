#ifndef KFACE_KTRACE_HPP
#define KFACE_KTRACE_HPP

#include "RotationSystem.hpp"
#include <kface/Graph.hpp>
#include <vector>

namespace kface {
	
template<typename EqClass>
class KTrace {
public:
	KTrace(const Edge& e, const Graph& g, int k);

	bool is_feasible(Vertex v) const;
	bool is_canonical(Vertex v);
	bool is_canonical() const;
	bool is_partial() const;
	void extend(Vertex v);
	void extend_face(Vertex v);
	void backtrack();
	void backtrack_face();

	Vertex back() const;

	const std::vector<Vertex>& walk() const;
	const std::vector<size_t>& fidx() const;
	const Graph& g() const;

private:
	bool is_parallel(Vertex v) const;

	/* data */
	int m_k;
	const Graph& m_g;
	std::vector<Vertex> m_walk;
	std::vector<size_t> m_fidx;
	RotationSystem m_rotation;
	EqClass group;
};

template<typename EC>
KTrace<EC>::KTrace(const Edge& e, const Graph& g, int k): m_k(k), m_g(g)
                                                        , m_walk({source(e, g), target(e, g)})
                                                        , m_fidx(1, 0)
	                                                     , m_rotation(m_g), group(g, m_walk, m_fidx) { }

template<typename EC>
inline Vertex KTrace<EC>::back() const { return m_walk.back(); }

template<typename EC>
inline const std::vector<size_t>& KTrace<EC>::fidx() const { return m_fidx; }

template<typename EC>
inline bool KTrace<EC>::is_parallel(Vertex v) const { 
	return m_walk[m_fidx.back()] == m_walk.back() && m_walk[m_fidx.back() + 1] == v;
}

template<typename EC>
const std::vector<Vertex>& KTrace<EC>::walk() const {
	return m_walk;
}

template<typename EC>
const Graph& KTrace<EC>::g() const {
	return m_g;
}

template<typename EC>
bool KTrace<EC>::is_partial() const {
	return m_walk.size() < (num_edges(m_g) * 2);
}

template<typename EC>
bool KTrace<EC>::is_canonical() const {
	return group.is_canonical();
}

template<typename EC>
bool KTrace<EC>::is_feasible(Vertex v) const {
	Vertex from = m_walk[m_walk.size() - 2];
	Vertex to = v;
	/* std::cout << m_rotation.is_valid(m_walk.back(), from, to); */
	/* std::cout << " " << is_parallel(v) << " f: " << (m_k < m_fidx.size()) << " v: " << v; */
	/* std::cout << " k: " << m_k << " si: " << m_fidx.size() << std::endl; */
	return m_rotation.is_valid(m_walk.back(), from, to) 
		&& (!is_parallel(v) || m_k > m_fidx.size());
}

template<typename EC>
bool KTrace<EC>::is_canonical(Vertex v) {
	/* return (is_parallel(v)) ? group.is_canonical_face() : group.is_canonical(v); */
	return group.is_canonical(v);
}

template<typename EC>
void KTrace<EC>::extend(Vertex v) {
	assert(is_partial());
	if (is_parallel(v)) {
		extend_face(v);
	} else {
		m_rotation.fix(m_walk.back(), m_walk[m_walk.size() - 2], v);
		group.prune(v);
		m_walk.push_back(v);
	}
}

template<typename EC>
void KTrace<EC>::extend_face(Vertex v) {
	/* std::cout << "EXTEND NEW FACE!\n"; */
	assert(is_parallel(v));
	assert(m_rotation.is_valid(m_walk.back(), m_walk[m_walk.size() - 2], v));
	m_rotation.fix(m_walk.back(), m_walk[m_walk.size() - 2], v);
	m_walk.pop_back();
	m_fidx.push_back(m_walk.size());
	auto e = m_rotation.get_min_unfixed();
	m_walk.push_back(e.first);
	m_walk.push_back(e.second);
}

template<typename EC>
void KTrace<EC>::backtrack() {
	assert(m_walk.size() > 2);
	if (m_walk.size() - m_fidx.back() == 2) {
		backtrack_face();
	} else {
		Vertex v = m_walk[m_walk.size() - 2];
		Vertex from = m_walk[m_walk.size() - 3];
		Vertex to = m_walk.back();
		m_rotation.backtrack(v, from, to);
		group.backtrack();
		m_walk.pop_back();
	}
}

template<typename EC>
void KTrace<EC>::backtrack_face() {
	/* std::cout << "BACTRACK NEW FACE!\n"; */
	m_fidx.pop_back();
	m_walk.pop_back();
	m_walk.pop_back();
	m_walk.push_back(m_walk[0]);
	m_rotation.backtrack(m_walk.back(), m_walk[m_walk.size() - 2], m_walk[1]);
}

template<typename EC>
inline std::ostream& operator << (std::ostream& os, const KTrace<EC>& t) {
	size_t cur_f = 1, i = 0;
	for (Vertex v : t.walk()) {
		if (t.fidx().size() > cur_f && i == t.fidx()[cur_f]) {
			++cur_f;
			os << "| ";
		}
		os << v << " ";
		++i;
	}
	return os;
}

} /* namespace kface */

#endif /* ifndef KFACE_TRACE_HPP */
