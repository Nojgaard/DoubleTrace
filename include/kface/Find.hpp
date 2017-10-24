#ifndef KFACE_FIND_HPP
#define KFACE_FIND_HPP
#include <kface/Graph.hpp>
#include <vector>
#include <kface/GapHist.hpp>

namespace kface {
enum Algorithm { Brute, Orderly, Gap, All};
enum class GapRepr { None, Bio, Edge, Vertex };

inline std::ostream& operator << (std::ostream& os, const GapRepr& gr) {
	switch(gr) {
		case GapRepr::None: os << "NONE"; break;
		case GapRepr::Bio: os << "BIO"; break;
		case GapRepr::Edge: os << "EDGE"; break;
		case GapRepr::Vertex: os << "VERTEX"; break;
	}
	return os;
}

inline std::ostream& operator << (std::ostream& os, const Algorithm alg) {
	switch(alg) {
		case Brute: os << "BRUTE"; break;
		case Orderly: os << "ALGEBRAIC"; break;
		case Gap: os << "GAP"; break;
		case All: os << "ALL EMBEDDINGS"; break;
	}
	return os;
}

/* void find_one_face(const Graph& g); */
unsigned long count_one_face(const Graph& g);
unsigned long count_one_face(const Graph& g, Algorithm alg);
unsigned long count_one_face(const Graph& g, Algorithm alg, size_t min_gap, size_t max_gap);
std::vector<std::vector<Vertex>> find_one_face(const Graph& g, Algorithm alg);
std::vector<std::vector<Vertex>> find_one_face(const Graph& g, Algorithm alg, size_t min_gap, size_t max_gap, GapRepr gap_repr);
/* int count_kface_embeddings(int k, const Graph& g); */
std::pair<unsigned long, bool> count_one_face(const Graph& g, int timeout);
std::pair<unsigned long, bool> count_one_face(const Graph& g, int timeout, Algorithm alg);
std::pair<unsigned long, bool> count_one_face(const Graph& g, int timeout,
		Algorithm alg, size_t min_gap, size_t max_gap, GapRepr gap_repr);

std::pair<GapHist,bool> find_gap_hist(const Graph& g, Algorithm alg, GapRepr gap_repr, int timeout);
std::tuple<unsigned long, size_t, bool> count_max_embeddings(const Graph& g, Algorithm
		alg, int timeout, GapRepr gr);
std::tuple<unsigned long, unsigned long, bool> count_one_face_and_misses(const Graph& g, 
		Algorithm alg, int timeout);

} /* namespace kface */
#endif /* ifndef KFACE_FIND_HPP */
