#ifndef DTRACE_BRUTEFORCE_FACE_TRACE_H
#define DTRACE_BRUTEFORCE_FACE_TRACE_H
#include <dtrace/graph/base.h>
#include <unordered_map>
#include <dtrace/canon/emb_brute.h>
#include <dtrace/canon/trace.h>
#include <unordered_set>
#include <dtrace/stats.h>

namespace dtrace {
namespace bruteforce {

using DirEdge = typename std::pair<Vertex, Vertex>;
using Face = typename std::vector<DirEdge>;
/* std::vector<std::vector<Face>> find_two_face_embeddings(const Graph &g); */
/* std::vector<std::vector<Face>> find_n_face_embeddings(const Graph &g, size_t num_faces); */
std::unordered_set<canon::EmbBrute> find_n_face_embeddings(const Graph &g, size_t num_faces, Stats& h);
std::unordered_set<canon::Trace> find_one_face_embeddings(const Graph &g, Stats& h);
void print_face_embedding(const std::vector<Face> &emb);
} /* namespace benchmark */
} /* namespace dtrace */
#endif /* ifndef DTRACE_BENCHMARK_FACE_TRACE_H */
