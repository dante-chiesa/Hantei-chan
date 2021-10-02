#ifndef MISC_H
#define MISC_H

#include <string>

extern bool screenShot;

static inline int to_pow2(int a) {
	int v = 1;
	while (v < a) {
		v <<= 1;
	}
	
	return v;
};

bool ReadInMem(const char *filename, char *&data, unsigned int &size);

std::string sj2utf8(const std::string &input);


#endif
