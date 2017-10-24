#ifndef KFACE_ROTATION_SYSTEM_HPP
#define KFACE_ROTATION_SYSTEM_HPP
#include <kface/Graph.hpp>
#include <vector>
#include <limits>

namespace kface { 
class RotationSystem {
public:
	RotationSystem(const Graph& g, const std::vector<Vertex>& walk);

	std::pair<Vertex, Vertex> get_min_unfixed() const;
	bool is_valid(Vertex v, Vertex from, Vertex to) const;
	bool is_valid(Vertex v) const;
	bool is_valid(Edge e) const;
	bool is_valid() const;
	void fix(Vertex v, Vertex from, Vertex to);
	void fix(Vertex v);
	void fix(Edge e);
	void backtrack(Vertex v, Vertex from, Vertex to);
	void backtrack();

private:
	size_t& tail(Vertex r, Vertex c);
	size_t& head(Vertex r, Vertex c);
	std::vector<bool>::reference fixed(Vertex r, Vertex c);
	size_t& num_free(Vertex v);

	const size_t& tail(Vertex r, Vertex c) const;
	const size_t& head(Vertex r, Vertex c) const;
	std::vector<bool>::const_reference fixed(Vertex r, Vertex c) const;
	const size_t& num_free(Vertex v) const;

	size_t size2d() const;
	size_t size1d() const;

	/* data */
	const Graph& m_g;
	const std::vector<Vertex>& m_walk;
	size_t row_size;
	std::vector<size_t> m_tail, m_head;
	std::vector<bool> m_fixed;
	std::vector<size_t> m_num_free;
};

RotationSystem::RotationSystem(const Graph& g
		, const std::vector<Vertex>& walk): m_g(g), m_walk(walk), row_size(num_vertices(g))
  		, m_tail(size2d()), m_head(size2d()), m_fixed(size2d(), false), m_num_free(size1d())	{
	  for (size_t i = 0; i < size1d(); ++i) {
		  num_free(i) = degree(i, m_g);
		  for (size_t j = 0; j < size1d(); ++j) {
			  tail(i, j) = j;
			  head(i, j) = j;
		  }
	  }
}

std::pair<Vertex,Vertex> RotationSystem::get_min_unfixed() const {
	/* size_t inf = std::numeric_limits<size_t>::max(); */
	/* auto min_e = std::make_pair(inf, inf); */
	for (size_t v = 0; v < num_vertices(m_g); ++v) {
		for (auto ia = adjacent_vertices(v, m_g); ia.first != ia.second; ++ia.first) {
			Vertex u = *ia.first;
			if (!fixed(v, u)) { return std::make_pair(v, u); }
		}
	}
	assert(false);
	return std::make_pair(-1, -1);
	/* return min_e; */
}

inline bool RotationSystem::is_valid(Vertex v, Vertex from, Vertex to) const {
	return !fixed(v, to) && (num_free(v) == 1 || head(v, to) != from);
}

inline bool RotationSystem::is_valid(Vertex to) const {
	Vertex v = m_walk.back();
	Vertex from = m_walk[m_walk.size() - 2];
	return is_valid(v, from, to);
}

inline bool RotationSystem::is_valid(Edge e) const {
	return is_valid(target(e, m_g));
}

inline bool RotationSystem::is_valid() const { return true; }

inline void RotationSystem::fix(Vertex v, Vertex from, Vertex to) {
	assert(is_valid(v, from, to));
	assert(num_free(v) > 0);

	fixed(v, to) = true;
	--num_free(v);
	head(v, tail(v, from)) = head(v, to);
	tail(v, head(v, to)) = tail(v, from);
}

inline void RotationSystem::fix(Edge e) {
	fix(target(e, m_g));
}

inline void RotationSystem::fix(Vertex to) {
	Vertex v = m_walk.back();
	Vertex from = m_walk[m_walk.size() - 2];
	fix(v, from, to);
}

inline void RotationSystem::backtrack(Vertex v, Vertex from, Vertex to) {
	++num_free(v);
	tail(v, head(v, to)) = to;
	head(v, tail(v, from)) = from;
	fixed(v, to) = false;
}

inline void RotationSystem::backtrack() {
	Vertex v = m_walk[m_walk.size() - 2];
	Vertex from = m_walk[m_walk.size() - 3];
	Vertex to = m_walk.back();
	backtrack(v, from, to);
}

inline size_t& RotationSystem::tail(Vertex r, Vertex c) { return m_tail[r * row_size + c]; }
inline size_t& RotationSystem::head(Vertex r, Vertex c) { return m_head[r * row_size + c]; }
inline size_t& RotationSystem::num_free(Vertex v) { return m_num_free[v]; }
inline std::vector<bool>::reference RotationSystem::fixed(Vertex r, Vertex c) { 
	return m_fixed[r * row_size + c]; 
}

inline const size_t& RotationSystem::tail(Vertex r, Vertex c) const { return m_tail[r * row_size + c]; }
inline const size_t& RotationSystem::head(Vertex r, Vertex c) const { return m_head[r * row_size + c]; }
inline const size_t& RotationSystem::num_free(Vertex v) const { return m_num_free[v]; }
inline std::vector<bool>::const_reference RotationSystem::fixed(Vertex r, Vertex c) const { 
	return m_fixed[r * row_size + c]; 
}

inline size_t RotationSystem::size2d() const { return row_size * row_size; }
inline size_t RotationSystem::size1d() const { return row_size; }


} /* namespace kface */

#endif /* ifndef KFACE_ROTATION_SYSTEM_HPP */
