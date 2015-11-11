#include "test.h"
#include "datatypes.h"

void testing::createWav(
	std::string filename,
	RIFFChunkHeader* mainHeader,
	subChunk1Header* subChunk1,
	subChunk2Header* subChunk2,
	std::vector<ChannelType> channels
)
{
	std::ofstream ofs(filename, std::ofstream::binary);
	if (ofs.is_open()) {
		ofs << "RIFF";
		ofs.write((char*)&mainHeader->chunkSize, sizeof(int));
		ofs << "WAVE";
		ofs << "fmt ";
		ofs.write((char*)&subChunk1->chunkSize,sizeof(int));
		ofs.write((char*)&subChunk1->audioFormat,sizeof(short));
		ofs.write((char*)&subChunk1->numChannels,sizeof(short));
		ofs.write((char*)&subChunk1->sampleRate,sizeof(int));
		ofs.write((char*)&subChunk1->byteRate,sizeof(int));
		ofs.write((char*)&subChunk1->blockAlign,sizeof(short));
		ofs.write((char*)&subChunk1->bitsPerSample,sizeof(short));
		ofs << "data";
		ofs.write((char*)&subChunk2->chunkSize,sizeof(int));

		int currentChannel = 0;
		//initialises them to 0
		std::vector<int> indices (channels.size());

		for (int i = 0; i < channels.size()*channels[0].size(); i++) {
			if (indices[currentChannel] < channels[currentChannel].size()) {
				ofs.write((char*)&channels[currentChannel][indices[currentChannel]],sizeof(int16_t));
				indices[currentChannel]++;
			}
			currentChannel++;
			if (currentChannel >= channels.size()) {
				currentChannel = 0;
			}
		}
	}

}
void testing::createDataFile(std::vector< std::pair<long int, double> > pairs) {
	std::ofstream ofs("data");
	std::sort(pairs.begin(),pairs.end());
	if (ofs.is_open()) {
		std::ostringstream ss;
		for (long int i = 0; i < pairs.size(); i++) {
			ss << pairs[i].first << " ";
			ss << pairs[i].second;
			ofs << ss.str();
		}
	}
}
