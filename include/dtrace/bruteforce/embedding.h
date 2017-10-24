#ifndef DTRACE_BRUTEFORCE_EMBEDDING_H
#define DTRACE_BRUTEFORCE_EMBEDDING_H
#include <dtrace/graph/base.h>

namespace dtrace {
namespace bruteforce {

class Embedding {
public:
	Embedding(const Graph &g);
	void embed_vertex(Vertex v, const std::vector<Vertex> v_embedding);
	void unembed_vertex(Vertex v);
	bool is_partial() const;
	const std::vector<Vertex> & v_embedding(Vertex v) const;
	friend std::ostream & operator << (std::ostream &os, const Embedding &emb);
private:
	const Graph &g;
	std::vector<std::vector<Vertex>> embedding;
	size_t embedded;
};

} /* namespace benchmark */
} /* namespace dtrace */

#endif /* ifndef DTRACE_BRUTEFORCE_EMBEDDING_H */
