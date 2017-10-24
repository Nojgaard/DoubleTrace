#ifndef KFACE_GAP_HPP
#define KFACE_GAP_HPP
#include <kface/Graph.hpp>
#include <unordered_set>
#include "../PruneVector.hpp"

namespace kface {
namespace prune {

struct GapHash {
	size_t operator () (const std::vector<size_t>& vs) const {
		size_t seed = 0;
		for (size_t v : vs) { boost::hash_combine(seed, v); }
		return seed;
	}
};

class Gap {
public:
	Gap(const Graph& g, const std::vector<size_t>& m_walk);
	bool is_canonical(size_t v);
	bool is_canonical();
	void prune(size_t v);
	void backtrack();
	const std::vector<std::vector<size_t>>& perms() const;
private:
	struct PruneAuts { 
		PruneAuts(const Gap& og): og(og) {}
		int operator () (size_t v, size_t i) const; 
		const Gap& og;
	};

	const size_t& aut(size_t i, size_t v) const;
	void prune_auts(size_t v);
	std::vector<size_t> build_gap(bool forward) const;
	std::vector<size_t> build_canon_gap(bool forward) const;
	std::vector<size_t> build_canon_gap() const;
	size_t find_min_rotation(const std::vector<size_t>& gap) const;

	const Graph& m_g;
	const std::vector<size_t>& m_walk;
	std::unordered_set<std::vector<size_t>, GapHash> gaps;
	std::vector<size_t> m_auts;
	PruneVector<size_t, PruneAuts>  m_valid_auts;
};
	
} /* prune */ 
} /* kface */ 

#endif /* ifndef KFACE_GAP_HPP */
