#ifndef KFACE_PRUNE_KORDERLY_GEN_HPP
#define KFACE_PRUNE_KORDERLY_GEN_HPP

#include <kface/Graph.hpp>
#include <vector>
#include "../PruneVector.hpp"

namespace kface {
namespace prune {

class KOrderlyGen {
public:
	KOrderlyGen(const Graph& g, const std::vector<size_t>& walk, const std::vector<size_t>& fidx);
	bool is_canonical(size_t v);
	bool is_canonical() const;
	bool is_canonical_face();
	void prune(size_t v);
	void prune_face(size_t v, size_t w);
	void backtrack();
	void backtrack_face();

private:
	struct PruneAuts { 
		PruneAuts(const KOrderlyGen& kg): kg(kg) {}
		int operator () (size_t v, size_t i) const; 
		const KOrderlyGen& kg;
	};

	struct PruneData {
		PruneData(size_t i, size_t k, size_t f): i(i), k(k), f(f) {}
		size_t i, k, f;
	};

	struct PruneShifts {
		PruneShifts(const KOrderlyGen& kg): kg(kg) {}
		int operator () (size_t v, PruneData& d);
		int operator () (PruneData& d);
		const KOrderlyGen& kg;
	};

	const size_t& aut(size_t i, size_t v) const;
	size_t fsize(size_t f) const;

	void backtrack_shifts(size_t rewind, size_t vp);

	/* data */
	const std::vector<size_t>& m_walk;
	const std::vector<size_t>& m_fidx;
	std::vector<size_t> m_auts;

	PruneVector<size_t, PruneAuts>  m_valid_auts;
	PruneVector<PruneData, PruneShifts> m_valid_shifts;
};

} /* namespace prune */
} /* namespace kface */

#endif /* ifndef KFACE_PRUNE_KORDERLY_GEN_HPP */
