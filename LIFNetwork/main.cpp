#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <cmath>

#include <windows.h>

#include <corecrt_math_defines.h>
#include "LIFNetwork.hpp"
#include "../XMLParser.hpp"

// --------- Main Simulation ---------
int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <XML configuration file>" << std::endl;
		return 1;
	}

	std::string xmlFile = argv[1];
	NetworkParameters params;
	XMLParser parser;
	MessageBox(NULL, "Hello, World!", "Message Box", MB_OK);
	// Parse the XML file to get network parameters
	if (!parser.parse(xmlFile, params)) {
		std::cerr << "Failed to parse network configuration." << std::endl;
		return 1;
	}

	LIFNetwork* network = LIFEngine::createNetwork(params);
	if (!network) return 1;

	// Parameters for sine wave
	double frequency = 50;  // Hz
	double amplitude = 100e-12; // 100 pA
	double timeStep = 0.0001; // Smaller time step for better resolution
	double time = 0.0;
	int numSteps = 20000; // Number of steps to cover 2 seconds

	// Simulate with sine wave input
	for (int t = 0; t < numSteps; ++t)
	{
		std::vector<double> input(params.layerSizes[0]);
		for (size_t i = 0; i < input.size(); ++i)
		{
			input[i] = amplitude + amplitude * std::sin(2 * M_PI * frequency * time + M_PI * 1.5);
		}
		network->feedForward(input);

		time += timeStep;
	}
	network->printNetworkToFile();
	delete network;


	// Run the Python script
// 	int result = system("python LIFNetwork/plot_vm_to_dt.py Iins.txt vms.txt Vouts.txt");
// 	if (result != 0) {
// 		std::cerr << "Failed to run Python script." << std::endl;
// 		return 1;
// 	}


// 	const char* files[] = { "Iins.txt", "vms.txt", "Vouts.txt" };
// 	for (const char* file : files) {
// 		if (remove(file) == 0) {
// 			std::cout << "Deleted " << file << " successfully." << std::endl;
// 		}
// 		else {
// 			std::cerr << "Error deleting " << file << std::endl;
// 		}
// 	}

	return 0;
}
