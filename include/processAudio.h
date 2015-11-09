#include <iostream>
#include <fstream>
#include <cstring> //for strcmp
#include <stdint.h>
#include <deque>
#include <cmath>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "datatypes.h"

float getBPMFilterBank(const std::vector<ChannelType> &channels, int sampleRate);
float getBPMFreqSel(const std::vector<ChannelType> &channels, int sampleRate);
