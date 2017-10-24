#include "LocalRotation.hpp"
#include <kface/Graph.hpp>

namespace kface {

LocalRotation::LocalRotation(Vertex v, const Graph& g)
	: m_tail(num_vertices(g), -1)
	, m_head(num_vertices(g), -1), m_fixed(num_vertices(g), false)
   , m_num_free(degree(v, g)) {
		for (size_t i = 0; i < num_vertices(g); ++i) {
			m_tail[i] = i; m_head[i] = i;
		}
	}

LocalRotation::LocalRotation(LocalRotation&& other) {
	m_next.swap(other.m_next);
	m_tail.swap(other.m_tail);
	m_head.swap(other.m_head);
	m_fixed.swap(other.m_fixed);
	std::swap(m_num_free, other.m_num_free);
}

bool LocalRotation::is_valid(Vertex from, Vertex to) const {
	return !m_fixed[to] && (m_num_free == 1 || m_head[to] != from);
}

void LocalRotation::fix(Vertex from, Vertex to) {
	assert(is_valid(from, to));
	assert(m_num_free > 0);

	m_fixed[to] = true;
	--m_num_free;
	m_head[m_tail[from]] = m_head[to];
	m_tail[m_head[to]] = m_tail[from];

}

void LocalRotation::backtrack(Vertex from, Vertex to) {
	++m_num_free;
	m_tail[m_head[to]] = to;
	m_head[m_tail[from]] = from;
	m_fixed[to] = false;
}
	
} /* namespace kface */
