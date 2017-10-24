#ifndef DTRACE_ISO_H
#define DTRACE_ISO_H
#include <dtrace/graph/base.h>
namespace dtrace {
namespace morphism {

bool is_isomorphic(const Graph& g1, const Graph& g2);

struct iso_callback {
	iso_callback(bool& is_iso) : is_iso(is_iso) {
		is_iso = false;
	}

	template <typename map_lr, typename map_rl>
	bool operator () (map_lr, map_rl) {
		is_iso = true;
		return false;
	}
private:
	bool& is_iso;
};
	
} /* namespace morphism */
} /* namespace dtrace */
#endif /* ifndef DTRACE_ISO_H */
