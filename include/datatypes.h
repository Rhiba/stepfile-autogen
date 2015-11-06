#ifndef DATATYPES_H_INCLUDED
#define DATATYPES_H_INCLUDED

#include <array>
#include <vector>
#include <boost/variant.hpp>

typedef boost::variant<uint8_t, int16_t, int32_t> DataType;
typedef std::vector<DataType> ChannelType;

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
