#include "genStep.h"

int makeDirs(std::string pathToStepmania, std::string songName) {
	struct stat st = {0};
	struct stat st2 = {0};

	std::string creationsPath = pathToStepmania + "/Creations";
	const char* createName = creationsPath.c_str();
	std::cout << "Checking if /Creations folder exists... ";
	if (stat(createName, &st2) == -1) {
			std::cout << "no" << std::endl << "Creating /Creations folder." << std::endl;
		    mkdir(createName, 0700);
	} else {
		std::cout << "yes" << std::endl;
	}

	std::string filename = pathToStepmania + "/Creations/" + songName;
	std::cout << "Checking if /Creations/"+songName+" folder exists... ";
	const char* dirName = filename.c_str();
	if (stat(dirName, &st) == -1) {
			std::cout << "no" << std::endl << "Creating /Creations/"+songName+" folder." << std::endl;
		    mkdir(dirName, 0700);
	} else {
		std::cout << "yes" << std::endl;
	}
}
int copyAudio(std::string audioPath, std::string pathToStepmania, std::string songName) {
	std::ifstream  src(audioPath, std::ios::binary);
	std::cout << "Copying audio to new folder." << std::endl;
	std::string extension = "";
	for (int i = audioPath.size() - 1; i >= 0; i--) {
		if (audioPath[i] == '.') {
			extension = audioPath[i] + extension;
			break;
		} else {
			extension = audioPath[i] + extension;
		}
	}
	std::ofstream  dst(pathToStepmania + "/Creations/" + songName + "/" + songName + extension, std::ios::binary);
	dst << src.rdbuf();
}

int generateStepHeader(std::string pathToStepmania, std::string songName, double tempo, std::string artist, std::string audioFileName, float offset) {
	std::cout << "Generating step file: " << songName << ".sm" << std::endl;
	std::cout << "Writing header data." << std::endl;
	std::string filename = pathToStepmania + "/Creations/" + songName + "/" + songName + ".sm";
	std::ofstream file;
	file.open(filename);
	file << "#TITLE:"+songName+";\n#SUBTITLE:;\n#ARTIST:"+artist+";\n#TITLETRANSLIT:;\n#SUBTITLETRANSLIT:;\n#ARTISTTRANSLIT:;\n#CREDIT:;\n#BANNER:;\n#BACKGROUND:;\n#LYRICSPATH:;\n#CDTITLE:;\n#MUSIC:"+audioFileName+";\n#OFFSET:"+std::to_string(offset)+";\n#SAMPLESTART:45.000;\n#SAMPLELENGTH:12.000;\n#SELECTABLE:YES;\n#BPMS:0.000="+std::to_string(tempo)+";\n#STOPS:;\n#BGCHANGES:;";
}
int generateBaseSteps(std::string pathToSMFile, double lengthOfSong, double bpm) {
	std::ofstream file;
	file.open(pathToSMFile, std::ios::app);

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	int row = 0;

	rc = sqlite3_open("step-database", &db);
	if (rc) {
		std::cout << "Uh oh - database not open! Please check 'step-database' file exists and is in the current directory. Aborting..." << std::endl;
		exit(1);
	} else {
		std::cout << "Database open." << std::endl;
	}
	sqlite3_stmt *stmt;
	//for now, getting all patterns
	char sql[] = "SELECT * FROM patterns";
	sqlite3_prepare_v2(db,sql,strlen(sql) + 1, &stmt, NULL);
	std::vector< std::string > patterns;
	while (1) {
		int s;
		s = sqlite3_step (stmt);
		if (s == SQLITE_ROW) {
			int bytes;
			const unsigned char * text;
			bytes = sqlite3_column_bytes(stmt, 4);
			text  = sqlite3_column_text (stmt, 4);
			patterns.push_back((char*)text);
			row++;
		}
		else if (s == SQLITE_DONE) {
			break;
		}
		else {
			std::cout << "Select failed. Aborting..." << std::endl;
			exit (1);
		}
	}
	sqlite3_close(db);

	double beats = lengthOfSong * (bpm/60.0);
	int measures = floor(beats);
	std::cout << "Number of measures (at 4 beats/measure): " << std::to_string(measures/4) << std::endl;

	std::cout << "Generating base steps." <<std::endl;
	file << "\n#NOTES:\n\tdance-single:\n\t:\n\tHard:8\n\t:\n\t:\n";
	for (int i = 0; i < (measures/4); i++) {
		int random = rand() % patterns.size();
		size_t n = std::count(patterns[random].begin(), patterns[random].end(), ',') + 1;
		int c = 0;
		for (int j = 0; j < patterns[random].size(); j++) {
			if (patterns[random][j] == ',') {
				file << ",\n";
				c = 0;
			} else if (j == patterns[random].size() -1) {
				file << ",\n";
				c = 0;
			} else if (c != 3) {
				file << patterns[random][j];
				c++;
			} else {
				file << patterns[random][j] << "\n";
				c = 0;
			}
		}
		i += n;
	}
}
