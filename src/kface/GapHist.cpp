#include <kface/GapHist.hpp>
#include <kface/Gap.hpp>
#include <iostream>
#include "Util.hpp"
#include <kface/Find.hpp>

namespace kface {

GapHist::GapHist(const Graph& g, GapRepr gr): m_g(g), m_gap_repr(gr),
	m_min_hist(num_edges(g) * 2, 0), m_max_hist(num_edges(g) * 2, 0),
	m_all_hist(num_edges(g) * 2, 0), m_sum(0), m_num_walks(0),
   m_sum_range(0)	{ }

void GapHist::add_walk(const std::vector<Vertex>& walk) {

	std::vector<unsigned int> gap;
	switch (m_gap_repr) {
		case GapRepr::None: gap = build_vertex_gap(m_g, walk, true); break;
		case GapRepr::Vertex: gap = build_vertex_gap(m_g, walk, true); break;
		case GapRepr::Edge: gap = build_edge_gap(m_g, walk, true); break;
		case GapRepr::Bio: gap = build_bio_gap(m_g, walk); break;
	}
  	/* = (m_use_egap) ? build_edge_gap(m_g, walk,true) */ 
																/* : build_vertex_gap(m_g,walk,true); */
	/* std::cout << "walk: " << walk << std::endl; */
	/* std::cout << "gap : " << gap << std::endl; */

	++m_num_walks;
	unsigned int min = num_edges(m_g) * 2, max = 1;
	for (auto v : gap) {
		m_sum += v;
		if (v < min) { min = v; }
		if (v > max) { max = v; }
		++m_all_hist[v-1];
	}
	m_sum_range += max - min + 1;
	++m_min_hist[min-1];
	++m_max_hist[max-1];
}

const std::vector<unsigned int>& GapHist::min_hist() const {
	return m_min_hist;
}

const std::vector<unsigned int>& GapHist::max_hist() const {
	return m_max_hist;
}

const std::vector<unsigned int>& GapHist::all_hist() const {
	return m_all_hist;
}

double GapHist::average() const {
	if (m_num_walks == 0) { return 0; }
	return m_sum / (double)m_num_walks / (num_edges(m_g) * 2);
}

double GapHist::average_range() const {
	if (m_num_walks == 0) { return 0; }
	return m_sum_range / (double)m_num_walks;
}

unsigned long GapHist::num_walks() const {
	return m_num_walks;
}
	
} /* namespace kface */
