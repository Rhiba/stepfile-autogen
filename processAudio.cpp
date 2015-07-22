/*
**
** A tool for automatically generating step files for Stepmania. 
** Created by Rhiannon Michelmore (rhiannon.michelmore@gmail.com).
**
**/
#include <stdio.h> //for FILE
#include <iostream>
#include <cstring> //for strcmp

int main(int argc, char *argv[]) 
{
	if (argc != 2) {
		std::cout << "Usage: processAudio <path/to/filename.wav>" << std::endl;
		exit(0);
	}
	FILE *fp = fopen(argv[1],"r"); 

	if (fp == NULL) {
		std::cout << "File does not exist, exiting..." << std::endl;
		exit(0);
	} else {
		std::cout << "File found: " << argv[1] << std::endl << std::endl;
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

	int dataSize = 0;

	if (fp) {
		fread(id, sizeof(char), 4, fp);
		std::cout << "Checking ID match." << std::endl;
		std::cout << "ID: " << id[0] << id[1] << id[2] << id[3] << std::endl;
		if (!strcmp(id,"RIFF")) {

			std::cout << "RIFF match found, continuing." << std::endl;
			std::cout << "Reading size." << std::endl;

			fread(&size,sizeof(int),1,fp);

			std::cout << "Size: " << size << std::endl;
			std::cout << "Checking inner ID match." << std::endl;

			fread(id, sizeof(char), 4, fp);

			std::cout << "ID: " << id[0] << id[1] << id[2] << id[3] << std::endl;

			if (!strcmp(id,"WAVE")) {

				std::cout << "WAVE match found, continuing." << std::endl;
				std::cout << "Looking for format." << std::endl;

				fread(id, sizeof(char), 4, fp);

				std::cout << "Found \"" << id[0] << id[1] << id[2] << id[3] << "\"" << std::endl;
				std::cout << "Reading format length." << std::endl;

				fread(&formatLength, sizeof(int), 1, fp);
				
				std::cout << "Format length: " << formatLength << std::endl;
				std::cout << "Reading format tag." << std::endl;

				fread(&formatTag, sizeof(short), 1, fp);

				std::cout << "Format tag: " << formatTag;

				if (formatTag == 1) {
					std::cout << " (no compression)" << std::endl;
				} else {
					std::cout << " (compressed)" << std::endl;
				}

				std::cout << "Reading number of channels." << std::endl;

				fread(&channels, sizeof(short), 1, fp);

				std::cout << "Channels: " << channels << std::endl;
				std::cout << "Reading sample rate." << std::endl;

				fread(&sampleRate, sizeof(int), 1, fp);

				std::cout << "Sample rate: " << sampleRate << std::endl;
				std::cout << "Reading average bytes per second." << std::endl;

				fread(&abyps, sizeof(short), 1, fp);

				std::cout << "Average bytes per second: " << abyps << std::endl;
				std::cout << "Reading block align." << std::endl;

				fread(&blockAlign, sizeof(short), 1, fp);

				std::cout << "Block align: " << blockAlign << std::endl;
				std::cout << "Reading bits per sample." << std::endl;

				fread(&bips, sizeof(short), 1, fp);

				std::cout << "Bits per sample: " << bips << std::endl;
				std::cout << "Reading in extra 2 bytes." << std::endl;
				fread(id, sizeof(short), 1, fp);

				std::cout << "Looking for data. \nConfig bytes skipped over:";

				char c;
				char d[3];
				bool dataFound = false;
				int counter = 1;

				while (!dataFound) {
					fread(&c,sizeof(char),1,fp);
					if (c == 'd') {
						fread(d,sizeof(char),3,fp);
						if (d[0] == 'a' && d[1] == 't' && d[2] == 'a') {
							std::cout << "\nFound \"data\"" << std::endl;
							dataFound = true;
							break;
						}
					}
					std::cout << " [" << counter << "]";
					counter++;
				}

				std::cout << "Reading data size." << std::endl;

				fread(&dataSize,sizeof(int),1,fp);

				std::cout << "Data size: " << dataSize << std::endl;

				
			} else {
				std::cout << "No WAVE match found, aborting." << std::endl;
			}
		} else {
			std::cout << "No RIFF match, aborting." << std::endl;
			exit(0);
		}

	}
	return 0;
}
