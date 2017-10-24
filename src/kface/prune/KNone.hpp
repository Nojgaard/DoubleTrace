#ifndef KFACE_KNONE_HPP
#define KFACE_KNONE_HPP

#include <kface/Graph.hpp>
#include <vector>

namespace kface {
namespace prune {

class KNone {
public:
	KNone(const Graph& g, const std::vector<size_t>& walk, const std::vector<size_t>& fidx);
	bool is_canonical(size_t v) const;
	bool is_canonical() const;
	void prune(size_t v);
	void backtrack();

private:
	bool is_canonical_auts(const std::vector<size_t>& aut) const;
	bool is_canonical_shifts(const std::vector<size_t>& aut) const;
	bool is_canonical_revs(const std::vector<size_t>& aut) const;

	size_t find_min_shift(const std::vector<size_t>& aut, size_t fidx) const;
	size_t find_min_rev(const std::vector<size_t>& aut, size_t fidx) const;
	/* data */
	const std::vector<size_t>& m_walk;
	const std::vector<size_t>& m_fidx;
	const std::vector<std::vector<size_t>> m_auts;
};
	
} /* namespace prune */
} /* namespace kface */

#endif /* ifndef KFACE_KNONE_HPP */
