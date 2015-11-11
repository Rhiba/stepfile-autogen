#include "utils.h"
namespace utils {
	int round(int numToRound, int multiple)
	{
		if(multiple == 0) {  
			return numToRound;  
		}  
		return (int) ((numToRound + multiple/2)/multiple)*multiple;
	}
	void fft(CArray& x)
	{
		const size_t N = x.size();
		if (N <= 1) return;

		// divide
		CArray even = x[std::slice(0, N/2, 2)];
		CArray  odd = x[std::slice(1, N/2, 2)];

		// conquer
		fft(even);
		fft(odd);

		// combine
		for (size_t k = 0; k < N/2; ++k)
		{
			Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
			x[k    ] = even[k] + t;
			x[k+N/2] = even[k] - t;
		}
	}

	void ifft(CArray& x)
	{
		// conjugate the complex numbers
		x = x.apply(std::conj);

		// forward fft
		fft( x );

		// conjugate the complex numbers again
		x = x.apply(std::conj);

		// scale the numbers
		x /= x.size();
	}
	float average(std::deque<float> values) {
		float av = 0;
		for (auto& v:values) {
			av += v;
		}
		return av/((float)values.size());
	}
}
