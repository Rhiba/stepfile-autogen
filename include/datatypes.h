#ifndef DATATYPES_H_INCLUDED
#define DATATYPES_H_INCLUDED

#include <array>
#include <vector>
#include <boost/variant.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <complex>
#include <iostream>
#include <valarray>

typedef std::vector<int16_t> ChannelType;

typedef boost::multiprecision::number< boost::multiprecision::cpp_dec_float<100> >LongDouble;

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

const double PI = 3.141592653589793238460;

enum class testMode
{
	noTesting = 0,
	createTestWav = 1
};

struct RIFFChunkHeader
{
	std::array<char, 4> chunkId;
	uint32_t            chunkSize;
	std::array<char, 4> format;
};

struct subChunk1Header
{
	std::array<char, 4> chunkId;
	uint32_t            chunkSize;
	uint16_t            audioFormat;
	uint16_t            numChannels;
	uint32_t            sampleRate;
	uint32_t            byteRate;
	uint16_t            blockAlign;
	uint16_t            bitsPerSample;
};

struct subChunk2Header
{
	std::array<char, 4> chunkId;
	uint32_t            chunkSize;
};

#endif // DATATYPES_H_INCLUDED
