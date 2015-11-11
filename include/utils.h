#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "datatypes.h"
#include <deque>

namespace utils {
	int round(int numToRound, int multiple);
	void fft(CArray& x);
	void ifft(CArray& x);
	float average(std::deque<float> values);
}

#endif //UTILS_H_INCLUDED
