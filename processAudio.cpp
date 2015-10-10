/*
 **
 ** A tool for automatically generating step files for Stepmania. 
 ** Created by Rhiannon Michelmore (rhiannon.michelmore@gmail.com).
 **
 **/
#include "processAudio.h"
#include "test.h"

int main(int argc, char *argv[]) 
{
    int testingMode = 0; //0 for no testing (no creating test wav), 1 for testing
    if (argc < 2) {
        std::cout << "Usage: processAudio <path/to/filename.wav> [flags]" << std::endl;
        std::cout << "Flags:" << std::endl;
        std::cout << "-t --test Testing mode. Will create a test WAV file to check input was read correctly." << std::endl;
        exit(0);
    }
    if (argc == 3) {
        if (!strcmp(argv[2],"-t") || !strcmp(argv[2],"--test")) {
            testingMode = 1;
        } else {
            std::cout << "Unrecognised flag, aborting..." << std::endl;
            exit(0);
        }
    }
    std::ifstream ifs(argv[1], std::ifstream::binary);

    if (!(ifs && ifs.is_open())) {
        std::cout << "File does not exist, exiting..." << std::endl;
        exit(0);
    } else {
        std::cout << "File found: " << argv[1] << std::endl;
    }

    char id[4];

    int size = 0;

    short formatTag = 0;
    short channels = 0;
    short blockAlign = 0;
    short bips = 0;

    int formatLength = 0;
    int sampleRate = 0;
    int abyps = 0;

    int dataSize = 0; //this is in bytes

    std::vector<char> otherData;

    if (!ifs.is_open()) {
        std::cout << "No RIFF match, aborting." << std::endl;
        exit(0);
    }

    for (int i = 0; i < 4; i++) {
        ifs.get(id[i]);	
    }

    std::cout << "Checking ID match." << std::endl;
    std::cout << "ID: " << id << std::endl;

    if (strcmp(id,"RIFF")) {
        std::cout << "No WAVE match found, aborting." << std::endl;
        exit(0);
    }

    std::cout << "RIFF match found, continuing." << std::endl;
    std::cout << "Reading size." << std::endl;

    ifs.read((char*)&size, sizeof(int)); 

    std::cout << "Size: " << size << std::endl;
    std::cout << "Checking inner ID match." << std::endl;

    for (int i = 0; i < 4; i++) {
        ifs.get(id[i]);	
    }

    std::cout << "ID: " << id << std::endl;
    if (!strcmp(id,"WAVE")) {

        std::cout << "WAVE match found, continuing." << std::endl;
        std::cout << "Looking for format." << std::endl;

        for (int i = 0; i < 4; i++) {
            ifs.get(id[i]);	
        }

        std::cout << "Found \"" << id << "\"" << std::endl;
        std::cout << "Reading format length." << std::endl;

        ifs.read((char*)&formatLength, sizeof(int));

        std::cout << "Format length: " << formatLength << std::endl;
        std::cout << "Reading format tag." << std::endl;

        ifs.read((char*)&formatTag, sizeof(short));

        std::cout << "Format tag: " << formatTag;

        if (formatTag == 1) {
            std::cout << " (no compression)" << std::endl;
        } else {
            std::cout << " (compressed)" << std::endl;
        }

        std::cout << "Reading number of channels." << std::endl;

        ifs.read((char*)&channels, sizeof(short));

        std::cout << "Channels: " << channels << std::endl;
        std::cout << "Reading sample rate." << std::endl;

        ifs.read((char*)&sampleRate, sizeof(int));

        std::cout << "Sample rate: " << sampleRate << std::endl;
        std::cout << "Reading average bytes per second." << std::endl;

        ifs.read((char*)&abyps, sizeof(int));

        std::cout << "Average bytes per second: " << abyps << std::endl;
        std::cout << "Reading block align." << std::endl;

        ifs.read((char*)&blockAlign, sizeof(short));

        std::cout << "Block align: " << blockAlign << std::endl;
        std::cout << "Reading bits per sample." << std::endl;

        ifs.read((char*)&bips, sizeof(short));

        std::cout << "Bits per sample: " << bips << std::endl;
        std::cout << "Looking for data. \nConfig bytes skipped over:";

        char c;
        char d[3];
        bool dataFound = false;
        int counter = 1;

        while (!dataFound) {
            ifs.get(c);
            if (c == 'd') {
                for (int i = 0; i < 3; i++) {
                    ifs.get(d[i]);
                }
                if (d[0] == 'a' && d[1] == 't' && d[2] == 'a') {
                    std::cout << "\nFound \"data\"" << std::endl;
                    dataFound = true;
                    break;
                } else {
                    otherData.push_back(c);
                }
            } else {
                otherData.push_back(c);
            }
            std::cout << " [" << counter << "]";
            counter++;
        }

        std::cout << "Reading data size." << std::endl;

        ifs.read((char*)&dataSize,sizeof(int));

        std::cout << "Data size: " << dataSize << std::endl;
        std::cout << std::endl << "Beginning data reading." << std::endl << std::endl;

        ChannelType leftChannel;
        ChannelType rightChannel;

        counter = 0;

        while (!ifs.eof()) {
            DataType tmp; 

            uint8_t tmp8;
            int16_t tmp16;
            int32_t tmp32;

            switch (bips) {
                case 8:
                    ifs.read((char*)&tmp8,sizeof(uint8_t));
                    tmp = tmp8;
                    break;
                case 16:
                    ifs.read((char*)&tmp16,sizeof(int16_t));
                    tmp = tmp16;
                    break;
                case 32:
                    ifs.read((char*)&tmp32,sizeof(int32_t));
                    tmp = tmp32;
                    break;
                default:
                    std::cout << "Bits per second value: " << bips << " not supported." <<std::endl;
                    break;
            }
            if (counter % 2 == 0) {
                leftChannel.push_back(tmp);
            } else {
                rightChannel.push_back(tmp);
            }
            if (counter == (int)((dataSize/2)/4)) {
                std::cout << "25%" << std::endl;
            } else if (counter == (int)((dataSize/2)/2)) {
                std::cout << "50%" << std::endl;
            } else if (counter == (int)((dataSize/2)/2) + (int)((dataSize/2)/4)) {
                std::cout << "75%" << std::endl;
            } else if (counter == (dataSize/2)){
                std::cout << "100%" << std::endl;
            }
            counter++;
        }

        std::cout << std::endl;

        if (testingMode == 1) {
            //Write collected data out to a new wav file to test we read it correctly
            std::cout << "Creating test WAV." << std::endl;
            std::string name = "test.wav";
            testing::createWav(name,size,formatLength,formatTag,channels,sampleRate,abyps,blockAlign,bips,dataSize,leftChannel,rightChannel);
            std::cout << "Finished test WAV, written to: " << name << std::endl;
        }
    }
    return 0;
}
