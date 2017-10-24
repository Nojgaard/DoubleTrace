#ifndef DTRACE_CANON_EMB_BRUTE
#define DTRACE_CANON_EMB_BRUTE

#include <dtrace/graph/base.h>
#include <morphism/aut.h>

namespace dtrace {
namespace canon {

class EmbBrute {
public:
	EmbBrute(const std::vector<std::vector<Vertex>>& emb, const std::vector<morphism::Perm>& auts);
	EmbBrute(const std::vector<Vertex>& emb, const std::vector<morphism::Perm>& auts);
	size_t hash() const;
	bool operator == (const EmbBrute& rhs) const;
	friend std::ostream& operator << (std::ostream& os, const EmbBrute& other);
private:
	void min_expr(std::vector<Vertex>& trace);
std::vector<Vertex> min_face(const std::vector<Vertex>& face,const morphism::Perm& aut);

	std::vector<std::vector<Vertex>> canon;
};


} /* namespace canon */
} /* namespace dtrace */

namespace std {
	template<> struct hash<dtrace::canon::EmbBrute> {
		size_t operator () (const dtrace::canon::EmbBrute& canon) const {
			return canon.hash();
		}
	};
} /* namespace std */

#endif /* ifndef DTRACE_CANON_EMB_BRUTE */
