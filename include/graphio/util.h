#ifndef GRAPHIO_UTIL_H
#define GRAPHIO_UTIL_H
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string>& split( const std::string& s
	                            , char delim
										 , std::vector<std::string>& elems) {
	std::stringstream ss(s);
	std::string token;
	while (std::getline(ss, token, delim)) {
		if (!token.empty()) { elems.push_back(token); }
	}
	return elems;
}

std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

#endif /* ifndef GRAPHIO_UTIL_H */
