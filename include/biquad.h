#ifndef BIQUAD_H_INCLUDED
#define BIQUAD_H_INCLUDED

class BiquadFilter {
	public:
		BiquadFilter();
		float process(float in);
	
	protected:
		void calcBiquadFilter(void);

		double a0, a1, a2, b1, b2;
		double sampleFreq, Q;
		double z1, z2;
};

inline float BiquadFilter::process(float in) {
	double out = (in * a0) + z1;
	z1 = (in * a1) + z2 - (b1 * out);
	z2 = (in * a2) - (b2 * out);
	return out;
}

#endif //BIQUAD_H_INCLUDED
