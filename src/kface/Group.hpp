#ifndef KFACE_GROUP_HPP
#define KFACE_GROUP_HPP
#include <vector>
#include <stdlib.h>
#include <kface/Graph.hpp>
namespace kface {
	
class Group {
public:
	Group(const Graph& g);
	bool is_min(size_t v) const;
	const std::vector<std::vector<size_t>>& perms() const;
	void stabilize(size_t v);
	void backtrack();
	friend std::ostream& operator << (std::ostream& os, const Group& g);

private:
	/* data */
	std::vector<std::vector<size_t>> m_perms;
	std::vector<size_t> valid_perms, discard_perms, checkpoint;
};

} /* namespace kface */
#endif /* ifndef KFACE_GROUP_HPP */
