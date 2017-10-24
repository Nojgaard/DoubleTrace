#ifndef GRAPHIO_G6_H
#define GRAPHIO_G6_H
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cassert>
#include <boost/graph/lookup_edge.hpp>

namespace graphio {
namespace g6 {

const int BIAS6 = 63;
const int MAXBYTE = 126;
const int SMALLN = 62;
const int SMALLISHN = 258047;
const int WORDSIZE = 32;
const int TOPBIT6 = 32;

int ascii_length(int n) {
	if (n <= SMALLN) { 
		return 1;
	} else if (n <= SMALLISHN) {
		return 4;
	} else {
		return 8;
	}
}

int graph_size(const std::string &str) {
	assert(str.size() > 0);
	if (str[0] == ':') { throw std::runtime_error("graph_size: Invalid character.\n"); }

	int i = 0;
	int n = str[i++] - BIAS6;
	if (n > SMALLN) {
		assert(str.size() > 3);
		n = str[i++] - BIAS6;
		n = (n << 6) | (str[i++] - BIAS6);
		n = (n << 6) | (str[i++] - BIAS6);
	}
	return n;
}

template<typename Graph>
void string_to_graph(const std::string &str, Graph &g) {
	g[boost::graph_bundle].name = str;
	int n = graph_size(str);
	if (n == 0) { return; }

	assert(str[0] != ':' && str[0] != '&');

	for (int i = 0; i < n; ++i) { add_vertex(g); }
	int p = ascii_length(n);
	int k = 1, x = 0;
	for (int j = 1; j < n; ++j) {
		for (int i = 0; i < j; ++i) {
			if (--k == 0) {
				k = 6;
				x = str[p++] - BIAS6;
			}
			if ((x & TOPBIT6)) {
				auto pe = add_edge(i, j, g);
				assert(pe.second);
			}
			x <<= 1;
		}
	}
}

template<typename Graph>
void read(std::istream &s, std::vector<Graph> &gs) {
	std::string line;
	while (std::getline(s, line)) {
		gs.push_back(Graph());
		string_to_graph(line, gs.back());
	}
}

template<typename Graph>
void read(const std::string& s, Graph& g) {
	string_to_graph(s, g);
}

void write_n(std::ostream &os, unsigned long n) {
	if (n < SMALLN) {
		os << ((char)(n+BIAS6));
		return;
	}
	os << ((char)MAXBYTE);
	int num_bytes = 2;
	if (n > SMALLISHN) {
		os << ((char)MAXBYTE);
		num_bytes = 3;
	}
	for (int byte = num_bytes; byte >= 0; --byte) {
		unsigned char c = 0;
		int n_pos = 6*(byte+1)-1;
		for (int c_pos = 5; c_pos >= 0; --n_pos, --c_pos) {
			c += (n & (1<<n_pos))>>((int)n_pos/6);
		}
		os << (c+63);
	}
}

template<typename Graph>
void write(const Graph &g, std::ostream &os) {
	unsigned long n = num_vertices(g);
	write_n(os, n);

	unsigned char byte = 0;
	int byte_pos = 5;
	unsigned int bytes_written = 0;
	for (size_t j = 1; j < n; ++j) {
		for (size_t i = 0; i < j; ++i) {
			unsigned int bit = boost::lookup_edge(i, j, g).second;
			byte = byte | (bit << byte_pos--);
			if (byte_pos < 0) {
				assert((byte+BIAS6) <= MAXBYTE);
				os << ((char)(byte+BIAS6));
				++bytes_written;
				byte = 0; byte_pos = 5;
			}
		}
	}
	if (byte_pos != 5) {
		assert((byte+BIAS6) <= MAXBYTE);
		os << ((char)(byte + BIAS6));
		++bytes_written;
		byte = 0;
	}
	assert(byte == 0);
	assert(bytes_written == (int)std::ceil(n*(n-1)/12.0f));
	os << std::endl;
}

template<typename Graph>
void write(const std::vector<Graph> &gs, std::ostream &os) {
	for (const Graph &g : gs) {
		write(g, os);
	}
}

} /* namespace g6 */
} /* namespace graphio */
#endif /* ifndef GRAPHIO_G6_H */
