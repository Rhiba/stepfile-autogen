#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
int generateStepHeader(float tempo, std::string fname);
int makeDirs(std::string pathToStepmania);
int generateBaseSteps();
