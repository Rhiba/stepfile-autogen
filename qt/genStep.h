#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <sqlite3.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <map>
int generateStepHeader(std::string pathToStepmania, std::string songName, double tempo, std::string artist, std::string audioFileName, float offset);
int copyAudio(std::string audioPath, std::string pathToStepmania, std::string songName);
int makeDirs(std::string pathToStepmania, std::string songName);
int generateBaseSteps(std::string pathToSMFile, double lengthOfSong, double bpm);
int generateMarkovSteps(std::string pathToSMFile, double lengthOfSong, double bpm);
