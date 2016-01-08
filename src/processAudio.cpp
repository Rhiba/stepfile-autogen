/*
 **
 ** A tool for automatically generating step files for Stepmania. 
 ** Created by Rhiannon Michelmore (rhiannon.michelmore@gmail.com).
 **
 **/
#include "processAudio.h"
#include "test.h"
#include "utils.h"
#include "biquad.h"
#include "genStep.h"

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
	/*
	generateStepHeader(1,"hi");
	generateBaseSteps();
	return 0;
	*/
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
		int16_t tmp;
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
	double bpm = getBPMDWT(channels, chunk1Header->sampleRate);
	/*
	double bpm = getBPMLowPass(channels, chunk1Header->sampleRate);
	std::cout << "BPM with Low Pass filter: " << bpm << std::endl;
	//float bpm = getBPMFreqSel(channels, chunk1Header->sampleRate);
	//std::cout << "BPM with Frequency Select: " << bpm << std::endl;
	std::cout << "Applying biquad low pass filter..." << std::endl;
	BiquadFilter *filt1 = new BiquadFilter();
	BiquadFilter *filt2 = new BiquadFilter();
	filt1->setBiquad(200/((float)chunk1Header->sampleRate),0.707);
	filt2->setBiquad(200/((float)chunk1Header->sampleRate),0.707);
	for (long int i = 0; i < channels[0].size() && i < channels[1].size(); i++) {
		float tmp;
		tmp = (float)channels[0][i];
		tmp = filt1->process(tmp);
		channels[0][i] = tmp;
		tmp = (float)channels[1][i];
		tmp = filt2->process(tmp);
		channels[1][i] = tmp;
	}
	//testing::createDataFile(channels);

	if (testingMode == testMode::createTestWav) {
		//Write collected data out to a new wav file to test we read it correctly
		std::cout << "Creating test WAV." << std::endl;
		std::string name = "test.wav";
		testing::createWav(name, mainHeader, chunk1Header, chunk2Header, channels);
		std::cout << "Finished test WAV, written to: " << name << std::endl;
	}
	*/
}

