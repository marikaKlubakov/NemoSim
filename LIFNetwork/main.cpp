#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <cmath>

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include <windows.h>

#include <corecrt_math_defines.h>
#include "LIFNetwork.hpp"
#include "../XMLParser.hpp"

// --------- Main Simulation ---------
int main(int argc, char* argv[])
{
	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " <XML configuration file>" << " <data input file>" << std::endl;
		return 1;
	}

	std::string xmlFile = argv[1];
	std::string dataInputFile = argv[2];
	NetworkParameters params;
	XMLParser parser;
	//MessageBox(NULL, "Hello, World!", "Message Box", MB_OK);
	// Parse the XML file to get network parameters
	if (!parser.parse(xmlFile, params)) {
		std::cerr << "Failed to parse network configuration." << std::endl;
		return 1;
	}

	LIFNetwork* network = LIFEngine::createNetwork(params);
	if (!network) return 1;


	//std::ofstream outFile("sine_wave_input.txt");

	std::ifstream inputFile(dataInputFile);
		std::string line;

		if (inputFile.is_open()) {
		while (std::getline(inputFile, line)) {
		double value = std::stod(line); // Convert string to double
		std::vector<double> input;
		input.push_back(value);
		network->feedForward(input);
		}
		inputFile.close();
		} else {
		std::cerr << "Unable to open file" << std::endl;
		}


		inputFile.close();

	network->printNetworkToFile();
	delete network;
	return 0;
}
