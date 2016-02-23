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
int generateMarkovSteps(std::string pathToSMFile, double lengthOfSong, double bpm) {
	std::map< std::string, std::map< std::string, double > > probs;
	std::string line;
	std::ifstream myfile ("prob.txt");
	int lineCount = 0;
	bool readingProbs = false;
	std::string currKey = "";
	std::map< std::string, double > currMap;
	int mk = 0;
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			if (lineCount == 0) {
				//found the markov number
				mk = std::stoi(line);
			} else {
				if (line.empty()) {
				} else {
					if (line[line.length()-1] == ':' && readingProbs == false) {
						readingProbs = true;
						currKey = line.substr(0,line.length()-1);
						currMap.clear();
					} else if (line[line.length()-1] == ';') {
						readingProbs = false;
						probs[currKey] = currMap;
					} else if (readingProbs == true) {
						currMap[line.substr(0,4)] = std::stod(line.substr(6,line.length()));
					}
				}
			}
			lineCount++;
		}
		myfile.close();
	}
	//now test it worked
	/*
	for (std::map< std::string, std::map< std::string, double > >::iterator it=probs.begin(); it!=probs.end(); ++it) {
		std::cout << it->first << ":" << std::endl;
		for (std::map< std::string, double >::iterator it2=it->second.begin(); it2!=it->second.end(); ++it2) {
			std::cout << it2->first << ": " << it2->second << std::endl;
		}
	}
	*/

	std::ofstream file;
	file.open(pathToSMFile, std::ios::app);
	double beats = lengthOfSong * (bpm/60.0);
	int measures = floor(beats);
	std::cout << "Number of measures (at 4 beats/measure): " << std::to_string(measures/4) << std::endl;

	std::cout << "Generating base steps." <<std::endl;
	file << "\n#NOTES:\n\tdance-single:\n\t:\n\tHard:8\n\t:\n\t:\n";
	int random = rand() % probs.size();
	std::map<std::string, std::map< std::string, double > >::iterator i1( probs.begin() );
	std::advance( i1, random );
	std::string prevPatt = i1->first;
	file << i1->first;
	int count = 0;
	int fin = rand() % 3;
	int end = 0;
	if (fin == 1) {
		end = 8;
	} else if (fin == 2) {
		end = 16;
	} else if (fin == 3) {
		end = 32;
	} else {
		end = 64;
	}
	std::cout << "end: " << end << std::endl;
	int i = 0;
	bool finished = false;
	bool finishing = false;
	while (i < measures/4 || !finished) {
		if (i >= measures/4) {
			finishing = true;
		}
		if (count == end) {
			if (finishing = true) {
				finished = true;
				break;
			}
			i++;
			file << ",";
			fin = rand() % 4;
			if (fin == 1) {
				end = 8;
			} else if (fin == 2) {
				end = 16;
			} else if (fin == 3) {
				end = 32;
			} else {
				end = 64;
			}
			count = 0;
		}
		std::map< std::string, double > currProbs;
		if (probs.find(prevPatt) != probs.end()) {
			currProbs = probs[prevPatt];
		} else {
			random = rand() % probs.size();
			std::map<std::string, std::map< std::string, double > >::iterator i2( probs.begin() );
			std::advance( i2, random );
			currProbs = i2->second;
		}

		//extract random event based on probability here
		std::string patternChosen = "";
		const double eps = 1e-9;
		double r=((double)rand())/(RAND_MAX); // alternatively here there can be any function that
		// generates a random number uniformly distribute in (0,1);
		for (std::map< std::string, double >::iterator it=currProbs.begin(); it!=currProbs.end(); ++it) {
			r -= it->second;
			if (r < eps) {
				patternChosen = it->first;
				//for now, replace 3's
				std::replace( patternChosen.begin(), patternChosen.end(), '3', '1');
				std::replace( patternChosen.begin(), patternChosen.end(), '2', '1');
				file << patternChosen;
				break;
			}
		}
		prevPatt = prevPatt.substr(4,prevPatt.length()) + patternChosen;
		count++;
	}
	file << ";";
}
