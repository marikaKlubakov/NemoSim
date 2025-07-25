#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <cmath>
#include <fstream>
#include "XMLParser.hpp"
#include "NEMOEngine.hpp"
#include <fstream>
#include <iostream>
#include <string>


#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif
bool changeWorkingDirectory(const std::string& path) {
#ifdef _WIN32
	return SetCurrentDirectory(path.c_str());
#else
	return chdir(path.c_str()) == 0;
#endif
}


std::string getCurrentWorkingDirectory() 
{
	char buffer[MAX_PATH];
#ifdef _WIN32
	GetCurrentDirectory(MAX_PATH, buffer);
#else
	getcwd(buffer, PATH_MAX);
#endif
	return std::string(buffer);
}



bool RetrieveNetworkParamsFromXML(XMLParser* parser, NetworkParameters* params, std::string XMLfilename, std::string SupFilename = nullptr)
{
	if (!parser->parse(XMLfilename, *params))
	{
		std::cerr << "Failed to parse network configuration." << std::endl;
		return false;
	}

	if (!SupFilename.empty())
	{
		if (!parser->parse(SupFilename, *params))
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

	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <json configuration file>" << std::endl;
		return 1;
	}


	
	NetworkParameters params;
	XMLParser parser;

	//parse json file
	Config config = parser.parseConfigFromFile(argv[1]);

	//change working directory
	changeWorkingDirectory(config.outputDirectory);

	// Parse the XML file to get network parameters
	bool succeed = RetrieveNetworkParamsFromXML(&parser, &params, config.xmlConfigPath.c_str(), config.supXmlConfigPath);
	if (!succeed)
	{
		return 1;
	}

	NEMOEngine NemoEngine(params);

	std::ifstream inputFile(config.dataInputPath);
	NemoEngine.runEngine(inputFile);

	return 0;
}


