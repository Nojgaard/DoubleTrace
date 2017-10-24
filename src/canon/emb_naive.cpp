#include <dtrace/canon/emb_naive.h>
#include <morphism/iso.h>
#include <dtrace/util.h>

namespace dtrace {
namespace canon {

/* std::vector<std::vector<Vertex>> find_vertex_gap(const std::vector<Vertex> &t) { */
/* } */

Graph create_subgraph(const std::vector<Vertex>& vs, const Graph& g) {
	Graph sg;
	std::vector<int> idx_map(num_vertices(g), -1);

	for (Vertex v : vs) {
		if (idx_map.at(v) == -1) {
			idx_map.at(v) = add_vertex(sg);
		}
	}
	for (auto ei = edges(g); ei.first != ei.second; ++ei.first) {
		const Edge& e = *ei.first;
		Vertex src = source(e, g), tar = target(e, g);
		if (idx_map.at(src) != -1 && idx_map.at(tar) != -1) {
			add_edge(idx_map.at(src), idx_map.at(tar), sg);
		}
	}
	return sg;
}


EmbNaive::EmbNaive(const std::pair<Face, Face> &fs, const Graph &g) : g(g) {
	/* this->g = g; */
	/* boost::subgraph<Graph> hg = g; */
	/* face_canons.push_back(canon::Trace(num_vertices(g), fs.first)); */
	/* face_canons.push_back(canon::Trace(num_vertices(g), fs.second)); */
	/* std::sort(face_canons.begin(), face_canons.end()); */

	/* faces.push_back(fs.first); */
	/* faces.push_back(fs.second); */
	canon_faces.push_back(TCanonFace(fs.first, g));
	canon_faces.push_back(TCanonFace(fs.second, g));
	std::sort(canon_faces.begin(), canon_faces.end());
	
	init_canon();
}

EmbNaive::EmbNaive(const std::vector<Face> &fs, const Graph &g) : g(g) {
	/* this->g = g; */
	for (const Face &f : fs) {
		/* face_canons.push_back(canon::Trace(num_vertices(g), f)); */
		canon_faces.push_back(TCanonFace(f, g));
	}
	std::sort(canon_faces.begin(), canon_faces.end());
	/* faces = fs; */
	canon = init_canon();
}

EmbNaive::EmbNaive(const std::vector<Face> &fs, const Graph &g, bool use_debug) : g(g) {
	/* this->g = g; */
	for (const Face &f : fs) {
		/* face_canons.push_back(canon::Trace(num_vertices(g), f)); */
		canon_faces.push_back(TCanonFace(f, g));
	}
	std::sort(canon_faces.begin(), canon_faces.end());
	/* faces = fs; */
	canon = init_canon3();
}

canon::Trace get_canon(const Face& fixed_f1, const Face& fixed_f2, const Graph& g) {
	std::vector<Vertex> glued;
	glued.push_back(100);
	glued.insert(glued.end(), fixed_f1.begin(), fixed_f1.end());
	glued.push_back(100);
	glued.insert(glued.end(), fixed_f2.begin(), fixed_f2.end());
	return canon::Trace(num_vertices(g), glued);
}

canon::Trace get_canon(const Face& fixed_face, canon::Trace& c, Face f, const Graph& g) {
	canon::Trace min_canon;
	if (!c.is_repeating() && !c.is_palindrome()) {
		if (c.is_forward()) {
			std::rotate(f.begin(), f.begin() + c.get_shift(), f.end());
		} else {
			std::rotate(f.rbegin(), f.rbegin() + c.get_shift(), f.rend());
			std::reverse(f.begin(), f.end());
		}
		std::cout << fixed_face << "+ " << f << std::endl;
		return get_canon(fixed_face, f, g);
	} else if (!c.is_repeating()) {
		Face rf = f;
		std::rotate(rf.rbegin(), rf.rbegin() + c.get_pshift(), rf.rend());
		std::reverse(rf.begin(), rf.end());
		std::rotate(f.begin(), f.begin() + c.get_shift(), f.end());
		return std::min(get_canon(fixed_face, f, g), get_canon(fixed_face, rf, g));
	} else {
		Face rf = f;
		std::reverse(rf.begin(), rf.end());
		for (size_t i = 0; i < f.size(); ++i) {
			if (i > 0) {
				std::rotate(f.begin(), f.begin() + 1, f.end());
				std::rotate(rf.begin(), rf.begin() + 1, rf.end());
			}
			canon::Trace cand = get_canon(fixed_face, f, g);
			if ((i == 0) || cand < min_canon) {
				min_canon = cand;
			}
			cand = get_canon(fixed_face, rf, g);
			if (cand < min_canon) {
				min_canon = cand;
			}
		}
	}
	return min_canon;
}

canon::Trace EmbNaive::init_canon2() {
	canon::Trace min_canon;
	Face& f1 = canon_faces[0].face;
	canon::Trace& c1 = canon_faces[0].canon;
	Face& f2 = canon_faces[1].face;
	canon::Trace& c2 = canon_faces[1].canon;
	if (!c1.is_palindrome() && !c1.is_repeating()) {
		if (!canon_faces[0].canon.is_repeating()) {
			if (canon_faces[0].canon.is_forward()) {
				std::rotate(f1.begin(), f1.begin() + c1.get_shift(), f1.end());
			} else {
				std::rotate(f1.rbegin(), f1.rbegin() + c1.get_shift(), f1.rend());
				std::reverse(f1.begin(), f1.end());
			}
		}
		return get_canon(f1, c2, f2, g);
	} else if (!c1.is_repeating()) {
		Face rf = f1;
		std::rotate(rf.rbegin(), rf.rbegin() + c1.get_pshift(), rf.rend());
		std::reverse(rf.begin(), rf.end());
		std::rotate(f1.begin(), f1.begin() + c1.get_shift(), f1.end());
		return std::min(get_canon(f1, c2, f2, g), get_canon(rf, c2, f2, g));
	} else {
		Face rf = f1;
		std::reverse(rf.begin(), rf.end());
		for (size_t i = 0; i < f1.size(); ++i) {
			if (i > 0) {
				std::rotate(f1.begin(), f1.begin() + 1, f1.end());
				std::rotate(rf.begin(), rf.begin() + 1, rf.end());
			}
			canon::Trace cand = get_canon(f1, c2, f2, g);
			if ((i == 0) || cand < min_canon) {
				min_canon = cand;
			}
			cand = get_canon(rf, c2, f2, g);
			if (cand < min_canon) {
				min_canon = cand;
			}
		}
	}
	/* Face& f2 = canon_faces[1].face; */
	/* canon::Trace& c2 = canon_faces[1].canon; */
	/* if (!canon_faces[1].canon.is_repeating()) { */
	/* 	if (canon_faces[1].canon.is_forward()) { */
	/* 		std::rotate(f2.begin(), f2.begin() + c2.get_shift(), f2.end()); */
	/* 	} else { */
	/* 		std::rotate(f2.rbegin(), f2.rbegin() + c2.get_shift(), f2.rend()); */
	/* 	} */
	/* } */
	/* for (size_t i = 0; i < f1.size(); ++i) { */
	/* 	if (i > 0 && !c1.is_repeating()) { break; } */
	/* 	else if (i > 0) { */
	/* 		std::rotate(f1.begin(), f1.begin() + 1, f1.end()); */
	/* 	} */
	/* 	for (size_t j = 0; j < f2.size(); ++j) { */
	/* 		if (j > 0 && !c2.is_repeating()) { break; } */
	/* 		else if (j > 0) { */
	/* 			std::rotate(f2.begin(), f2.begin() + 1, f2.end()); */
	/* 		} */
	/* 		std::vector<Vertex> glued; */
	/* 		glued.push_back(100); */
	/* 		glued.insert(glued.end(), f1.begin(), f1.end()); */
	/* 		glued.push_back(100); */
	/* 		glued.insert(glued.end(), f2.begin(), f2.end()); */
	/* 		canon::Trace cand(num_vertices(g), glued); */
	/* 		if ((i == 0 && j == 0) || cand < min_canon) { */
	/* 			min_canon = cand; */
	/* 		} */

	/* 		if (!c2.is_palindrome()) { continue; } */
	/* 		glued.push_back(100); */
	/* 		glued.insert(glued.end(), f1.begin(), f1.end()); */
	/* 		glued.push_back(100); */
	/* 		glued.insert(glued.end(), f2.rbegin(), f2.rend()); */
	/* 		canon::Trace rcand(num_vertices(g), glued); */
	/* 		if ((i == 0 && j == 0) || rcand < min_canon) { */
	/* 			min_canon = rcand; */
	/* 		} */
	/* 	} */
	/* 	if (!c1.is_palindrome()) { continue; } */
	/* 	for (size_t j = 0; j < f2.size(); ++j) { */
	/* 		if (j > 0 && !c2.is_repeating()) { break; } */
	/* 		else if (j > 0) { */
	/* 			std::rotate(f2.begin(), f2.begin() + 1, f2.end()); */
	/* 		} */
	/* 		std::vector<Vertex> glued; */
	/* 		glued.push_back(100); */
	/* 		glued.insert(glued.end(), f1.rbegin(), f1.rend()); */
	/* 		glued.push_back(100); */
	/* 		glued.insert(glued.end(), f2.begin(), f2.end()); */
	/* 		canon::Trace cand(num_vertices(g), glued); */
	/* 		if ((i == 0 && j == 0) || cand < min_canon) { */
	/* 			min_canon = cand; */
	/* 		} */

	/* 		if (!c2.is_palindrome()) { continue; } */
	/* 		glued.push_back(100); */
	/* 		glued.insert(glued.end(), f1.rbegin(), f1.rend()); */
	/* 		glued.push_back(100); */
	/* 		glued.insert(glued.end(), f2.rbegin(), f2.rend()); */
	/* 		canon::Trace rcand(num_vertices(g), glued); */
	/* 		if ((i == 0 && j == 0) || rcand < min_canon) { */
	/* 			min_canon = rcand; */
	/* 		} */
	/* 	} */
	/* } */
	return min_canon;
}

canon::Trace EmbNaive::init_canon3() {
	canon::Trace min_canon;
	std::vector<Edge> ef1 = vertex_to_edge(g, canon_faces[0].face);
	std::vector<Edge> ef2 = vertex_to_edge(g, canon_faces[1].face);

	std::set<Edge> border_edges;
	std::vector<int> num_visits(num_edges(g), 0);
	std::vector<std::pair<size_t, size_t>> border_idx(num_edges(g));
	for (size_t i = 0; i < ef2.size(); ++i) {
		Edge e = ef2[i];
		++num_visits[g[e].idx];
		if (num_visits[g[e].idx] == 1) {
			border_edges.insert(e);
			border_idx[g[e].idx].second = i;
		} else {
			border_edges.erase(e);
		}
	}
	for (size_t i = 0; i < ef1.size(); ++i) {
		Edge e = ef1[i];
		if (border_edges.find(e) != border_edges.end()) {
			border_idx[g[e].idx].first = i;
		}
	}
	bool has_min = false;
	for (Edge e : border_edges) {
		Face f1 = canon_faces[0].face;
		Face f2 = canon_faces[1].face;
		int sf1 = border_idx[g[e].idx].first;
		int sf2 = border_idx[g[e].idx].second;
		std::rotate(f1.begin(), f1.begin() + sf1, f1.end());
		std::rotate(f2.begin(), f2.begin() + sf2, f2.end());
		canon::Trace cand = get_canon(f1, f2, g);
		if (!has_min || cand < min_canon) {
			min_canon = cand;
			has_min = true;
		}
		std::reverse(f1.begin(), f1.end());
		std::reverse(f2.begin(), f2.end());
		std::rotate(f1.begin(), f1.begin() + (f1.size()-2), f1.end());
		std::rotate(f2.begin(), f2.begin() + (f2.size()-2), f2.end());
		cand = get_canon(f1, f2, g);
		if (cand < min_canon) {
			min_canon = cand;
		}
	}
	return min_canon;
}

canon::Trace EmbNaive::init_canon() {
	canon::Trace min_canon;
	for (size_t i = 0; i < canon_faces[0].face.size(); ++i) {
		Face f1 = canon_faces[0].face;
		std::rotate(f1.begin(), f1.begin() + i, f1.end());
		for (size_t j = 0; j < canon_faces[1].face.size(); ++j) {
			Face f2 = canon_faces[1].face;
			std::rotate(f2.begin(), f2.begin() + j, f2.end());
			std::vector<Vertex> glued;
			glued.push_back(100);
			glued.insert(glued.end(), f1.begin(), f1.end());
			glued.push_back(100);
			glued.insert(glued.end(), f2.begin(), f2.end());
			canon::Trace cand(num_vertices(g), glued);
			if ((i == 0 && j == 0) || cand < min_canon) {
				min_canon = cand;
			}
		}
		std::reverse(f1.begin(), f1.end());
		for (size_t j = 0; j < canon_faces[1].face.size(); ++j) {
			Face f2 = canon_faces[1].face;
			std::rotate(f2.begin(), f2.begin() + j, f2.end());
			std::reverse(f2.begin(), f2.end());
			std::vector<Vertex> glued;
			glued.push_back(100);
			glued.insert(glued.end(), f1.begin(), f1.end());
			glued.push_back(100);
			glued.insert(glued.end(), f2.begin(), f2.end());
			canon::Trace cand(num_vertices(g), glued);
			if (cand < min_canon) {
				min_canon = cand;
			}
		}
	}
	return min_canon;
}

bool EmbNaive::operator == (const EmbNaive &rhs) const {
	return (canon == rhs.canon);// && canon_faces == rhs.canon_faces;
	/* return face_canons == rhs.face_canons; */
	if (canon_faces != rhs.canon_faces) {
		return false;
	}
	/* assert(!(canon_faces[0].canon == canon_faces[1].canon)); */
	return canon == rhs.canon;

	/* if (!check_parity(rhs)) { return false; } */
	/* return valid_bijection(rhs); */

	/* return true; */

	/* std::vector<Graph> sgs, rhs_sgs; */
	/* for (size_t i = 0; i < canon_faces.size(); ++i) { */
	/* 	sgs.push_back(create_subgraph(canon_faces[i].face, g)); */
	/* 	rhs_sgs.push_back(create_subgraph(rhs.canon_faces[i].face, g)); */
	/* } */
	/* bool res = (is_isomorphic(sgs[0], rhs_sgs[0]) && is_isomorphic(sgs[1], rhs_sgs[1])) */
	/* 		  || (is_isomorphic(sgs[0], rhs_sgs[1]) && is_isomorphic(sgs[1], rhs_sgs[0])); */
	/* return res; */
}

std::pair<bool,std::vector<Vertex>> EmbNaive::make_bijection( const Face& lhs
		                                                   , const Face& rhs) const {
	assert(lhs.size() == rhs.size());
	std::vector<Vertex> res(num_vertices(g), -1u);
	bool valid = true;
	for (size_t i = 0; i < lhs.size(); ++i) {
		assert(lhs[i] < res.size() && rhs[i] < res.size());
		if (res[lhs[i]] != -1u && res[lhs[i]] != rhs[i]) {
			valid = false;
			break;
		}
		res[lhs[i]] = rhs[i];
	}
	return std::make_pair(valid, res);
}

bool EmbNaive::valid_bijection(const EmbNaive& rhs) const {
	auto bij1 = make_bijection(canon_faces[1].face, rhs.canon_faces[1].face);
	if (!bij1.first) { return false; }
	auto bij2 = make_bijection(canon_faces[0].face, rhs.canon_faces[0].face);
	if (!bij2.first) { return false; }
	
	for (size_t i = 0; i < bij1.second.size(); ++i) {
		if (bij1.second[i] == -1u || bij2.second[i] == -1u) { continue; }
		if (bij1.second[i] != bij2.second[i]) { return false; }
	}

	return true;
}

bool EmbNaive::check_parity(const EmbNaive& rhs) const {
	bool same = true, diff = true;
	for (size_t i = 0; i < canon_faces.size(); ++i) {
			same = same && ((canon_faces[i].canon.is_forward() == rhs.canon_faces[i].canon.is_forward()) || canon_faces[i].canon.is_palindrome());
			diff = diff && ((canon_faces[i].canon.is_forward() != rhs.canon_faces[i].canon.is_forward()) || canon_faces[i].canon.is_palindrome());
	}
	return same || diff;
}

void EmbNaive::print_gaps() const {
	std::cout << canon << std::endl;
	for (const TCanonFace& fc : canon_faces) {
		std::cout << "(" << ((fc.canon.is_forward()) ? "f" : "b") << ") ";
		std::cout << "(" << ((fc.canon.is_palindrome()) ? "p" : "u") << ") ";
		std::cout << "(" << fc.canon.get_shift() << ") ";
		std::cout << fc.canon << "| ";
	}
	std::cout << std::endl;
}

size_t EmbNaive::hash() const {
	return canon.hash();
	size_t seed = 0;
	for (const TCanonFace &cf : canon_faces) {
		boost::hash_combine(seed, 100);
		boost::hash_combine(seed, cf.canon.hash());
	}
	return seed;
}

} /* namespace embedding */
} /* namespace dtrace */
