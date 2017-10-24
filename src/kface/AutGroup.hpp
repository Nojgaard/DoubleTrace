#ifndef KFACE_AUT_GROUP_HPP
#define KFACE_AUT_GROUP_HPP
#include <vector>
#include "PruneVector.hpp"
#include "Util.hpp"
#include <kface/Graph.hpp>

namespace kface {

struct PruneAuts {
	PruneAuts(const std::vector<std::vector<size_t>>& a);
	bool operator () (size_t v, size_t i);
private:
	const std::vector<std::vector<size_t>>& m_auts;
};

struct PruneShifts {
	PruneShifts(const std::vector<std::vector<size_t>>& a,const std::vector<size_t>& w);
	bool operator () (size_t v, std::pair<size_t, size_t>& ik);

private:
	const std::vector<std::vector<size_t>>& m_auts;
	const std::vector<size_t>& m_walk;
};

struct PruneReverse {
	PruneReverse(const std::vector<std::vector<size_t>>& a,const std::vector<size_t>& w);
	bool operator () (size_t v, std::pair<size_t, size_t>& ik);

private:
	const std::vector<std::vector<size_t>>& m_auts;
	const std::vector<size_t>& m_walk;
};
	
class AutGroup {
public:
	AutGroup(const Graph& g, const std::vector<size_t>& m_walk);
	bool is_min(size_t v) const;
	bool is_canonical(size_t v) const;
	bool is_canonical() const;
	void prune(size_t v);
	void backtrack();
	const std::vector<std::vector<size_t>>& perms() const;

private:
	void prune_auts(size_t v);
	void prune_shifts(size_t v);
	void prune_revs(size_t v);
	bool is_canon_shift() const;
	bool is_canon_shift_partial(size_t v) const;
	bool is_canon_reverse() const;
	bool is_canon_reverse_partial(size_t v) const;
	/* data */
	const std::vector<size_t>& m_walk;
	std::vector<std::vector<size_t>> m_auts;
	PruneVector<size_t, PruneAuts>  m_valid_auts;
	PruneVector<std::pair<size_t, size_t>, PruneShifts>  m_valid_shifts;
	PruneVector<std::pair<size_t, size_t>, PruneReverse>  m_valid_revs;
};

} /* namespace kface */

#endif /* ifndef KFACE_AUT_GROUP_HPP */
