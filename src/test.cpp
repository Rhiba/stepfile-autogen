#include "test.h"

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
				DataType t = channels[currentChannel][indices[currentChannel]];
				if (t.which() == 0) {
					uint8_t tmp8 = boost::get<uint8_t>(channels[currentChannel][indices[currentChannel]]);
					ofs.write((char*)&tmp8, sizeof(tmp8));
				} else if (t.which() == 1) {
					int16_t tmp16 = boost::get<int16_t>(channels[currentChannel][indices[currentChannel]]);
					ofs.write((char*)&tmp16, sizeof(tmp16));
				} else {
					int32_t tmp32 = boost::get<int32_t>(channels[currentChannel][indices[currentChannel]]);
					ofs.write((char*)&tmp32, sizeof(tmp32));
				}
				indices[currentChannel]++;
			}
			currentChannel++;
			if (currentChannel >= channels.size()) {
				currentChannel = 0;
			}
		}
	}

}
