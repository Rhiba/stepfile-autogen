#include <iostream>
#include <fstream>
#include <cstring> //for strcmp
#include <stdint.h>
#include <boost/variant.hpp>
#include <vector>
#include <deque>
#include <cmath>
typedef boost::variant<uint8_t, int16_t, int32_t> DataType;
typedef std::vector<DataType> ChannelType;
float getBPM(ChannelType &leftChannel, ChannelType &rightChannel);
