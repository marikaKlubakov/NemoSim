#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <cmath>
#include <fstream>
#include "XMLParser.hpp"
#include "NEMOEngine.hpp"

// --------- Main Simulation ---------
int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cerr << "Usage: " << argv[0] << " <XML configuration file>" << " <data input file>" << " optional <supervisor XML configuration file>" << std::endl;
		return 1;
	}

	std::string xmlFile = argv[1];
	std::string dataInputFile = argv[2];


	NetworkParameters params;
	XMLParser parser;

	// Parse the XML file to get network parameters
	if (!parser.parse(xmlFile, params))
	{
		std::cerr << "Failed to parse network configuration." << std::endl;
		return 1;
	}

	if (argc == 4)
	{
		std::string supervisorXml = argv[3];
		if (!parser.parse(supervisorXml, params))
		{
			std::cerr << "Failed to parse network configuration." << std::endl;
			return 1;
		}
	}

	NEMOEngine NemoEngine(params);

	std::ifstream inputFile(dataInputFile);
	NemoEngine.runEngine(inputFile);

	return 0;
}
