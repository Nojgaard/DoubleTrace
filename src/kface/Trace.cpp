#include "Trace.hpp"

namespace kface {

/* template<typename EC> */
/* Trace<EC>::Trace(const Edge& e, const Graph& g): m_g(g), m_walk({source(e, g), target(e, g)}) */
/* 	                                        , group(g, m_walk) { */
/* 	/1* m_walk.push_back(source(e, g)); *1/ */
/* 	/1* m_walk.push_back(target(e, g)); *1/ */
/* 	/1* m_rotation.reserve(num_vertices(g)); *1/ */
/* 	for (size_t i = 0; i < num_vertices(g); ++i) { */
/* 		m_rotation.push_back(LocalRotation(i, g)); */
/* 	} */
	
/* 	/1* std::cout << group; *1/ */
/* } */

/* template<typename EC> */
/* Vertex Trace<EC>::back() const { return m_walk.back(); } */

/* template<typename EC> */
/* const Vertex& Trace<EC>::operator [] (size_t i) const { return m_walk[i]; } */

/* template<typename EC> */
/* const std::vector<Vertex>& Trace<EC>::walk() const { */
/* 	return m_walk; */
/* } */

/* template<typename EC> */
/* const Graph& Trace<EC>::g() const { */
/* 	return m_g; */
/* } */

/* template<typename EC> */
/* bool Trace<EC>::is_partial() const { */
/* 	return size() < full_size(); */
/* } */

/* template<typename EC> */
/* size_t Trace<EC>::size() const { */
/* 	return m_walk.size(); */
/* } */

/* template<typename EC> */
/* size_t Trace<EC>::full_size() const { */
/* 	return num_edges(m_g) * 2; */
/* } */

/* /1* bool Trace::is_canonical_full() const { *1/ */
/* /1* 	return group.is_canon(); *1/ */
/* /1* } *1/ */
/* /1* bool Trace::is_canonical() const { *1/ */
/* /1* 	return m_canon.is_canonical(); *1/ */
/* 	//reverse */
/* 	/1* for (size_t i = 0; i < size(); ++i) { *1/ */
/* 	/1* 	size_t j = size()- 1 - i; *1/ */
/* 	/1* 	if (walk()[i] < walk()[j]) { break; } *1/ */
/* 	/1* 	else if (walk()[i] > walk()[j]) { return false; } *1/ */
/* 	/1* } *1/ */

/* 	/1* for (auto& perm : group.perms()) { *1/ */
/* 	/1* 	for (size_t i = 0; i < size(); ++i) { *1/ */
/* 	/1* 		size_t j = size()- 1 - i; *1/ */
/* 	/1* 		if (walk()[i] < perm[walk()[j]]) { break; } *1/ */
/* 	/1* 		else if (walk()[i] > perm[walk()[j]]) { return false; } *1/ */
/* 	/1* 	} *1/ */
/* 	/1* 	if (is_partial()) { *1/ */
/* 	/1* 		size_t k = 0, i = 1; *1/ */
/* 	/1* 		while (i + k < size()) { *1/ */
/* 	/1* 			size_t ik = i + k; *1/ */
/* 	/1* 			if (walk()[k] == perm[walk()[ik]]) { ++k; } *1/ */
/* 	/1* 			else if (walk()[k] > perm[walk()[ik]]) { return false; } *1/ */
/* 	/1* 			else { *1/ */
/* 	/1* 				i = i + k + 1; *1/ */
/* 	/1* 				k = 0; *1/ */
/* 	/1* 			} *1/ */
/* 	/1* 		} *1/ */
/* 	/1* 	} else { *1/ */
/* 	/1* 		size_t i = 0, j = 1, k = 0; *1/ */
/* 	/1* 		while (i < size() && j < size() && k < size()) { *1/ */
/* 	/1* 			size_t ik = k; *1/ */
/* 	/1* 			size_t jk = (j + k) % size(); *1/ */
/* 	/1* 			if (walk()[ik] == perm[walk()[jk]]) { ++k; } *1/ */ 
/* 	/1* 			else if (walk()[ik] > perm[walk()[jk]]) { *1/ */
/* 	/1* 				return false; *1/ */
/* 	/1* 			} else { *1/ */
/* 	/1* 				j = j + k + 1; *1/ */
/* 	/1* 				if (j <= i) { j = i + 1; } *1/ */
/* 	/1* 				k = 0; *1/ */
/* 	/1* 			} *1/ */
/* 	/1* 		} *1/ */
/* 	/1* 		i = 0, j = 0, k = 0; *1/ */
/* 	/1* 		while (i < size() && j < size() && k < size()) { *1/ */
/* 	/1* 			size_t ik = k; *1/ */
/* 	/1* 			size_t jk = size() - 1 - ((j + k) % size()); *1/ */
/* 	/1* 			if (walk()[ik] == perm[walk()[jk]]) { ++k; } *1/ */ 
/* 	/1* 			else if (walk()[ik] > perm[walk()[jk]]) { *1/ */
/* 	/1* 				return false; *1/ */
/* 	/1* 			} else { *1/ */
/* 	/1* 				j = j + k + 1; *1/ */
/* 	/1* 				if (j <= i) { j = i + 1; } *1/ */
/* 	/1* 				k = 0; *1/ */
/* 	/1* 			} *1/ */
/* 	/1* 		} *1/ */
/* 	/1* 	} *1/ */
/* 	/1* } *1/ */

/* 	/1* size_t i = 0, j = 1, k = 0; *1/ */
/* 	/1* while (j+k < size()) { *1/ */
/* 	/1* 	size_t ik = (i + k); *1/ */
/* 	/1* 	size_t jk = (j + k); *1/ */
/* 	/1* 	if (walk()[ik] == walk()[jk]) { ++k; } *1/ */ 
/* 	/1* 	else if (walk()[ik] > walk()[jk]) { *1/ */
/* 	/1* 		std::cout << "HMMMMM\n"; *1/ */
/* 	/1* 		return false; *1/ */
/* 	/1* 	} else { *1/ */
/* 	/1* 		j = j + k + 1; *1/ */
/* 	/1* 		if (j <= i) { j = i + 1; } *1/ */
/* 	/1* 		k = 0; *1/ */
/* 	/1* 	} *1/ */
/* 	/1* } *1/ */

/* 	/1* return true; *1/ */
/* /1* } *1/ */

/* template<typename EC> */
/* bool Trace<EC>::is_valid_extension(Vertex v) const { */
/* 	Vertex from = m_walk[m_walk.size() - 2]; */
/* 	Vertex to = v; */
/* 	return m_rotation[m_walk.back()].is_valid(from, to) */ 
/* 		&& !(m_walk[0] == m_walk.back() && m_walk[1] == v); */
/* 	/1* bool res = m_rotation[m_walk.back()].is_valid(from, to); *1/ */

/* 	/1* //Don't start a new face *1/ */
/* 	/1* res = res && !(m_walk[0] == m_walk.back() && m_walk[1] == v); *1/ */
/* 	/1* if (res && m_walk.size() == full_size() - 1) { *1/ */
/* 	/1* 	return (m_rotation[v].is_valid(m_walk.back(), m_walk[0]) *1/ */
/* 	/1* 	     && m_rotation[m_walk[0]].is_valid(v, m_walk[1])); *1/ */
/* 	/1* } *1/ */
/* 	/1* return res; *1/ */
/* } */

/* /1* bool Trace::is_canon_extension(Vertex v) const { *1/ */
/* /1* 	return group.is_min(v) && group.is_canon_partial(v); *1/ */
/* /1* } *1/ */

/* template<typename EC> */
/* void Trace<EC>::extend(Vertex v) { */
/* 	assert(is_partial()); */
/* 	m_rotation[m_walk.back()].fix(m_walk[m_walk.size() - 2], v); */
/* 	/1* m_canon.extend(v); *1/ */
/* 	/1* group.prune(v); *1/ */
/* 	m_walk.push_back(v); */
/* } */

/* template<typename EC> */
/* void Trace<EC>::backtrack() { */
/* 	assert(m_walk.size() > 2); */
/* 	Vertex v = m_walk[size() - 2]; */
/* 	Vertex from = m_walk[size() - 3]; */
/* 	Vertex to = m_walk.back(); */
/* 	m_rotation[v].backtrack(from, to); */
/* 	/1* group.backtrack(); *1/ */
/* 	/1* m_canon.backtrack(); *1/ */
/* 	m_walk.pop_back(); */
/* } */

} /* namespace kface */
