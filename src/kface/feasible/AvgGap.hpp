#ifndef FEASIBLE_SUM_GAP
#define FEASIBLE_SUM_GAP
#include <kface/Graph.hpp>
#include <vector>

namespace kface {
namespace feasible {
		
	
template<typename GapRepr>
class AvgGap {
public:
	AvgGap(const Graph& g, const std::vector<Vertex>& walk): m_gap_repr(g, walk), m_sum_gap({0}) {
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
		size_t gap = std::max(m_gap_repr.max_gap(ve), m_sum_gap.back());
		m_sum_gap.push_back(gap);	
		m_gap_repr.fix(ve);
	}

	inline void backtrack() {
		m_sum_gap.pop_back();
		m_gap_repr.backtrack();
	}

	inline void bounds(size_t max_sum) {
		/* m_gap_repr.bounds(0, max_gap); */
		max_sum_ = max_sum;
	}

	inline size_t max_gap() const {
		return std::max(m_max_gap.back(), m_gap_repr.max_gap());
	}

private:
	GapRepr m_gap_repr;
	std::vector<size_t> m_sum_gap;
	size_t max_sum_ = 0;

};

} /* namespace feasible */
} /* namespace kface */
#endif /* ifndef FEASIBLE_MIN_GAP */
