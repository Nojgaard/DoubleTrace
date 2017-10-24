#ifndef DTRACE_EMBEDDING_H
#define DTRACE_EMBEDDING_H
#include <dtrace/graph/base.h>
#include <dtrace/canon/trace.h>
#include <unordered_map>

namespace dtrace {
void find_face_embeddings(Graph &g);
/* std::vector<std::vector<Vertex>> find_two_face_embeddings(const Graph &g); */
std::unordered_map<canon::Trace, std::pair<std::vector<Vertex>,std::vector<Vertex>>> find_two_face_embeddings(const Graph &org_g);
std::unordered_map<canon::Trace, std::vector<Vertex>> find_one_face_embeddings(Graph &g);
/* std::unordered_map<canon::Trace, std::vector<Vertex>> find_one_face_embeddings(Graph &g); */
} /* namespace dtrace */

#endif /* ifndef DTRACE_EMBEDDING_H */
