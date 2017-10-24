#include "RotationSystem.hpp"

namespace kface {

/* RotationSystem::RotationSystem(const Graph& g): m_g(g), row_size(num_vertices(g)) */
/*   , m_tail(size2d()), m_head(size2d()), m_fixed(size2d(), false), m_num_free(size1d())	{ */
/* 	  for (size_t i = 0; i < size1d(); ++i) { */
/* 		  num_free(i) = degree(i, m_g); */
/* 		  for (size_t j = 0; j < size1d(); ++j) { */
/* 			  tail(i, j) = j; */
/* 			  head(i, j) = j; */
/* 		  } */
/* 	  } */
/* } */

/* bool RotationSystem::is_valid(Vertex v, Vertex from, Vertex to) const { */
/* 	return !fixed(v, to) && (num_free(v) == 1 || head(v, to) != from); */
/* } */

/* void RotationSystem::fix(Vertex v, Vertex from, Vertex to) { */
/* 	assert(is_valid(v, from, to)); */
/* 	assert(num_free(v) > 0); */

/* 	fixed(v, to) = true; */
/* 	--num_free(v); */
/* 	head(v, tail(v, from)) = head(v, to); */
/* 	tail(v, head(v, to)) = tail(v, from); */
/* } */

/* void RotationSystem::backtrack(Vertex v, Vertex from, Vertex to) { */
/* 	++num_free(v); */
/* 	tail(v, head(v, to)) = to; */
/* 	head(v, tail(v, from)) = from; */
/* 	fixed(v, to) = false; */
/* } */

/* inline size_t& RotationSystem::tail(Vertex r, Vertex c) { return m_tail[r * row_size + c]; } */
/* inline size_t& RotationSystem::head(Vertex r, Vertex c) { return m_head[r * row_size + c]; } */
/* inline size_t& RotationSystem::num_free(Vertex v) { return m_num_free[v]; } */
/* inline std::vector<bool>::reference RotationSystem::fixed(Vertex r, Vertex c) { */ 
/* 	return m_fixed[r * row_size + c]; */ 
/* } */

/* inline const size_t& RotationSystem::tail(Vertex r, Vertex c) const { return m_tail[r * row_size + c]; } */
/* inline const size_t& RotationSystem::head(Vertex r, Vertex c) const { return m_head[r * row_size + c]; } */
/* inline const size_t& RotationSystem::num_free(Vertex v) const { return m_num_free[v]; } */
/* inline std::vector<bool>::const_reference RotationSystem::fixed(Vertex r, Vertex c) const { */ 
/* 	return m_fixed[r * row_size + c]; */ 
/* } */

/* inline size_t RotationSystem::size2d() const { return row_size * row_size; } */
/* inline size_t RotationSystem::size1d() const { return row_size; } */

} /* namespace kface */
