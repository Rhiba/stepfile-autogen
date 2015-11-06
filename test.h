#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED

#include <iostream>
#include <fstream>
#include <cstring> //for strcmp
#include <stdint.h>
#include "datatypes.h"

namespace testing {
	void createWav(
		std::string filename,
		RIFFChunkHeader* mainHeader,
		subChunk1Header* subChunk1,
		subChunk2Header* subChunk2,
		std::vector<ChannelType> channels
	);
}


#endif // TEST_H_INCLUDED
