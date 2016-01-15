# stepfile-autogen
A tool that will autogenerate step files for Stepmania (and programs using the same file format), given a raw audio file and a desired difficulty. :D

##Building
When in top level directory of project:
`mkdir build`
`cd build`
`cmake ..`
`make`

##Running
When in top level directory of project:
`./build/processAudio <audio/path.wav> <path/to/stepmania/Songs> <songname> <artist> [flags]`
