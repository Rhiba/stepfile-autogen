#include "genStep.h"

int makeDirs(std::string pathToStepmania) {
	struct stat st = {0};
	std::string filename = pathToStepmania + "/PartOfMe";
	const char* dirName = filename.c_str();

	if (stat(dirName, &st) == -1) {
		    mkdir(dirName, 0700);
	}
}

int generateStepHeader(float tempo, std::string fname) {
	float bpm = 130;
	std::string filename = "/var/tmp/dcs-get/stepmania-4eab6ff/stepmania-5.0/Songs/Creations/PartOfMe/PartOfMe.sm";
	std::ofstream file;
	file.open(filename);
	file << "#TITLE:PartOfMe;\n#SUBTITLE:;\n#ARTIST:Katy Perry;\n#TITLETRANSLIT:;\n#SUBTITLETRANSLIT:;\n#ARTISTTRANSLIT:;\n#CREDIT:;\n#BANNER:;\n#BACKGROUND:;\n#LYRICSPATH:;\n#CDTITLE:;\n#MUSIC:PartOfMe.wav;\n#OFFSET:-0.675000;\n#SAMPLESTART:45.000;\n#SAMPLELENGTH:12.000;\n#SELECTABLE:YES;\n#BPMS:0.000=130.00;\n#STOPS:;\n#BGCHANGES:;";
}
int generateBaseSteps() {
	std::string filename = "/var/tmp/dcs-get/stepmania-4eab6ff/stepmania-5.0/Songs/Creations/PartOfMe/PartOfMe.sm";
	std::ofstream file;
	file.open(filename, std::ios::app);
	file << "\n#NOTES:\n\tdance-single:\n\t:\n\tHard:8\n\t:\n\t:\n0000\n0000,\n0000\n0000,\n0000\n0000,\n0000\n0000,\n1000\n1000,\n1000\n1000,\n1000\n1000,\n1000\n1000,\n1000\n1000,\n1000\n1000,\n1000\n1000,\n1000\n1000\n;";
}
