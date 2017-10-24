#ifndef KFACE_PRUNE_NONE_HPP
#define KFACE_PRUNE_NONE_HPP
#include <kface/Graph.hpp>
#include "../PruneVector.hpp"

namespace kface {
namespace prune {
	
struct Dummy {};
class None {
public:
	None(const Graph& g, const std::vector<size_t>& walk);
	bool is_canonical(size_t v) const;
	bool is_canonical() const;
	void prune(size_t v);
	void backtrack();

private:
	/* data */
	const std::vector<size_t>& m_walk;
	std::vector<std::vector<size_t>> m_auts;
	/* PruneVector<size_t, Dummy> m_valid_auts; */
};

} /* namespace prune */
} /* namespace kface */
#endif /* ifndef KFACE_PRUNE_NONE_HPP */
