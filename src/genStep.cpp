#include "genStep.h"

int generateStepHeader(float tempo, std::string fname) {
	float bpm = 130;
	std::string filename = "../test/caramell.sm";
	std::ofstream file;
	file.open(filename);
	file << "#TITLE:Caramelldansen;\n#SUBTITLE:;\n#ARTIST:Caramell Girls;\n#TITLETRANSLIT:;\n#SUBTITLETRANSLIT:;\n#ARTISTTRANSLIT:;\n#CREDIT:;\n#BANNER:;\n#BACKGROUND:;\n#LYRICSPATH:;\n#CDTITLE:;\n#MUSIC:caramell.wav;\n#OFFSET:0.000;\n#SAMPLESTART:30.000;\n#SAMPLELENGTH:5.000;\n#SELECTABLE:YES;\n#BPMS:0.000=164.23;\n#STOPS:;\n#BGCHANGES:;";
}
int generateBaseSteps() {
	std::string filename = "../test/caramell.sm";
	std::ofstream file;
	file.open(filename, std::ios::app);
	file << "\n#NOTES:dance-single::Hard:8::10001000,10001000,10001000,10001000,10001000,10001000,10001000,10001000,10001000,10001000,10001000,10001000;";
}
