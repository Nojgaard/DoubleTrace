#ifndef KFACE_CANON_HPP
#define KFACE_CANON_HPP
#include <kface/Graph.hpp>
#include "Group.hpp"
#include "AutGroup.hpp"

namespace kface {

class Canon {
public:
	Canon(const AutGroup& g, const std::vector<size_t>& walk, size_t full_size);

	bool extensive_canon(size_t tk, const std::vector<size_t>& perm) const;
	bool is_canonical() const;
	void extend(Vertex v);
	void backtrack();

private:
	/* data */
	const AutGroup& m_group;
	const std::vector<size_t>& m_walk;
	std::vector<std::vector<size_t>> hist;
	size_t m_full_size;
};

} /* namespace kface */

#endif /* ifndef KFACE_CANON_HPP */
