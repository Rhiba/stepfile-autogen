#include "nn.h"


int main(int argc, char *argv[]) {
	std::vector<int> topology;
	topology.push_back(10); //2 inputs
	topology.push_back(1); //2 hidden nodes
	topology.push_back(1); //1 output
	if (argc != 2 && argc != 3) {
		std::cout << "Usage: ./nn <data_file> [optional_weights_file]" << std::endl;
		return 1;
	}
	std::vector<std::vector <Connection> > fileWeights;
	Net nn(topology);
	if (argc == 3) {
		int pass = 0;
		std::ifstream ifile(argv[2]);
		std::string s((std::istreambuf_iterator<char>(ifile)), std::istreambuf_iterator<char>());
		std::stringstream ss(s);
		std::string token;
		while (std::getline(ss,token,'\n')) {
			std::vector<Connection> conns;
			std::stringstream ss3(token);
			std::string tok2;
			while (std::getline(ss3,tok2,',')) {
				Connection conn;
				std::stringstream ss2(tok2);
				std::string tok;
				int c = 0;
				while (std::getline(ss2,tok,':')) {
					if (c == 0) {
						conn.weight = std::stod(tok);
						c++;
					} else {
						conn.deltaWeight = std::stod(tok);
					}
				}
				conns.push_back(conn);
			}
			fileWeights.push_back(conns);
		}
		nn = Net(topology,fileWeights);
	}
	std::ifstream infile(argv[1]);
	std::string line;
	double av_err = 0;
	double av_count = 0;
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
		if (argc == 2) {
			nn.back(targetVals);
		}
		std::cout << "Recent err: " << nn.getRecentAvErr() << std::endl << std::endl;
		av_err += nn.getRecentAvErr();
		av_count++;
	}
	av_err /= av_count;
	std::cout << "\nAverage err: " << av_err << std::endl;
	
	/*
	std::vector< std::vector< Connection > > weights = nn.getCurrentWeights();
	std::string output = "";
	for (int i = 0; i < weights.size(); i++) {
		for (int j = 0; j < weights[i].size(); j++) {
			output += std::to_string(weights[i][j].weight);
			output += ":" + std::to_string(weights[i][j].deltaWeight);
			if (j + 1 < weights[i].size()) {
				output += ",";
			}
		}
		if (i + 1 < weights.size()) {
			output += "\n";
		}
	}
	std::cout << output;
	std::ofstream outfile("net_weights.txt");
	outfile << output;
	*/
	//TESTING THAT SAVING WEIGHTS WORKS
	/*
	Net nnn(topology,weights);
	std::ifstream infile2("songdata2.txt");
	std::string line2;
	double av_err2 = 0;
	double av_count2 = 0;
	int pass2 = 0;
	while (std::getline(infile2,line2)) {
		pass2++;
		size_t pos2 = 0;
		std::string token2;
		int counter2 = 0;
		std::vector<double> inputVals2;
		std::vector<double> resultsVals2;
		std::vector<double> targetVals2;
		while ((pos2 = line2.find(' ')) != std::string::npos) {
			token2 = line2.substr(0, pos2);
			if (counter2 == 0) {
				targetVals2.push_back(std::stod(token2));
			} else {
				inputVals2.push_back(std::stod(token2));
			}
			counter2++;
			line2.erase(0, pos2 + 1);
		}
		std::cout << "size of input list: " << inputVals2.size() << std::endl;
		std::cout << "Pass: " << pass2 << std::endl;
		nnn.forward(inputVals2);
		std::cout << "Target: " << (targetVals2[0]*(20-1) + 1) << std::endl;
		nnn.results(resultsVals2);
		std::cout << "Actual: " << (resultsVals2[0]*(20-1) + 1) << std::endl;
		std::cout << "Recent err: " << nnn.getRecentAvErr() << std::endl << std::endl;
		av_err2 += nnn.getRecentAvErr();
		av_count2++;
	}
	av_err2 /= av_count2;
	std::cout << "\nAverage err: " << av_err2 << std::endl;
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

Net::Net(const std::vector<int> &topology, std::vector< std::vector< Connection > > &listOfConnections) { //checked
	int numLayers = topology.size();
	//iterate through layers
	int conn_count = 0;
	for (int i = 0; i < numLayers; ++i) {
		layers.push_back(Layer());
		int numOutputs = i == topology.size() - 1 ? 0 : topology[i+1];
		//iterate through neurons
		for (int j = 0; j <= topology[i]; ++j) {
			if (conn_count < listOfConnections.size()) {
				layers.back().push_back(Neuron(numOutputs,j,listOfConnections.at(conn_count)));
			} else {
				layers.back().push_back(Neuron(numOutputs,j));
			}
			conn_count++;
		}
		layers.back().back().setOutputVal(1.0);
	}
}
std::vector< std::vector < Connection > > Net::getCurrentWeights() {
	std::vector< std::vector <Connection > > toReturn;
	for (int i = 0; i < layers.size(); i++) {
		for (int j = 0; j < layers[i].size(); j++) {
			toReturn.push_back(layers[i][j].getWeights());
		}
	}
	return toReturn;
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
Neuron::Neuron(int numOutputs, int inIdx, std::vector<Connection> &weights) {
	for (int i = 0; i < numOutputs; ++i) {
		outputWeights.push_back(weights.at(i));
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
double Neuron::alpha = 0.3;
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
