#include <iostream>
#include <fstream>
#include <cstring> //for strcmp
#include <stdint.h>
#include <deque>
#include <map>
#include <cmath>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "datatypes.h"

float getBPMDWT(const std::vector<ChannelType> &channels, int sampleRate);
double getBPMLowPass(const std::vector<ChannelType> &channels, int sampleRate);
double calcMostCommon(std::map<double,long int>& hist);
void calcDistances(std::deque<long int>& neighbours, std::map<double,long int>& hist);
