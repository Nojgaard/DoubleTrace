#ifndef KFACE_GAP_HIST
#define KFACE_GAP_HIST
#include <vector>
#include <kface/Graph.hpp>

namespace kface {

enum class GapRepr;

class GapHist {
public:
	GapHist(const Graph& g, GapRepr gr);
	void add_walk(const std::vector<Vertex>& walk);

	const std::vector<unsigned int>& min_hist() const;
	const std::vector<unsigned int>& max_hist() const;
	const std::vector<unsigned int>& all_hist() const;
	double average() const;
	double average_range() const;
	unsigned long num_walks() const;

private:
	const Graph& m_g;
	/* bool m_use_egap; */
	GapRepr m_gap_repr;
	std::vector<unsigned int> m_min_hist, m_max_hist, m_all_hist;
	unsigned long m_sum, m_num_walks, m_sum_range;
};

} /* namespace kface */
#endif /* ifndef KFACE_GAP_HIST */
