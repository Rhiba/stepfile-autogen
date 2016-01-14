#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
int generateStepHeader(std::string pathToStepmania, std::string songName, double tempo, std::string artist, std::string audioFileName, float offset);
int copyAudio(std::string audioPath, std::string pathToStepmania, std::string songName);
int makeDirs(std::string pathToStepmania, std::string songName);
int generateBaseSteps(std::string pathToSMFile, double lengthOfSong, double bpm);
