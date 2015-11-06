/*
 **
 ** A tool for automatically generating step files for Stepmania. 
 ** Created by Rhiannon Michelmore (rhiannon.michelmore@gmail.com).
 **
 **/
#include "processAudio.h"
#include "test.h"

//Ensure that structs are packed as they appear here
#pragma pack(1)

//makes this stuff only visible to this file, won't pollute others
namespace
{
	//ostream operator<< for std::array
	template <typename T, size_t N>
	std::ostream& operator<< (std::ostream& out, const std::array<T, N>& arr)
	{
		if(!arr.empty()){
			std::copy(arr.begin(), arr.end(), std::ostream_iterator<T>(out));
		}
		return out;
	}

	//std::array to_string
	template <typename T, size_t N>
	std::string to_string(const std::array<T, N>& arr)
	{
		return std::string(arr.begin(), arr.end());
	}
}

int main(int argc, char *argv[])
{
	auto testingMode = testMode::noTesting;
	if (argc < 2) {
		std::cout << "Usage: processAudio <path/to/filename.wav> [flags]" << std::endl;
		std::cout << "Flags:" << std::endl;
		std::cout << "-t --test Testing mode. Will create a test WAV file to check input was read correctly." << std::endl;
		exit(0);
	}
	if (argc == 3) {
		if (!strcmp(argv[2],"-t") || !strcmp(argv[2],"--test")) {
			testingMode = testMode::createTestWav;
		} else {
			std::cout << "Unrecognised flag, aborting..." << std::endl;
			exit(0);
		}
	}
	std::ifstream ifs(argv[1], std::ifstream::binary);

	if (!(ifs && ifs.is_open())) {
		std::cout << "File does not exist, exiting..." << std::endl;
		exit(0);
	}

	std::cout << "File found: " << argv[1] << std::endl;

	RIFFChunkHeader* mainHeader = nullptr;
	std::vector<char> mainBuf(sizeof(RIFFChunkHeader));
	ifs.read(mainBuf.data(), static_cast<long>(mainBuf.size()));
	mainHeader = reinterpret_cast<RIFFChunkHeader *>(mainBuf.data());

	std::cout << "chunkID: " << mainHeader->chunkId << std::endl;
	if(to_string(mainHeader->chunkId) != "RIFF")
		throw std::runtime_error("No RIFF match found! Found: " + to_string(mainHeader->chunkId));

	std::cout << "chunkSize: " << mainHeader->chunkSize << std::endl;

	std::cout << "Format: " << mainHeader->format << std::endl;
	if (to_string(mainHeader->format) != "WAVE")
		throw std::runtime_error("No WAVE match found! Found: " + to_string(mainHeader->format));


	subChunk1Header* chunk1Header = nullptr;
	std::vector<char> chunk1Buf(sizeof(subChunk1Header));
	ifs.read(chunk1Buf.data(), static_cast<long>(chunk1Buf.size()));
	chunk1Header = reinterpret_cast<subChunk1Header *>(chunk1Buf.data());

	std::cout << "chunkID: \"" << chunk1Header->chunkId << "\"" << std::endl;
	if(to_string(chunk1Header->chunkId) != "fmt ")
		throw std::runtime_error("Not an \"fmt \" chunk! Found: " + to_string(chunk1Header->chunkId));

	std::cout << "Format Chunk Size: " << chunk1Header->chunkSize << std::endl;

	std::cout << "Format tag: " << chunk1Header->audioFormat;
	if (chunk1Header->audioFormat == 1)
		std::cout << " (no compression)" << std::endl;
	else
		std::cout << " (compressed)" << std::endl;

	std::cout << "Number of channels: " << chunk1Header->numChannels << std::endl;

	std::cout << "Sample rate: " << chunk1Header->sampleRate << std::endl;

	std::cout << "Average bytes per second: " << chunk1Header->byteRate << std::endl;

	std::cout << "Block align: " << chunk1Header->blockAlign << std::endl;

	std::cout << "Bits per sample: " << chunk1Header->bitsPerSample << std::endl;

	uint32_t PCMChunkSize = 16;
	uint32_t excessBytes = chunk1Header->chunkSize - PCMChunkSize;
	if(excessBytes)
	{
		std::cout << "Skipping " << excessBytes << " excess bytes at end of format chunk!" << std::endl;
		ifs.seekg(excessBytes, std::ios_base::cur);
	}


	subChunk2Header* chunk2Header = nullptr;
	std::vector<char> chunk2Buf(sizeof(subChunk2Header));
	while(true)
	{
		ifs.read(chunk2Buf.data(), static_cast<long>(chunk2Buf.size()));
		chunk2Header = reinterpret_cast<subChunk2Header *>(chunk2Buf.data());

		std::cout << "chunkID: \"" << chunk2Header->chunkId << "\"" << std::endl;
		if(to_string(chunk2Header->chunkId) != "data")
		{
				std::cout << "Skipping " << chunk2Header->chunkSize << " (rest of chunk)!" << std::endl;
				ifs.seekg(chunk2Header->chunkSize, std::ios_base::cur);
				continue;
		}
		//throw std::runtime_error("Not a \"data\" chunk! Found: " + to_string(chunk2Header->chunkId));

		std::cout << "Data Chunk Size: " << chunk2Header->chunkSize << std::endl;
		break;
	}

	std::cout << std::endl << "Beginning data reading." << std::endl << std::endl;

	std::vector<ChannelType> channels(chunk1Header->numChannels);

	uint32_t counter = 0;

	while (!ifs.eof())
	{
		DataType tmp;

		uint8_t tmp8;
		int16_t tmp16;
		int32_t tmp32;

		switch (chunk1Header->bitsPerSample) {
			case 8:
				ifs.read(reinterpret_cast<char*>(&tmp8),sizeof(uint8_t));
				tmp = tmp8;
				break;
			case 16:
				ifs.read(reinterpret_cast<char*>(&tmp16),sizeof(int16_t));
				tmp = tmp16;
				break;
			case 32:
				ifs.read(reinterpret_cast<char*>(&tmp32),sizeof(int32_t));
				tmp = tmp32;
				break;
			default:
				std::cout << "Bits per second value: " << chunk1Header->bitsPerSample << " not supported." <<std::endl;
				break;
		}

		size_t channelNumber = counter % chunk1Header->numChannels;
		channels[channelNumber].push_back(tmp);

		auto dataSize = chunk2Header->chunkSize/(chunk1Header->bitsPerSample/8);
		if (counter == dataSize/4)
			std::cout << "25%" << std::endl;
		else if (counter == dataSize/2)
			std::cout << "50%" << std::endl;
		else if (counter == (dataSize*3)/4)
			std::cout << "75%" << std::endl;
		else if (counter == dataSize)
			std::cout << "100%" << std::endl;

		counter++;
	}

	std::cout << std::endl << "Starting BPM processing..." << std::endl;
	float bpm = getBPM(channels);

	if (testingMode == testMode::createTestWav) {
		//Write collected data out to a new wav file to test we read it correctly
		std::cout << "Creating test WAV." << std::endl;
		std::string name = "test.wav";
		testing::createWav(name, mainHeader, chunk1Header, chunk2Header, channels);
		std::cout << "Finished test WAV, written to: " << name << std::endl;
	}
}

float getBPM(const std::vector<ChannelType> &channels) {
	return 1.0;
}
