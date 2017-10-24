#include <dtrace/bruteforce/embedding.h>

namespace dtrace {
namespace bruteforce {
	
Embedding::Embedding(const Graph &g): g(g), embedding(num_vertices(g)), embedded(0) { }

void Embedding::embed_vertex(Vertex v, const std::vector<Vertex> v_embedding) {
	++embedded;
	embedding.at(v) = v_embedding;
}

void Embedding::unembed_vertex(Vertex v) {
	--embedded;
	embedding.at(v).clear();
}

bool Embedding::is_partial() const {
	return embedded < num_vertices(g);
}

const std::vector<Vertex> & Embedding::v_embedding(Vertex v) const {
	return embedding.at(v);
}

std::ostream & operator << (std::ostream &os, const Embedding &emb) {
	for (size_t i = 0; i < emb.embedding.size(); ++i) {
		os << i << ": ";
		for (Vertex v : emb.embedding[i]) {
			os << v << " ";
		}
		os << ", ";
	}
	return os;
}

} /* namespace benchamrk */
} /* namespace dtrace */
