#include <vector>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <fstream>
#include <iostream>

struct Connection {
	double weight;
	double deltaWeight;
};

class Neuron;

typedef std::vector<Neuron> Layer;

class Neuron {
	public:
		Neuron(int numOutputs, int inIdx);
		void setOutputVal(double val) { outputVal = val; }
		double getOutputVal(void) const {return outputVal; }
		void forward(const Layer &prevLayer);
		void calcOutputGradients(double targetVal);
		void calcHiddenGradients(const Layer &nextLayer);
		void updateInputWeights(Layer &prevLayer);
	private:
		static double eta;
		static double alpha;
		static double transfer(double x);
		static double transferDeriv(double x);
		static double randomWeight(void) { return rand() / double(RAND_MAX); }
		double sumDOW(const Layer &nextLayer) const;
		double outputVal;
		std::vector<Connection> outputWeights;
		int idx;
		double gradient;
};

class Net
{
	public:
		Net(const std::vector<int> &topology);
		void forward(const std::vector<double> &inputVals);
		void back(const std::vector<double> &targetVals);
		void results(std::vector<double> &resultVals) const;
		double getRecentAvErr(void) { return recentAvError; };

	private:
	std::vector<Layer> layers;
	double error;
	double recentAvError;
	double recentAvSmooth;
};

