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
	FILE *fp = fopen("partofme.wav","r"); 

	char id[4];

	int size = 0;
	int *pSize = &size;

	short formatTag = 0;
	short *pFormatTag = &formatTag;

	short channels = 0;
	short *pChannels = &channels;

	short blockAlign = 0;
	short *pBlockAlign = &blockAlign;

	short bips = 0;
	short *pBips = &bips;

	int formatLength = 0;
	int *pFormatLength = &formatLength;

	int sampleRate = 0;
	int *pSampleRate = &sampleRate;

	int abyps = 0;
	int *pAbyps = &abyps;

	if (fp) {
		fread(id, sizeof(char), 4, fp);
		std::cout << "Checking ID match." << std::endl;
		std::cout << "ID: " << id[0] << id[1] << id[2] << id[3] << std::endl;
		if (!strcmp(id,"RIFF")) {

			std::cout << "RIFF match found, continuing." << std::endl;
			std::cout << "Reading size." << std::endl;

			fread(pSize,sizeof(int),1,fp);

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

				fread(pFormatLength, sizeof(int), 1, fp);
				
				std::cout << "Format length: " << formatLength << std::endl;
				std::cout << "Reading format tag." << std::endl;

				fread(pFormatTag, sizeof(short), 1, fp);

				std::cout << "Format tag: " << formatTag;

				if (formatTag == 1) {
					std::cout << " (no compression)" << std::endl;
				} else {
					std::cout << " (compressed)" << std::endl;
				}

				std::cout << "Reading number of channels." << std::endl;

				fread(pChannels, sizeof(short), 1, fp);

				std::cout << "Channels: " << channels << std::endl;
				std::cout << "Reading sample rate." << std::endl;

				fread(pSampleRate, sizeof(int), 1, fp);

				std::cout << "Sample rate: " << sampleRate << std::endl;
				std::cout << "Reading average bytes per second." << std::endl;

				fread(pAbyps, sizeof(short), 1, fp);

				std::cout << "Average bytes per second: " << abyps << std::endl;
				std::cout << "Reading block align." << std::endl;

				fread(pBlockAlign, sizeof(short), 1, fp);

				std::cout << "Block align: " << blockAlign << std::endl;
				std::cout << "Reading bits per sample." << std::endl;

				fread(pBips, sizeof(short), 1, fp);

				std::cout << "Bits per sample: " << bips << std::endl;
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
