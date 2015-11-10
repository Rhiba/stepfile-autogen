#include <iostream>
#include <fstream>
#include <cstring> //for strcmp
#include <stdint.h>
#include <deque>
#include <cmath>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "datatypes.h"

float getBPMDWT(const std::vector<ChannelType> &channels, int sampleRate);
float getBPMLowPass(const std::vector<ChannelType> &channels, int sampleRate);
