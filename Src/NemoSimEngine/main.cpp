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
#define MAX_PATH          260


#include <sys/stat.h>
#include <sys/types.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

bool directoryExists(const std::string& path) {
#ifdef _WIN32
	DWORD attribs = GetFileAttributes(path.c_str());
	return (attribs != INVALID_FILE_ATTRIBUTES) && (attribs & FILE_ATTRIBUTE_DIRECTORY);
#else
	struct stat info;
	return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
#endif
}

bool createDirectory(const std::string& path) {
#ifdef _WIN32
	return CreateDirectory(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
#else
	return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
#endif
}

bool changeWorkingDirectory(const std::string& path) {
	if (!directoryExists(path)) {
		if (!createDirectory(path)) {
			return false;
		}
	}

#ifdef _WIN32
	return SetCurrentDirectory(path.c_str());
#else
	return chdir(path.c_str()) == 0;
#endif
}




bool RetrieveNetworkParamsFromXML(XMLParser* parser, NetworkParameters* params, Config& config)
{
	if (!parser->parse(config.xmlConfigPath.c_str(), *params))
	{
		std::cerr << "Failed to parse network configuration." << std::endl;
		return false;
	}

	if (!config.supXmlConfigPath.empty())
	{
		if (!parser->parse(config.supXmlConfigPath.c_str(), *params))
		{
			std::cerr << "Failed to parse network configuration." << std::endl;
			return false;
		}
	}

	if (!config.neuronEnergyCsvPath.empty())
	{
		params->neuronEnergyCsvPath = config.neuronEnergyCsvPath;
	}

	if (!config.synapsesEnergyCsvPath.empty())
	{
		params->synapsesEnergyCsvPath = config.synapsesEnergyCsvPath;
	}

	params->verbosity = config.verbosity;
	return true;
}

// --------- Main Simulation ---------
int main(int argc, char* argv[])
{
	try {
		if (argc < 2)
		{
			std::cerr << "Usage: " << argv[0] << " <json configuration file>" << std::endl;
			return 1;
		}

		NetworkParameters params;
		XMLParser parser;

		//parse json file
		Config config = parser.parseConfigFromFile(argv[1]);

		// Validate configuration paths
		if (config.xmlConfigPath.empty())
		{
			std::cerr << "Configuration Error: XML configuration path is empty. Please specify 'XmlConfigPath' in the JSON config." << std::endl;
			return 1;
		}

		if (config.dataInputPath.empty())
		{
			std::cerr << "Configuration Error: Data input path is empty. Please specify 'DataInputFile' in the JSON config." << std::endl;
			return 1;
		}

		// Parse the XML file to get network parameters
		bool succeed = RetrieveNetworkParamsFromXML(&parser, &params, config);
		if (!succeed)
		{
			return 1;
		}

		NEMOEngine NemoEngine(params);

		std::ifstream inputFile(config.dataInputPath);
		if (!inputFile.is_open()) {
		    std::cerr << "Input Data Error: Failed to open input data file: " << config.dataInputPath << std::endl;
		    return 1;
		}
		//change working directory
		if (!changeWorkingDirectory(config.outputDirectory)) {
		    std::cerr << "Failed to change working directory to: " << config.outputDirectory << std::endl;
		    return 1;
		}
		NemoEngine.runEngine(inputFile);
	} catch (const std::exception& ex) {
        std::cerr << "Standard exception caught: " << ex.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception caught." << std::endl;
    }
    return 0;
}


