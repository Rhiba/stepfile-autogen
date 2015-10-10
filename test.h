#include <iostream>
#include <fstream>
#include <cstring> //for strcmp
#include <stdint.h>
#include <boost/variant.hpp>
#include <vector>
namespace testing {
    void createWav(std::string filename, int size, int formatLength, short formatTag, short channels, int sampleRate, int abyps, short blockAlign, short bips, int dataSize, ChannelType leftChannel, ChannelType rightChannel) {
        std::ofstream ofs(filename, std::ofstream::binary);
        if (ofs.is_open()) {
            ofs << "RIFF";
            ofs.write((char*)&size, sizeof(size));
            ofs << "WAVE";
            ofs << "fmt ";
            ofs.write((char*)&formatLength,sizeof(int));
            ofs.write((char*)&formatTag,sizeof(short));
            ofs.write((char*)&channels,sizeof(short));
            ofs.write((char*)&sampleRate,sizeof(int));
            ofs.write((char*)&abyps,sizeof(int));
            ofs.write((char*)&blockAlign,sizeof(short));
            ofs.write((char*)&bips,sizeof(short));
            ofs << "data";
            ofs.write((char*)&dataSize,sizeof(int));

            int leftCount = 0;
            int rightCount = 0;

            for (int i = 0; i < 2*leftChannel.size(); i++) {
                if (i % 2 == 0) {
                    if (leftCount < leftChannel.size()) {
                        DataType t = leftChannel.at(leftCount);
                        if (t.which() == 0) {
                            uint8_t tmp8 = boost::get<uint8_t>(leftChannel.at(leftCount));
                            ofs.write((char*)&tmp8, sizeof(tmp8));
                        } else if (t.which() == 1) {
                            int16_t tmp16 = boost::get<int16_t>(leftChannel.at(leftCount));
                            ofs.write((char*)&tmp16, sizeof(tmp16));
                        } else {
                            int32_t tmp32 = boost::get<int32_t>(leftChannel.at(leftCount));
                            ofs.write((char*)&tmp32, sizeof(tmp32));
                        }
                    }
                    leftCount++;
                }  else {
                    if (rightCount < rightChannel.size()) {
                        DataType t = rightChannel.at(rightCount);
                        if (t.which() == 0) {
                            uint8_t tmp8 = boost::get<uint8_t>(rightChannel.at(rightCount));
                            ofs.write((char*)&tmp8, sizeof(tmp8));
                        } else if (t.which() == 1) {
                            int16_t tmp16 = boost::get<int16_t>(rightChannel.at(rightCount));
                            ofs.write((char*)&tmp16, sizeof(tmp16));
                        } else {
                            int32_t tmp32 = boost::get<int32_t>(rightChannel.at(rightCount));
                            ofs.write((char*)&tmp32, sizeof(tmp32));
                        }
                        rightCount++;
                    }
                }
            }
        }
    }
}
