#ifndef KFACE_GAPNEW_HPP
#define KFACE_GAPNEW_HPP

#include <vector>
#include <kface/Graph.hpp>
namespace kface {
	
std::vector<unsigned int> build_vertex_gap(const Graph& g,const std::vector<Vertex>& walk, bool forward);
std::vector<unsigned int> build_edge_gap(const Graph& g,const std::vector<Vertex>& walk, bool forward);
std::vector<unsigned int> build_bio_gap(const Graph& g,const std::vector<Vertex>& walk);

} /* namespace kface */
#endif /* ifndef KFACE_GAP_HPP */
