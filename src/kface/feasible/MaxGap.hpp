#ifndef FEASIBLE_MAX_GAP
#define FEASIBLE_MAX_GAP
#include <kface/Graph.hpp>
#include <vector>

namespace kface {
namespace feasible {
		
	
template<typename GapRepr>
class MaxGap {
public:
	MaxGap(const Graph& g, const std::vector<Vertex>& walk): m_gap_repr(g, walk), m_max_gap({0}) {
		m_gap_repr.bounds(0, num_edges(g) * 2 + 1);
	}

	template<typename VE>
	inline bool is_valid(VE ve) const {
		return m_gap_repr.is_valid(ve);
	}

	inline bool is_valid() const {
		return m_gap_repr.is_valid();
	}

	template<typename VE>
	inline void fix(VE ve) {
		size_t gap = std::max(m_gap_repr.max_gap(ve), m_max_gap.back());
		m_max_gap.push_back(gap);	
		m_gap_repr.fix(ve);
	}

	inline void backtrack() {
		m_max_gap.pop_back();
		m_gap_repr.backtrack();
	}

	inline void bounds(size_t min_gap, size_t max_gap) {
		m_gap_repr.bounds(0, max_gap);
	}

	inline size_t max_gap() const {
		return std::max(m_max_gap.back(), m_gap_repr.max_gap());
	}

private:
	GapRepr m_gap_repr;
	std::vector<size_t> m_max_gap;

};

} /* namespace feasible */
} /* namespace kface */
#endif /* ifndef FEASIBLE_MIN_GAP */
