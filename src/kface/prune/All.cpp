#include "All.hpp"
#include <morphism/aut.h>

namespace kface {
namespace prune {
using namespace morphism;

	
All::All(const Graph& g, const std::vector<size_t>& walk): m_walk(walk) { }

bool All::is_canonical(size_t v) const { return true; }

bool All::is_canonical() const { return true; }

void All::prune(size_t v) { }
void All::backtrack() { }

} /* namespace prune */
} /* namespace kface */