float getBPMDWT(const std::vector<ChannelType>& channels, int sampleRate) {
	float bpm = 0;
	if (channels.size() != 2) {
		std::cout << "Need two channel WAV file, exiting." << std::endl;
		return 0.0;
	}
	//split into frames with power of 2 lasting about 10 seconds
	std::vector<ChannelType> windows;
	ChannelType tmp_window;
	//assume sample rate is 44100, we'll take 131072 samples which is 2.97 seconds and is also a power of 2
	long int c = 0;
	for (auto& r: channels[0]) {
		if (c < 131072) {
			tmp_window.push_back(r);
			c++;
		} else {
			windows.push_back(tmp_window);
			c = 0;
			tmp_window.clear();
			tmp_window.push_back(r);
			c++;
		}
	}
	//***** ABOVE THIS LINE WORKS (PROBABLY) *****//
	//
	//for each window!
	std::vector< std::pair<double, int> > listOfDistances;
	for (auto& window: windows) {
		std::vector<double> dwt_output, flag;
		std::vector<int> lengths;
		int levels = 4;
		std::vector<double> sig(window.begin(), window.end());
		std::string waveletType = "db4";
		dwt(sig,levels,waveletType,dwt_output,flag,lengths);
		long int acc = 0;
		std::vector<double> AC( window.begin() + acc,window.begin() + acc + lengths[0]);
		acc += lengths[0];
		std::vector<double> DC4(window.begin() + acc,window.begin() + acc + lengths[1]);
		acc += lengths[1];
		std::vector<double> DC3(window.begin() + acc,window.begin() + acc + lengths[2]);
		acc += lengths[2];
		std::vector<double> DC2(window.begin() + acc,window.begin() + acc + lengths[3]);
		acc += lengths[3];
		std::vector<double> DC1(window.begin() + acc,window.begin() + acc + lengths[4]);
		//now need to downsample, then take abs value
		std::vector<double> DC1p, DC2p, DC3p, DC4p;
		/*
		downsamp(DC1, 8, DC1p);
		downsamp(DC2, 4, DC2p);
		downsamp(DC3, 2, DC3p);
		downsamp(DC4, 1, DC4p);
		*/
		/* THIS WORKS
		std::cout << "NORMAL Detail coefficients" << std::endl;
		std::cout << "len of DC1: " << DC1.size() << " dc2: " << DC2.size() << " dc3: " << DC3.size() << " dc4: " << DC4.size() << std::endl;
		std::cout << "DOWNSAMPLED:" << std::endl;
		std::cout << "len of DC1: " << DC1p.size() << " dc2: " << DC2p.size() << " dc3: " << DC3p.size() << " dc4: " << DC4p.size() << std::endl;
		std::cout << std::endl;
		*/
		for (auto& f : DC1) { f = f < 0 ? -f : f;}
		for (auto& f : DC2) { f = f < 0 ? -f : f;}
		for (auto& f : DC3) { f = f < 0 ? -f : f;}
		for (auto& f : DC4) { f = f < 0 ? -f : f;}

		int windowSize = 44100 * 0.25; //cant get songs over 240bpm or less than 60bpm
		int stepSize = windowSize/20;
		//need to store position, times occured, and value for each maxima found.
		//need to remove any that occur < 18 times after their position leaves the window.
		std::vector< std::tuple< int, int, double > > listOfMaxima; //position, occurances, value
		for (int i = 0; i < DC1.size(); i++) {
			double windowMax = 0;
			int positionMax = 0;
			for (int q = i; q < i+windowSize; q++) {
				double val = DC1[q];
				if (val > windowMax) {
					windowMax = val;
					positionMax = q;
					q += stepSize;
				}
			}
			int found = 0;
			for (int k = listOfMaxima.size()-1; k >= 0; k--) {
				std::tuple<int,int,double> temp = listOfMaxima[k];
				if (windowMax == std::get<2>(temp)) {
					std::get<1>(temp)++;
					found = 1;
				}
			}
			if (found == 0) {
				std::tuple<int,int,double> n(positionMax,1,windowMax);
				listOfMaxima.push_back(n);
			}
			//cleaning up old values.
			listOfMaxima.erase(std::remove_if(listOfMaxima.begin(),listOfMaxima.end(), [i](std::tuple<int,int,double> k) {return std::get<0>(k) < i && std::get<1>(k) < 18;}), listOfMaxima.end());
			i += stepSize;
		}
		//std::cout << "size of peak list: " << listOfMaxima.size() << std::endl;
		//now need to record distance between peaks
		//
		std::vector<int> tmpVals;
		for (int i = 0; i < listOfMaxima.size(); i++) {
			for (int j = i - 1; j <= i + 1; j++) {
				if (j < 0 || j > listOfMaxima.size() || j == 0) {
				} else {
					int val = std::abs(std::get<0>(listOfMaxima[i]) - std::get<0>(listOfMaxima[j]));
					tmpVals.push_back(val);
				}
			}
		}
		for (int i = 0; i < tmpVals.size(); i++) {
			int found = 0;
			int tmp = tmpVals[i];
			for (int j = 0; j < listOfDistances.size(); j++) {
				if (listOfDistances[j].first == tmp) {
					found = 1;
					listOfDistances[j].second += 1;
				}
			}
			if (found == 0) {
				std::pair<int,int> n(tmp,1);
				listOfDistances.push_back(n);
			}
		}
		for (int i = 0; i < DC2.size(); i++) {
			double windowMax = 0;
			int positionMax = 0;
			for (int q = i; q < i+windowSize; q++) {
				double val = DC2[q];
				if (val > windowMax) {
					windowMax = val;
					positionMax = q;
					q += stepSize;
				}
			}
			int found = 0;
			for (int k = listOfMaxima.size()-1; k >= 0; k--) {
				std::tuple<int,int,double> temp = listOfMaxima[k];
				if (windowMax == std::get<2>(temp)) {
					std::get<1>(temp)++;
					found = 1;
				}
			}
			if (found == 0) {
				std::tuple<int,int,double> n(positionMax,1,windowMax);
				listOfMaxima.push_back(n);
			}
			//cleaning up old values.
			listOfMaxima.erase(std::remove_if(listOfMaxima.begin(),listOfMaxima.end(), [i](std::tuple<int,int,double> k) {return std::get<0>(k) < i && std::get<1>(k) < 18;}), listOfMaxima.end());
			i += stepSize;
		}
		//std::cout << "size of peak list: " << listOfMaxima.size() << std::endl;
		//now need to record distance between peaks
		//
		tmpVals.clear();
		for (int i = 0; i < listOfMaxima.size(); i++) {
			for (int j = i - 1; j <= i + 1; j++) {
				if (j < 0 || j > listOfMaxima.size() || j == 0) {
				} else {
					int val = std::abs(std::get<0>(listOfMaxima[i]) - std::get<0>(listOfMaxima[j]));
					tmpVals.push_back(val);
				}
			}
		}
		for (int i = 0; i < tmpVals.size(); i++) {
			int found = 0;
			int tmp = tmpVals[i];
			for (int j = 0; j < listOfDistances.size(); j++) {
				if (listOfDistances[j].first == tmp) {
					found = 1;
					listOfDistances[j].second += 1;
				}
			}
			if (found == 0) {
				std::pair<int,int> n(tmp,1);
				listOfDistances.push_back(n);
			}
		}
		for (int i = 0; i < DC3.size(); i++) {
			double windowMax = 0;
			int positionMax = 0;
			for (int q = i; q < i+windowSize; q++) {
				double val = DC3[q];
				if (val > windowMax) {
					windowMax = val;
					positionMax = q;
					q += stepSize;
				}
			}
			int found = 0;
			for (int k = listOfMaxima.size()-1; k >= 0; k--) {
				std::tuple<int,int,double> temp = listOfMaxima[k];
				if (windowMax == std::get<2>(temp)) {
					std::get<1>(temp)++;
					found = 1;
				}
			}
			if (found == 0) {
				std::tuple<int,int,double> n(positionMax,1,windowMax);
				listOfMaxima.push_back(n);
			}
			//cleaning up old values.
			listOfMaxima.erase(std::remove_if(listOfMaxima.begin(),listOfMaxima.end(), [i](std::tuple<int,int,double> k) {return std::get<0>(k) < i && std::get<1>(k) < 18;}), listOfMaxima.end());
			i += stepSize;
		}
		//std::cout << "size of peak list: " << listOfMaxima.size() << std::endl;
		//now need to record distance between peaks
		//
		tmpVals.clear();
		for (int i = 0; i < listOfMaxima.size(); i++) {
			for (int j = i - 1; j <= i + 1; j++) {
				if (j < 0 || j > listOfMaxima.size() || j == 0) {
				} else {
					int val = std::abs(std::get<0>(listOfMaxima[i]) - std::get<0>(listOfMaxima[j]));
					tmpVals.push_back(val);
				}
			}
		}
		for (int i = 0; i < tmpVals.size(); i++) {
			int found = 0;
			int tmp = tmpVals[i];
			for (int j = 0; j < listOfDistances.size(); j++) {
				if (listOfDistances[j].first == tmp) {
					found = 1;
					listOfDistances[j].second += 1;
				}
			}
			if (found == 0) {
				std::pair<int,int> n(tmp,1);
				listOfDistances.push_back(n);
			}
		}
		for (int i = 0; i < DC4.size(); i++) {
			double windowMax = 0;
			int positionMax = 0;
			for (int q = i; q < i+windowSize; q++) {
				double val = DC4[q];
				if (val > windowMax) {
					windowMax = val;
					positionMax = q;
					q += stepSize;
				}
			}
			int found = 0;
			for (int k = listOfMaxima.size()-1; k >= 0; k--) {
				std::tuple<int,int,double> temp = listOfMaxima[k];
				if (windowMax == std::get<2>(temp)) {
					std::get<1>(temp)++;
					found = 1;
				}
			}
			if (found == 0) {
				std::tuple<int,int,double> n(positionMax,1,windowMax);
				listOfMaxima.push_back(n);
			}
			//cleaning up old values.
			listOfMaxima.erase(std::remove_if(listOfMaxima.begin(),listOfMaxima.end(), [i](std::tuple<int,int,double> k) {return std::get<0>(k) < i && std::get<1>(k) < 18;}), listOfMaxima.end());
			i += stepSize;
		}
		//std::cout << "size of peak list: " << listOfMaxima.size() << std::endl;
		//now need to record distance between peaks
		//
		tmpVals.clear();
		for (int i = 0; i < listOfMaxima.size(); i++) {
			for (int j = i - 1; j <= i + 1; j++) {
				if (j < 0 || j > listOfMaxima.size() || j == 0) {
				} else {
					int val = std::abs(std::get<0>(listOfMaxima[i]) - std::get<0>(listOfMaxima[j]));
					tmpVals.push_back(val);
				}
			}
		}
		for (int i = 0; i < tmpVals.size(); i++) {
			int found = 0;
			int tmp = tmpVals[i];
			for (int j = 0; j < listOfDistances.size(); j++) {
				if (listOfDistances[j].first == tmp) {
					found = 1;
					listOfDistances[j].second += 1;
				}
			}
			if (found == 0) {
				std::pair<int,int> n(tmp,1);
				listOfDistances.push_back(n);
			}
		}
		std::cout << "looping" << std::endl;

	}

	std::cout << "size of distance list: " << listOfDistances.size() << std::endl;
	//get most frequent distance
	int maxDist = 0;
	int maxFreq = 0;
	for (int i = 0; i < listOfDistances.size(); i++) {
		if (listOfDistances[i].second > 5 && listOfDistances[i].first > 551 * 1.2) {
			std::cout << listOfDistances[i].second << " value: " << listOfDistances[i].first << std::endl;
		}
		if (listOfDistances[i].second > maxFreq && listOfDistances[i].first != 0 && listOfDistances[i].first > 551*1.2) {
			maxDist = listOfDistances[i].first;
			maxFreq = listOfDistances[i].second;
		}
	}
	std::cout << "most frequent distance: " << maxDist << std::endl;
	bpm = (44100.0/(maxDist*16))*60;
	std::cout << "bpm: " << bpm << std::endl;
	return bpm;
}
double getBPMLowPass(const std::vector<ChannelType>& channels, int sampleRate) {
	float bpm = 0;
	if (channels.size() != 2) {
		std::cout << "Need two channel WAV file, exiting." << std::endl;
		return 0.0;
	}
	std::vector<ChannelType> tmpChannels = channels;
	BiquadFilter *filt1 = new BiquadFilter();
	BiquadFilter *filt2 = new BiquadFilter();
	filt1->setBiquad(200/((float)sampleRate),0.707);
	filt2->setBiquad(200/((float)sampleRate),0.707);
	for (long int i = 0; i < tmpChannels[0].size() && i < tmpChannels[1].size(); i++) {
		float tmp;
		tmp = (float)tmpChannels[0][i];
		tmp = filt1->process(tmp);
		tmpChannels[0][i] = tmp;
		tmp = (float)tmpChannels[1][i];
		tmp = filt2->process(tmp);
		tmpChannels[1][i] = tmp;
	}
	std::deque<float> values(sampleRate);
	for (long int i = 0; i < sampleRate*5; i++) {
		if (values.size() < 44100) {
			values.push_front(std::abs((float)tmpChannels[0][i]+(float)tmpChannels[1][i])/2.0);
		} else {
			values.pop_back();
			values.push_front(std::abs((float)tmpChannels[0][i]+tmpChannels[1][i])/2.0);
		}
		/*
		std::cout << "L Channel: " << tmpChannels[0][i] << " R Channel: " << tmpChannels[1][i] << std::endl;
		std::cout << " Mixed: " << std::abs((float)tmpChannels[0][i] + tmpChannels[1][i])/2.0 << std::endl;
		*/
	}
	//C IS A CONST MULTIPLIER AND CAN BE CHANGED
	//THIS NEEDS TO NOT BE HARD CODED
	double C = 2;
	double average = utils::average(values);
	std::deque<long int> neighbours;
	//map<distance, frequency>
	std::map<double,long int> distHist;
	std::vector<Pair> pairs;
	//start 5 seconds in
	for (long int idx = utils::round(sampleRate,1024)*5; idx < tmpChannels[0].size() && idx < tmpChannels[1].size(); idx++) {
		for (long int windowPtr = idx; windowPtr < sampleRate+idx && windowPtr < tmpChannels[0].size() && windowPtr < tmpChannels[1].size(); windowPtr++) {
			double lChannel = (double)tmpChannels[0][windowPtr];
			double rChannel = (double)tmpChannels[1][windowPtr];
			double sum = std::abs((lChannel + rChannel) / 2.0);
			if (sum > average * C) {
				neighbours.push_back(windowPtr);
				windowPtr += ((float)sampleRate)/4.0;
			} else {
				windowPtr += 1024;
			}
			values.pop_back();
			values.push_front(sum);
			std::pair<long int, double> p(windowPtr,average);
			pairs.push_back(p);
			average = utils::average(values);
		}
		idx += sampleRate/4.0;
	}
	calcDistances(neighbours,distHist);
	double distance = calcMostCommon(distHist);
	std::cout << "Pairs size: " << pairs.size() << std::endl;
	testing::createDataFile(pairs);
	return (60.0/(distance/(double)sampleRate));
}

void calcDistances(std::deque<long int>& neighbours, std::map<double,long int>& hist) {
	//only want to add new distances for the last thing in neighbours as that is newly added
	for (long int i = 0; i < neighbours.size(); i++) {
		for (long int j = i-5; j <= i+5; j++) {
			if (j > 0 && j < neighbours.size() && i != j) {
				double val = std::abs(neighbours[i] - neighbours[j]);
				if (hist.find(val) == hist.end()) {
					hist[val] = 1;
				} else {
					hist[val] += 1;
				}
			}
		}
	}
}

double calcMostCommon(std::map<double,long int>& hist) {
	std::map<double,long int>::iterator it;
	double mostCommonDist = 0;
	long int mostCommonFreq = 0;
	for (it = hist.begin(); it != hist.end(); it++) {
		if (it->first < 1024 || it->first >= 44100) {
			continue;
		} else if (it->second > mostCommonFreq) {
			mostCommonFreq = it->second;
			mostCommonDist = it->first;
			//std::cout << "Most common bpm: " << (60.0/((float)mostCommonDist/(float)44100)) << std::endl;
			std::cout << "Dist: " << it->first << std::endl;
		}
	}
	return mostCommonDist;
}
