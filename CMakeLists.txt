cmake_minimum_required (VERSION 2.6)
project (processAudio)
set(CMAKE_CXX_FLAGS "-O0 --std=c++0x")
set(CMAKE_CXX_COMPILER /bin/g++)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(processAudio ${SOURCES})
