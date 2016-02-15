#include "nn.h"

int main() {
	std::vector<int> topology;
	topology.push_back(10); //2 inputs
	topology.push_back(5); //2 hidden nodes
	topology.push_back(1); //1 output
	Net nn(topology);

	std::ifstream infile("songdata.txt");
	std::string line;
	int pass = 0;
	while (std::getline(infile,line)) {
		pass++;
		size_t pos = 0;
		std::string token;
		int counter = 0;
		std::vector<double> inputVals;
		std::vector<double> resultsVals;
		std::vector<double> targetVals;
		while ((pos = line.find(' ')) != std::string::npos) {
			token = line.substr(0, pos);
			if (counter == 0) {
				targetVals.push_back(std::stod(token));
			} else {
				inputVals.push_back(std::stod(token));
			}
			counter++;
			line.erase(0, pos + 1);
		}
		std::cout << "size of input list: " << inputVals.size() << std::endl;
		std::cout << "Pass: " << pass << std::endl;
		nn.forward(inputVals);
		std::cout << "Target: " << (targetVals[0]*(20-1) + 1) << std::endl;
		nn.results(resultsVals);
		std::cout << "Actual: " << (resultsVals[0]*(20-1) + 1) << std::endl;
		nn.back(targetVals);
		std::cout << "Recent err: " << nn.getRecentAvErr() << std::endl << std::endl;
	}

	/*
	   int pass = 1;
	   double a, b, res;
	   while (infile >> a >> b >> res) {
	   std::cout << "Pass: " << pass << std::endl;
	   pass++;
	   std::cout << "Inputs: " << a << " " << b << std::endl;
	   std::vector<double> inputVals;
	   inputVals.push_back(a);
	   inputVals.push_back(b);
	   nn.forward(inputVals);
	   std::cout << "Target: " << res << std::endl;

	   std::vector<double> resultsVals;
	   nn.results(resultsVals);
	   std::cout << "Actual: " << resultsVals[0] << std::endl;

	   std::vector<double> targetVals;
	   targetVals.push_back(res);
	   nn.back(targetVals);
	   std::cout << "Recent err: " << nn.getRecentAvErr() << std::endl << std::endl;
	   std::cout << nn.getRecentAvErr() << std::endl;

	   }
	   */
}

Net::Net(const std::vector<int> &topology) { //checked
	int numLayers = topology.size();
	//iterate through layers
	for (int i = 0; i < numLayers; ++i) {
		layers.push_back(Layer());
		int numOutputs = i == topology.size() - 1 ? 0 : topology[i+1];
		//iterate through neurons
		for (int j = 0; j <= topology[i]; ++j) {
			layers.back().push_back(Neuron(numOutputs,j));
		}
		layers.back().back().setOutputVal(1.0);
	}
}
void Net::forward(const std::vector<double> &inputVals) { //checked
	//std::cout << "input size: " << inputVals.size() << " 1st layer size -1: " << layers[0].size() -1 << std::endl;
	assert(inputVals.size() == layers[0].size() - 1);

	for (int i = 0; i < inputVals.size(); ++i) {
		layers[0][i].setOutputVal(inputVals[i]);
	}

	for (int i = 1; i < layers.size(); ++i) {
		Layer &prevLayer = layers[i - 1];
		for (int j = 0; j < layers[i].size() -1; ++j) {
			layers[i][j].forward(prevLayer);
		}
	}
}
void Net::back(const std::vector<double> &targetVals) { //checked
	Layer &outputLayer = layers.back();
	error = 0.0;
	for (int i = 0; i < outputLayer.size()-1; ++i) {
		double d = targetVals[i] - outputLayer[i].getOutputVal();
		error += d * d;
	}
	error /= outputLayer.size() -1;
	error = sqrt(error);

	recentAvError = (recentAvError * recentAvSmooth + error) / (recentAvSmooth + 1.0);

	for (int n = 0; n < outputLayer.size() -1; ++n) {
		outputLayer[n].calcOutputGradients(targetVals[n]);
	}
	for (int n = layers.size() -2; n > 0; --n) {
		Layer &hidden = layers[n];
		Layer &next = layers[n+1];
		for (int i = 0; i < hidden.size(); ++i) {
			hidden[i].calcHiddenGradients(next);
		}
	}
	for (int i = layers.size()-1; i > 0; --i) {
		Layer &layer = layers[i];
		Layer &prevLayer = layers[i-1];
		for (int n = 0; n < layer.size()-1; ++n) {
			layer[n].updateInputWeights(prevLayer);
		}
	}
}
void Net::results(std::vector<double> &resultVals) const { //checked
	resultVals.clear();
	for (int n = 0; n < layers.back().size() -1; ++n) {
		resultVals.push_back(layers.back()[n].getOutputVal());
	}
}

Neuron::Neuron(int numOutputs, int inIdx) {
	for (int i = 0; i < numOutputs; ++i) {
		outputWeights.push_back(Connection());
		outputWeights.back().weight = randomWeight();
	}
	idx = inIdx;
}
void Neuron::calcOutputGradients(double targetVal) { //checked
	double d = targetVal - outputVal;
	gradient = d * Neuron::transferDeriv(outputVal);
}
void Neuron::calcHiddenGradients(const Layer &nextLayer) { //checked
	double dow = sumDOW(nextLayer);
	gradient = dow * Neuron::transferDeriv(outputVal);
}
double Neuron::sumDOW(const Layer &nextLayer) const { //checked
	double sum = 0.0;

	for (int n = 0; n< nextLayer.size() -1; ++n) {
		sum += outputWeights[n].weight * nextLayer[n].gradient;
	}
	return sum;
}
double Neuron::eta = 0.2;
double Neuron::alpha = 0.5;
void Neuron::updateInputWeights(Layer &prevLayer) { //checked

	for (int n = 0; n < prevLayer.size(); ++n) {
		Neuron &neuron = prevLayer[n];
		double oldDelta = neuron.outputWeights[idx].deltaWeight;
		double newDelta = eta * neuron.getOutputVal() * gradient + alpha * oldDelta;
		neuron.outputWeights[idx].deltaWeight = newDelta;
		neuron.outputWeights[idx].weight += newDelta; // I HATE THIS PLUS EQUALS
	}
}
double Neuron::transfer(double x) {
	return tanh(x);
}
double Neuron::transferDeriv(double x) {
	return 1 - x*x;
}
void Neuron::forward(const Layer &prevLayer) { //checked
	double sum = 0.0;

	for (int i = 0; i < prevLayer.size(); ++i) {
		sum += prevLayer[i].getOutputVal() * prevLayer[i].outputWeights[idx].weight;
	}
	outputVal = Neuron::transfer(sum);
}
