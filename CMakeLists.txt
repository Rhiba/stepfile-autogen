cmake_minimum_required (VERSION 2.6)
project (processAudio)
set(CMAKE_CXX_FLAGS "-O0 --std=c++0x -I/dcs/13/csuneh/3YP/stepfile-autogen/third/fftw/include -L/dcs/13/csuneh/3YP/stepfile-autogen/third/fftw/lib")
set(CMAKE_CXX_COMPILER /bin/g++)

include_directories(include)

file(GLOB SOURCES "src/*.cpp" "src/third/*.cpp")

add_executable(processAudio ${SOURCES})

target_link_libraries(processAudio 
	fftw3
	sqlite3
	)
