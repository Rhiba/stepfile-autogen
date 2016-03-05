#include "biquad.h"
#include <cmath>
#include <iostream>

BiquadFilter::BiquadFilter() {
	a0 = 1.0;
	a1 = a2 = b1 = b2 = 0.0;
	sampleFreq = 1024;
	Q = 0.707;
	z1 = z2 = 0.0;
}
void BiquadFilter::setBiquad(double Fc, double Q) {
	this->sampleFreq = Fc;
	this->Q = Q;
	calcBiquadFilter();
}
void BiquadFilter::calcBiquadFilter() {
	double norm;
	double K = std::tan(M_PI * sampleFreq);
	norm = 1 / (1 + K / Q + K * K);
	a0 = K * K * norm;
	a1 = 2 * a0;
	a2 = a0;
	b1 = 2 * (K * K - 1) * norm;
	b2 = (1 - K / Q + K * K) * norm;
}
void BiquadFilter::print() {
	std::cout << "a0: " << a0 << " a1: " << a1 << " a2: " << a2 << " b1: " << b1 << " b2: " << b2 << " z1: " << z1 << " z2: " << z2 << " Fc: " << sampleFreq << " Q: " << Q << std::endl;
}
