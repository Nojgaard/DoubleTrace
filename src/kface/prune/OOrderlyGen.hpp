#ifndef KFACE_PRUNE_ORDERLY_GEN_HPP
#define KFACE_PRUNE_ORDERLY_GEN_HPP
#include <vector>
#include "../PruneVector.hpp"
#include "../Util.hpp"
#include <kface/Graph.hpp>

namespace kface {
namespace prune {
	
class OOrderlyGen {
public:
	OOrderlyGen(const Graph& g, const std::vector<size_t>& m_walk);
	bool is_canonical(size_t v);
	bool is_canonical() const;
	void prune(size_t v);
	void backtrack();
	const std::vector<std::vector<size_t>>& perms() const;

private:
	struct PruneAuts { 
		PruneAuts(const OOrderlyGen& og): og(og) {}
		int operator () (size_t v, size_t i) const; 
		const OOrderlyGen& og;
	};
	struct PruneShifts { 
		PruneShifts(const OOrderlyGen& og): og(og) {}
		int operator () (size_t v, std::pair<size_t,size_t>& ik); 
		const OOrderlyGen& og;
	};
	struct PruneReverse { 
		PruneReverse(const OOrderlyGen& og): og(og) {}
		int operator () (size_t v, std::pair<size_t,size_t>& ik); 
		const OOrderlyGen& og;
	};

	const size_t& aut(size_t i, size_t v) const;
	bool is_min(size_t v) const;
	void prune_auts(size_t v);
	void prune_shifts(size_t v);
	void prune_revs(size_t v);
	bool is_canon_shift() const;
	bool is_canon_shift_partial(size_t v) const;
	bool is_canon_reverse() const;
	bool is_canon_reverse_partial(size_t v) const;
	void print_aut_walk(size_t i) const;
	/* data */
	const std::vector<size_t>& m_walk;
	size_t rn;
	/* std::vector<std::vector<size_t>> m_auts; */
	std::vector<size_t> m_auts;
	PruneVector<size_t, PruneAuts>  m_valid_auts;
	PruneVector<std::pair<size_t, size_t>, PruneShifts>  m_valid_shifts;
	PruneVector<std::pair<size_t, size_t>, PruneReverse>  m_valid_revs;
};

} /* namespace prune */
} /* namespace kface */

#endif /* ifndef KFACE_AUT_GROUP_HPP */
