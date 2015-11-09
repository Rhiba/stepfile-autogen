/*
 **
 ** A tool for automatically generating step files for Stepmania. 
 ** Created by Rhiannon Michelmore (rhiannon.michelmore@gmail.com).
 **
 **/
#include "processAudio.h"
#include "test.h"
#include "utils.h"

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
		uint16_t tmp;
		ifs.read(reinterpret_cast<char*>(&tmp),sizeof(int16_t));

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
	float bpm = getBPMFilterBank(channels, chunk1Header->sampleRate);
	std::cout << "BPM with Filter Bank: " << bpm << std::endl;
	//float bpm = getBPMFreqSel(channels, chunk1Header->sampleRate);
	//std::cout << "BPM with Frequency Select: " << bpm << std::endl;

	if (testingMode == testMode::createTestWav) {
		//Write collected data out to a new wav file to test we read it correctly
		std::cout << "Creating test WAV." << std::endl;
		std::string name = "test.wav";
		testing::createWav(name, mainHeader, chunk1Header, chunk2Header, channels);
		std::cout << "Finished test WAV, written to: " << name << std::endl;
	}
}

float getBPMFreqSel(const std::vector<ChannelType> &channels, int sampleRate) {
	float bpm = 0;
	int beats = 0;
	//start 20 seconds in so we can find a beat in case of long intros
	int startIndex = sampleRate*10;
	//need to computer the history buffer first (43 values)
	//historyBuff[subband(0-31)][0-42]
	std::vector< std::deque<double> > historyBuff(32);
	int currentIndex = startIndex;
	for (int i = 0; i < 43; i++) {
		std::valarray< std::complex<double> > samples(1024);
		for (int j = 0; j < 1024; j++) {
			std::complex<double> tmp(channels[0][currentIndex],channels[1][currentIndex]);
			samples[j] = tmp;
			currentIndex++;
		}
		utils::fft(samples);
		std::vector<double> freqAms(1024);
		for (int s = 0; s < 1024; s++) {
			freqAms[s] = std::abs(samples[s]);
		}
		std::vector<double> subband(32);
		for (int r = 0; r < 32; r++) {
			double sum = 0;
			for (int k = r*32; k < (r+1)*32; k++) {
				sum += freqAms[k];
			}
			subband[r] = 0.03125*sum;
		}
		for (int d = 0; d < 32; d++) {
			historyBuff[d].push_front(subband[d]);
		}
	}
	for (int i = 0; i < 43*10; i++) {
		std::valarray< std::complex<double> > samples(1024);
		for (int j = 0; j < 1024; j++) {
			std::complex<double> tmp(channels[0][currentIndex],channels[1][currentIndex]);
			samples[j] = tmp;
			currentIndex++;
		}
		utils::fft(samples);
		std::vector<double> freqAms(1024);
		for (int s = 0; s < 1024; s++) {
			freqAms[s] = std::abs(samples[s]);
		}
		std::vector<double> subband(32);
		for (int r = 0; r < 32; r++) {
			double sum = 0;
			for (int k = r*32; k < (r+1)*32; k++) {
				sum += freqAms[k];
			}
			subband[r] = 0.03125*sum;
		}
		for (int d = 0; d < 32; d++) {
			double Ei = 0;
			for (int w = 0; w < 43; w++) {
				Ei += historyBuff[d][w];
			}
			Ei *= (1.0/43.0);
			if (subband[d] > Ei*1.4) {
				beats++;
			}
			historyBuff[d].push_front(subband[d]);
		}
	}

	bpm = beats/10.0;

	return bpm;
}

float getBPMFilterBank(const std::vector<ChannelType> &channels, int sampleRate) {
	float bpm = 0;
	if (channels.size() != 2) {
		std::cout << "Need two channel WAV file, exiting." << std::endl;
		return 0.0;
	}
	//This calculates the number of samples needed for 5 seconds of data from the middle of the song, depending on the sampleRate.
	int samplesNeeded = utils::round(sampleRate, 1024) * 2.2;
	std::valarray< std::complex<double> > samplesComplex(samplesNeeded);
	//Just pick somewhere in the middle of the song to start
	int indexToStart = (int)(channels[0].size()/3);
	for (int i = 0; i < samplesNeeded; i++) {
		std::complex<double> tmp((double)channels[0][i+indexToStart],(double)channels[1][i+indexToStart]);
		samplesComplex[i] = tmp;
	}
	utils::fft(samplesComplex);
	//want to test bpms from 60 to 200 = 140bpms
	std::vector< double > E(140);
	for (int bpmc = 0; bpmc < E.size(); bpmc++) {
		//as 60 is our min bpm
		double BPM = bpmc + 60;
		int Ti = (60/BPM)*sampleRate;
		std::valarray< std::complex<double> >lj(samplesNeeded);
		for (int k = 0; k < samplesNeeded; k++) {
			if (k % Ti == 0) {
				//this magic number, see AmpMax in tutorial
				std::complex<double> tmp(32767,32767);
				lj[k] = tmp;
			} else {
				std::complex<double> tmp(0,0);
				lj[k] = tmp;
			}
		}
		utils::fft(lj);
		double sum = 0;
		for (int n = 0; n < samplesNeeded; n++) {
			double num = std::abs(samplesComplex[n]*lj[n]);
			sum += num;
		}
		E[bpmc] = sum;
	}
	double highestFreq = 0;
	for (int r = 0; r < E.size(); r++) {
		if (E[r] > highestFreq) {
			highestFreq = E[r];
			bpm = r+60;
		}
	}
	return bpm;
}
