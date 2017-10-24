#ifndef KFACE_LOCAL_ROTATION_HPP
#define KFACE_LOCAL_ROTATION_HPP

#include <kface/Graph.hpp>
#include "SparseSet.hpp"
#include <vector>

namespace kface {

class LocalRotation {
public:
	using iterator = SparseSet::iterator;
	using const_iterator = SparseSet::const_iterator;
	LocalRotation(Vertex v, const Graph& g);
	LocalRotation(LocalRotation&& other);

	bool is_valid(Vertex from, Vertex to) const;
	void fix(Vertex from, Vertex to);
	void backtrack(Vertex from, Vertex to);

private:
	/* data */
	std::vector<int> m_next;
	std::vector<size_t> m_tail, m_head;
	std::vector<bool> m_fixed;
	size_t m_num_free;
};

} /* namespace kface */

#endif /* ifndef KFACE_LOCAL_ROTATION_HPP */
