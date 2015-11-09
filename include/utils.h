#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "datatypes.h"

namespace utils {
	int round(int numToRound, int multiple);
	void fft(CArray& x);
	void ifft(CArray& x);
}

#endif //UTILS_H_INCLUDED
