#include <dtrace/canon/emb_brute.h>
#include <dtrace/util.h>

namespace dtrace {
namespace canon {

EmbBrute::EmbBrute( const std::vector<std::vector<Vertex>>& emb
		            , const std::vector<morphism::Perm>& auts) {

	for (auto& f : emb) {
		std::vector<Vertex> min_face = f;
		min_expr(min_face);
		canon.push_back(min_face);
	}
	std::sort(canon.begin(), canon.end());
	for (auto& aut : auts) {
		std::vector<std::vector<Vertex>> cand_canon;
		for (auto& f : emb) {
			cand_canon.push_back(min_face(f, aut));
		}
		std::sort(cand_canon.begin(), cand_canon.end());
		if (cand_canon < canon) {
			std::swap(canon, cand_canon);
		}
	}
}

EmbBrute::EmbBrute( const std::vector<Vertex>& f
		            , const std::vector<morphism::Perm>& auts) {
	std::vector<Vertex> mf = f;
	min_expr(mf);
	canon.push_back(mf);
	for (auto& aut : auts) {
		std::vector<std::vector<Vertex>> cand_canon;
		cand_canon.push_back(min_face(f, aut));
		std::sort(cand_canon.begin(), cand_canon.end());
		if (cand_canon < canon) {
			std::swap(canon, cand_canon);
		}
	}
}

std::vector<Vertex> EmbBrute::min_face( const std::vector<Vertex>& face,const morphism::Perm& aut) {
	/* std::vector<Vertex> min_trace = face; */
	/* min_expr(min_trace); */
	std::vector<Vertex> min_face = morphism::permute_trace(face, aut);
	min_expr(min_face);
	/* if (cand_trace < min_trace) { */
	/* 	min_trace = cand_trace; */
	/* } */
	return min_face;
}

void EmbBrute::min_expr(std::vector<Vertex>& trace) {
	std::vector<Vertex> revt(trace.rbegin(), trace.rend());
	std::rotate(trace.begin(), trace.begin() + min_expression(trace), trace.end());
	std::rotate(revt.begin(), revt.begin() + min_expression(revt), revt.end());
	if (revt < trace) {
		/* trace = revt; */
		std::swap(trace, revt);
	}
}

bool EmbBrute::operator == (const EmbBrute& rhs) const {
	return canon == rhs.canon;
}

size_t EmbBrute::hash() const {
	size_t seed = 0;
	for (auto& f : canon) {
		for (size_t v : f) {
			boost::hash_combine(seed, v);
		}
		boost::hash_combine(seed, "100");
	}
	return seed;
}

} /* namespace canon */
} /* namespace dtrace */
