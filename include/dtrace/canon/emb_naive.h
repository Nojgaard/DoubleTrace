#ifndef DTRACE_CANON_EMB_NAIVE_H
#define DTRACE_CANON_EMB_NAIVE_H
#include <dtrace/graph/base.h>
#include <dtrace/defs.h>
#include <dtrace/canon/trace.h>
#include <boost/graph/subgraph.hpp>
#include <vector>

namespace dtrace {
namespace canon {

struct TCanonFace {
	TCanonFace(Face face, const Graph& g) : face(face)
	                                     , canon(canon::Trace(num_vertices(g), face)) {
		if (!canon.is_forward()) {
			std::reverse(face.begin(), face.end());
		}

		std::rotate(face.begin(), face.begin() + canon.get_shift(), face.end());
	}

	bool operator < (const TCanonFace& rhs) const {
		return canon < rhs.canon;
	}

	bool operator == (const TCanonFace& rhs) const {
		return canon == rhs.canon;
	}

	Face face;
	canon::Trace canon;

};

class EmbNaive {
public:
	EmbNaive(const std::vector<Face> &fs, const Graph &g);
	EmbNaive(const std::vector<Face> &fs, const Graph &g, bool use_debug);
	EmbNaive(const std::pair<Face, Face> &fs, const Graph &g);
	
	void print_gaps() const;
	void print_info() const;
	bool operator == (const EmbNaive &rhs) const;
	size_t hash() const;
private:
	bool check_parity(const EmbNaive& rhs) const;
	std::pair<bool, std::vector<Vertex>> make_bijection( const Face& lhs
			                                             , const Face& rhs) const;
	bool valid_bijection(const EmbNaive& rhs) const;
	canon::Trace init_canon();
	canon::Trace init_canon2();
	canon::Trace init_canon3();

	/* std::vector<canon::Trace> face_canons; */
	/* std::vector<Face> faces; */
	canon::Trace canon;
	std::vector<TCanonFace> canon_faces;
	const Graph& g;
};

} /* namespace canon */
} /* namespace dtrace */

namespace std {
	template<>
	struct hash<dtrace::canon::EmbNaive> {
		size_t operator () (const dtrace::canon::EmbNaive &c) const {
			return c.hash();
		}
	};
} /* namespace std */
#endif /* ifndef DTRACE_EMBEDDING_CANON_H */
