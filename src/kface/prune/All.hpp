#ifndef KFACE_PRUNE_ALL_HPP
#define KFACE_PRUNE_ALL_HPP
#include <kface/Graph.hpp>

namespace kface {
namespace prune {
	
class All {
public:
	All(const Graph& g, const std::vector<size_t>& walk);
	bool is_canonical(size_t v) const;
	bool is_canonical() const;
	void prune(size_t v);
	void backtrack();

private:
	/* data */
	const std::vector<size_t>& m_walk;
};

} /* namespace prune */
} /* namespace kface */
#endif /* ifndef KFACE_PRUNE_NONE_HPP */
