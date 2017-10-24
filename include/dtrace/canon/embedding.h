#ifndef DTRACE_CANON_EMBEDDING_H
#define DTRACE_CANON_EMBEDDING_H
#include <dtrace/graph/base.h>
#include <dtrace/defs.h>
#include <dtrace/canon/trace.h>
#include <vector>

namespace dtrace {
namespace canon {

class Embedding {
public:
	Embedding(const std::vector<Face> &fs, const Graph &g);
	
	void print_gaps() const;
	void print_info() const;
	bool operator == (const Embedding &rhs) const;
	size_t hash() const;
private:
	std::vector<Vertex> construct_ftrace(const Face& f1, const Face& f2, size_t s1, size_t s2);
	std::vector<Vertex> construct_btrace(const Face& f1, const Face& f2, size_t s1, size_t s2);
	canon::Trace compute_canon(const std::vector<Face>& fs);
	canon::Trace construct_canon(const Face& fixed_f1, const Face& fixed_f2);

	/* std::vector<trace::Canon> face_canons; */
	/* std::vector<Face> faces; */
	canon::Trace canon;
	const Graph& g;
};

} /* namespace embedding */
} /* namespace dtrace */

namespace std {
	template<>
	struct hash<dtrace::canon::Embedding> {
		size_t operator () (const dtrace::canon::Embedding &c) const {
			return c.hash();
		}
	};
} /* namespace std */
#endif /* ifndef DTRACE_EMBEDDING_CANON_H */
