#include "biquad.h";
#include <cmath>

BiquadFilter::BiquadFilter() {
	a0 = 1.0;
	a1 = a2 = b1 = b2 = 0.0;
	sampleFreq = 0.25;
	Q = 0.707;
	z1 = z2 = 0.0;
}
BiquadFilter::~BiquadFilter() {
}
