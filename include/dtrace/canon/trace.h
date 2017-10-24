#ifndef DTRACE_CANON_TRACE_H
#define DTRACE_CANON_TRACE_H

#include <dtrace/graph/base.h>

namespace dtrace {
namespace canon {
	
class Trace {
public:
	Trace();
	Trace( const std::vector<size_t> &gaps_f
		  , const std::vector<size_t> &gaps_b);
	Trace(size_t num_vertices, const std::vector<Vertex> &trace);
	Trace(size_t num_vertices, const std::vector<Vertex> &trace, bool only_forw);
	bool operator == (const Trace &other) const;
	bool operator < (const Trace &other) const;
	size_t hash() const;
	size_t get_shift() const;
	size_t get_pshift() const;
	const std::vector<size_t>& get_gaps() const;
	bool is_repeating() const; 
	bool is_forward() const;
	bool is_palindrome() const;
	friend std::ostream & operator << (std::ostream &os, const Trace &other);

private:
	void find_gaps( size_t num_vertices
					  , const std::vector<Vertex> &walk
					  , std::vector<size_t> &gaps_f
					  , std::vector<size_t> &gaps_b
					  , std::vector<std::vector<size_t>> &vgaps_f
					  , std::vector<std::vector<size_t>> &vgaps_b);

	size_t min_expression(const std::vector<size_t> &gaps);
	std::pair<const std::vector<size_t> &, size_t> lex_min(
			  const std::vector<size_t> &gaps_f
			, const std::vector<size_t> &gaps_b
			, size_t shift_f, size_t shift_b);
	bool is_lex_min( const std::vector<size_t> &gaps_f
						, const std::vector<size_t> &gaps_b
						, size_t shift_f, size_t shift_b);

	bool is_repeating(const std::vector<size_t> &gaps) const;

	std::vector<size_t> gaps;
	std::vector<std::vector<size_t>> vertex_gaps;
	size_t shift, pshift;
	bool _is_forward, _is_palindrome, _is_repeating;
};

} /* namespace trace */
} /* namespace dtrace */

namespace std {
	template<> struct hash<dtrace::canon::Trace> {
		size_t operator () (const dtrace::canon::Trace &trace) const {
			return trace.hash();
		}
	};
} /* namespace std */

#endif /* ifndef DTRACE_TRACE_CANON */
