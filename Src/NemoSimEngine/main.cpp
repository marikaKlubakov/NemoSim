#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <cmath>
#include <fstream>
#include "XMLParser.hpp"
#include "NEMOEngine.hpp"

bool RetrieveNetworkParamsFromXML(XMLParser* parser, NetworkParameters* params, char* XMLfilename, char* SupFilename = nullptr)
{
	const std::string& XMLfile = XMLfilename;
	if (!parser->parse(XMLfile, *params))
	{
		std::cerr << "Failed to parse network configuration." << std::endl;
		return false;
	}

	if (SupFilename != nullptr)
	{
		std::string supervisorXml = SupFilename;
		if (!parser->parse(supervisorXml, *params))
		{
			std::cerr << "Failed to parse network configuration." << std::endl;
			return false;
		}
	}

	return true;
}

// --------- Main Simulation ---------
int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cerr << "Usage: " << argv[0] << " <XML configuration file>" << " <data input file>" << " optional <supervisor XML configuration file>" << std::endl;
		return 1;
	}

	NetworkParameters params;
	XMLParser parser;
	// Parse the XML file to get network parameters

	bool succeed = RetrieveNetworkParamsFromXML(&parser, &params,argv[1], argv[3]);
	if (!succeed)
	{
		return 1;
	}
	NEMOEngine NemoEngine(params);

	std::string dataInputFile = argv[2];
	std::ifstream inputFile(dataInputFile);
	NemoEngine.runEngine(inputFile);

	return 0;
}


