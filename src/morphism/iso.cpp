#include <dtrace/graph/base.h>
#include <morphism/iso.h>
#include <boost/graph/vf2_sub_graph_iso.hpp>
#include <boost/graph/subgraph.hpp>
#include <iostream>

namespace dtrace {
namespace morphism {
	
bool always_true(Vertex, Vertex) {
	return true;
}
	
bool is_isomorphic( const Graph& g1, const Graph& g2) {
	//look at strong-traces for more details
	/* auto v_order_left = boost::vertex_order_by_mult(g1); */
		
	bool is_iso;
	iso_callback cb(is_iso);
	boost::vf2_graph_iso(g1, g2, cb);
	
	return is_iso;
}

/* bool is_isomorphic( const boost::subgraph<Graph>& g1 */
/* 						, const boost::subgraph<Graph>& g2) { */
/* 	//look at strong-traces for more details */
/* 	/1* auto v_order_left = boost::vertex_order_by_mult(g1); *1/ */
		
/* 	bool is_iso; */
/* 	iso_callback cb(is_iso); */
/* 	/1* boost::vf2_graph_iso(g1, g2, cb); *1/ */
/* 	return is_iso; */
/* } */

} /* namespace morphism */
} /* namespace dtrace */
