#include <dtrace/canon/trace.h>
#include <dtrace/util.h>
#include <iostream>

namespace dtrace {
namespace canon {

void find_gapss( size_t num_vertices
		        , const std::vector<Vertex>& walk
				  , std::vector<size_t>& gaps
				  , bool forward) {
	std::vector<int> last_seen(num_vertices, -1);
	int i = 0, assigned = 0;
	gaps.resize(walk.size(), 0);
	
	int ass_exc = 0;
	while (assigned != (int)walk.size()) {
		Vertex v = walk[i];
		/* std::cout << assigned << " " << walk.size()  << std::endl; */
		if (v > num_vertices) {
			i = (i + 1)%walk.size();
			if (ass_exc < 2) {
				++assigned;
				++ass_exc;
			}
			continue;
		}
		int j = last_seen[v];
		if (j != -1) {
			if (!forward && gaps[walk.size() - 1 - i] == 0) {++assigned; }
			else if (forward && gaps[j] == 0) { ++assigned; }
			size_t d = ((i > j) ? i - j : walk.size() - j + i);
			if (forward) { gaps[j] = d; } else { gaps[walk.size() - 1 - i] = d; }
		}
		last_seen[v] = i;
		i = (i + 1)%walk.size();
	}
	/* if (!forward) { std::reverse(gaps.begin(), gaps.end()); } */

	/* for (Vertex v : walk) { */
	/* 	std::cout << v << " "; */
	/* } */
	/* std::cout << std::endl; */
	/* for (size_t g : gaps) { */
	/* 	std::cout << g << " "; */
	/* } */
	/* std::cout << ((forward) ? "(f)" : "(b)") << " "; */
	/* std::cout << std::endl; */
	/* std::cout << "-------" << std::endl; */
}

Trace::Trace() {}

Trace::Trace( const std::vector<size_t> &gaps_f
				, const std::vector<size_t> &gaps_b) {
	size_t shift_f = min_expression(gaps_f);
	size_t shift_b = min_expression(gaps_b);
	auto gaps_min = lex_min(gaps_f, gaps_b, shift_f, shift_b);
	gaps = gaps_min.first;
	std::rotate(gaps.begin(), gaps.begin() + gaps_min.second, gaps.end());
	shift = gaps_min.second;
}

Trace::Trace(size_t num_vertices, const std::vector<Vertex> &trace, bool only_forward) {
	std::vector<size_t> gaps_f, gaps_b;
	find_gapss(num_vertices, trace, gaps_f, true);
	size_t shift_f = min_expression(gaps_f);
	std::rotate(gaps_f.begin(), gaps_f.begin() + shift_f, gaps_f.end());
	_is_forward = true;
	shift = shift_f;
	gaps = gaps_f;
	_is_repeating = is_repeating(gaps);
	_is_palindrome = (gaps_f == gaps_b);
}

Trace::Trace(size_t num_vertices, const std::vector<Vertex> &trace) {
	std::vector<size_t> gaps_f, gaps_b;
	std::vector<std::vector<size_t>> vgaps_f(num_vertices), vgaps_b(num_vertices);
	/* find_gaps(num_vertices, trace, gaps_f, gaps_b, vgaps_f, vgaps_b); */
	find_gapss(num_vertices, trace, gaps_f, true);
	find_gapss(num_vertices, trace, gaps_b, false);
	size_t shift_f = min_expression(gaps_f);
	size_t shift_b = min_expression(gaps_b);
	std::rotate(gaps_f.begin(), gaps_f.begin() + shift_f, gaps_f.end());
	std::rotate(gaps_b.begin(), gaps_b.begin() + shift_b, gaps_b.end());
	/* if (is_lex_min(gaps_f, gaps_b, shift_f, shift_b)) { */
	if (gaps_f <= gaps_b) {
		_is_forward = true;
		shift = shift_f;
		pshift = shift_b;
		gaps = gaps_f;
		vertex_gaps = vgaps_f;
	} else {
		_is_forward = false;
		shift = shift_b;
		pshift = shift_f;
		gaps = gaps_b;
		vertex_gaps = vgaps_b;
	}
	/* std::rotate(gaps.begin(), gaps.begin() + shift, gaps.end()); */
	_is_repeating = is_repeating(gaps);
	_is_palindrome = (gaps_f == gaps_b);

	/* for (auto &vgaps : vertex_gaps) { */
	/* 	size_t me = min_expression(vgaps); */
	/* 	std::rotate(vgaps.begin(), vgaps.begin() + me, vgaps.end()); */
	/* } */
}

bool Trace::is_repeating(const std::vector<size_t>& gaps) const {
	for (size_t i = 1; i < gaps.size(); ++i) {
		if (gaps[0] != gaps[i]) { return false; }
	}
	return true;
}

bool Trace::is_palindrome() const {
	return _is_palindrome;
}

bool Trace::is_repeating() const {
	return _is_repeating;
}

bool Trace::is_forward() const {
	return _is_forward;
}

std::pair<const std::vector<size_t> &, size_t> Trace::lex_min(
		  const std::vector<size_t> &gaps_f
		, const std::vector<size_t> &gaps_b
		, size_t shift_f, size_t shift_b) {

	for (size_t i = 0; i < gaps_f.size(); ++i) {
		size_t fs = (i+shift_f)%gaps_f.size();
		size_t bs = (i+shift_b)%gaps_b.size();
		if (gaps_f.at(fs) < gaps_b.at(bs)) {
			return std::pair<const std::vector<size_t> &, size_t>(gaps_f, shift_f);
		} else if (gaps_f.at(fs) > gaps_b.at(bs)) {
			return std::pair<const std::vector<size_t> &, size_t>(gaps_b, shift_b);
		}
	}
	return std::pair<const std::vector<size_t> &, size_t>(gaps_f, shift_f);
}

bool Trace::is_lex_min( const std::vector<size_t> &gaps_f
				  			 , const std::vector<size_t> &gaps_b
							 , size_t shift_f, size_t shift_b) {
	for (size_t i = 0; i < gaps_f.size(); ++i) {
		size_t fs = (i+shift_f)%gaps_f.size();
		size_t bs = (i+shift_b)%gaps_b.size();
		if (gaps_f.at(fs) < gaps_b.at(bs)) {
			return true;
		} else if (gaps_f.at(fs) > gaps_b.at(bs)) {
			return false;
		}
	}
	return true;
}

size_t Trace::min_expression(const std::vector<size_t> &gaps) {
	size_t i = 0, j = 1, k = 0;
	while (i < gaps.size() && j < gaps.size() && k < gaps.size()) {
		size_t ik = (i + k) % gaps.size();
		size_t jk = (j + k) % gaps.size();
		if (gaps.at(ik) == gaps.at(jk)) { ++k; } 
		else if (gaps.at(ik) > gaps.at(jk)) {
			i = i + k + 1;
			if (i <= j) { i = j + 1; }
			k = 0;
		} else {
			j = j + k + 1;
			if (j <= i) { j = i + 1; }
			k = 0;
		}
	}
	size_t min_pos = std::min(i, j);
	return min_pos;
}

bool Trace::operator == (const Trace &other) const {
	/* assert(gaps.size() == other.gaps.size()); */
	return gaps == other.gaps;
}

bool Trace::operator < (const Trace &other) const {
	return gaps < other.gaps;
}

size_t Trace::hash() const {
	size_t seed = 0;
	for (size_t val : gaps) { boost::hash_combine(seed, val); }
	return seed;
}


void Trace::find_gaps( size_t num_vertices
						   , const std::vector<Vertex> &walk
						   , std::vector<size_t> &gaps_f
						   , std::vector<size_t> &gaps_b
						   , std::vector<std::vector<size_t>> &vgaps_f
						   , std::vector<std::vector<size_t>> &vgaps_b) {
	assert(vgaps_f.size() == num_vertices);
	assert(vgaps_b.size() == num_vertices);

	size_t i = 0, assigned_f = 0, assigned_b = 0;
	std::vector<int> last_seen_f(num_vertices, -1);
	std::vector<int> last_seen_b(num_vertices, -1);
	std::vector<bool> ass_f(num_vertices, false);
	std::vector<bool> ass_b(num_vertices, false);
	gaps_f.resize(walk.size(), 0);
	gaps_b.resize(walk.size(), 0);
	while(assigned_f != walk.size() || assigned_b != walk.size()) {
		Vertex vf = walk.at(i);
		size_t j = (size_t)last_seen_f.at(vf);
		if (last_seen_f.at(vf) != -1 && !ass_f[j]) {
			++assigned_f;
			ass_f[j] = true;
			size_t d = ((i > j) ? i - j : walk.size() - j + i);
			gaps_f.at(j) = d;
			vgaps_f.at(vf).push_back(d);
			/* if (gaps_f.at(j) == gaps_f.size()) { gaps_f.at(j) = 0; } */
		}
		last_seen_f.at(vf) = i;

		size_t ib = walk.size() - i - 1;
		Vertex vb = walk.at(ib);
		j = (size_t)last_seen_b.at(vb);
		size_t bpos = walk.size() - j - 1;
		if (last_seen_b.at(vb) != -1 && !ass_b[bpos]) {
			++assigned_b;
			ass_b[bpos] = true;
			size_t d = ((j > ib) ? j - ib : walk.size() - ib + j);
			gaps_b.at(bpos) = d;
			vgaps_b.at(vb).push_back(d);
			/* if (gaps_b.at(walk.size() - j-1) == gaps_b.size()) { gaps_b.at(walk.size() - j-1) = 0; } */
		}
		last_seen_b.at(vb) = ib;

		i = (i + 1)%walk.size();
	}
}

size_t Trace::get_shift() const {
	return shift;
}

size_t Trace::get_pshift() const {
	return pshift;
}

const std::vector<size_t>& Trace::get_gaps() const {
	return gaps;
}

std::ostream & operator << (std::ostream &os, const Trace &other) {
	for (size_t i : other.gaps) {
		os << i << " ";
	}
	return os;
}

} /* namespace trace */
} /* namespace dtrace */
