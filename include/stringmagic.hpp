#ifndef STRINGMAGIC_H_
#define STRINGMAGIC_H_

#include <vector>
#include <sstream>
// Some string utilities

// Split a string given a delimiter and either return in a
// pre-constructed vector (#1) or returns a new one (#2)
inline void split(const std::string &s, char delim, std::vector<std::string> &elems)
{
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim))
		elems.push_back(item);
};

inline std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

#endif