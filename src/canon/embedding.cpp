#include <dtrace/canon/embedding.h>
#include <dtrace/util.h>
#include <algorithm>

namespace dtrace {
namespace canon {

Embedding::Embedding(const std::vector<Face> &fs, const Graph &g) : g(g) {
	canon = compute_canon(fs);
}

std::vector<Vertex> Embedding::construct_ftrace( const Face& f1, const Face& f2
		                              , size_t s1, size_t s2) {
	std::vector<Vertex> trace;
	trace.push_back(100);
	trace.insert(trace.end(), f1.begin() + s1, f1.end());
	trace.insert(trace.end(), f1.begin(), f1.begin() + s1);
	trace.push_back(100);
	trace.insert(trace.end(), f2.begin() + s2, f2.end());
	trace.insert(trace.end(), f2.begin(), f2.begin() + s2);
	return trace;
}


std::vector<Vertex> Embedding::construct_btrace( const Face& f1, const Face& f2
		                              , size_t s1, size_t s2) {
	size_t rs1 = f1.size() - 1 - ((s1+1)%f1.size());
	size_t rs2 = f2.size() - 1 - ((s2+1)%f2.size());
	std::vector<Vertex> trace;
	trace.push_back(100);
	trace.insert(trace.end(), f1.rbegin() + rs1, f1.rend());
	trace.insert(trace.end(), f1.rbegin(), f1.rbegin() + rs1);
	trace.push_back(100);
	trace.insert(trace.end(), f2.rbegin() + rs2, f2.rend());
	trace.insert(trace.end(), f2.rbegin(), f2.rbegin() + rs2);
	return trace;
}

canon::Trace Embedding::construct_canon(const Face& fixed_f1, const Face& fixed_f2) {
	std::vector<Vertex> glued;
	glued.push_back(100);
	glued.insert(glued.end(), fixed_f1.begin(), fixed_f1.end());
	glued.push_back(100);
	glued.insert(glued.end(), fixed_f2.begin(), fixed_f2.end());
	return canon::Trace(num_vertices(g), glued);
}


canon::Trace Embedding::compute_canon(const std::vector<Face>& fs) {
	canon::Trace min_canon;
	std::vector<Edge> ef1 = vertex_to_edge(g, fs[0]);
	std::vector<Edge> ef2 = vertex_to_edge(g, fs[1]);

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
		Face f1 = fs[0];
		Face f2 = fs[1];
		int sf1 = border_idx[g[e].idx].first;
		int sf2 = border_idx[g[e].idx].second;
		std::vector<Vertex> cand_trace = construct_ftrace(f1, f2, sf1, sf2);
		/* std::cout << fs[0] << "| " << fs[1] << ", s1: " << sf1 << ", s2: " << sf2 <<  std::endl; */
		/* std::cout << "FORWARD: " << cand_trace << std::endl; */
		/* std::rotate(f1.begin(), f1.begin() + sf1, f1.end()); */
		/* std::rotate(f2.begin(), f2.begin() + sf2, f2.end()); */
		/* canon::Trace cand = construct_canon(f1, f2); */
		canon::Trace cand(num_vertices(g), cand_trace, true);
		if (!has_min || cand < min_canon) {
			min_canon = cand;
			has_min = true;
		}
		/* std::reverse(f1.begin(), f1.end()); */
		/* std::reverse(f2.begin(), f2.end()); */
		/* std::rotate(f1.begin(), f1.begin() + (f1.size()-2), f1.end()); */
		/* std::rotate(f2.begin(), f2.begin() + (f2.size()-2), f2.end()); */
		cand_trace = construct_btrace(f1, f2, sf1, sf2);
		/* std::cout << "BACKWAR: " << cand_trace << std::endl; */

		/* cand = construct_canon(f1, f2); */
		cand = canon::Trace(num_vertices(g), cand_trace, true);
		if (cand < min_canon) {
			min_canon = cand;
		}
	}
	return min_canon;
}

bool Embedding::operator == (const Embedding &rhs) const {
	return (canon == rhs.canon);
}


void Embedding::print_gaps() const {
	std::cout << canon << std::endl;
	/* for (const CanonFace& fc : canon_faces) { */
	/* 	std::cout << "(" << ((fc.canon.is_forward()) ? "f" : "b") << ") "; */
	/* 	std::cout << "(" << ((fc.canon.is_palindrome()) ? "p" : "u") << ") "; */
	/* 	std::cout << "(" << fc.canon.get_shift() << ") "; */
	/* 	std::cout << fc.canon << "| "; */
	/* } */
}

size_t Embedding::hash() const {
	return canon.hash();
}

} /* namespace embedding */
} /* namespace dtrace */
