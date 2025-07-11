#include "XMLParser.hpp"
#include <tinyxml2.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

bool XMLParser::parse(const std::string& filename, NetworkParameters& params) {
   using namespace tinyxml2;
   XMLDocument doc;
   if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
       std::cerr << "Error loading XML file: " << doc.ErrorStr() << std::endl;
       return false;
   }
   auto* root = doc.FirstChildElement("NetworkConfig");
   if (!root) {
       std::cerr << "Error: No <NetworkConfig> root element found." << std::endl;
       return false;
   }
   const char* networkType = root->Attribute("type");
   if (!networkType) {
       std::cerr << "Error: Network type attribute not found in <NetworkConfig>." << std::endl;
       return false;
   }
   params.networkType = StringToNetworkType.at(networkType);
   std::cout << "Parsing network type: " << networkType << std::endl;
   // --- LIFNetwork parameters ---
   auto* LIF = root->FirstChildElement("LIFNetwork");
   auto* arch = root->FirstChildElement("Architecture");
   if (LIF) {
       LIFNetworkParser(LIF, arch, params);
   }

   // --- BIUNetwork parameters ---
   auto* BIU = root->FirstChildElement("BIUNetwork");
   if (BIU) {
	   BIUNetworkParser(BIU, arch, params);
   }


   if (arch)
   {
	   auto* YFlash = arch->FirstChildElement("YFlash");
	   if (arch && YFlash)
	   {
		   YFlashParser(YFlash, params);
	   }
   }
   // Debug prints
   std::cout << "\nParsed Parameters:\n";
   std::cout << "Cm = " << params.Cm << "\n";
   std::cout << "Cf = " << params.Cf << "\n";
   std::cout << "VDD = " << params.VDD << "\n";
   std::cout << "VTh = " << params.VTh << "\n";
   std::cout << "dt = " << params.dt << "\n";
   std::cout << "IR = " << params.IR << "\n";
   std::cout << "fclk = " << params.fclk << "\n";
   std::cout << "RLeak = " << params.Rleak << "\n";
   std::cout << "Cn = " << params.Cn << "\n";
   std::cout << "Cu = " << params.Cu << "\n";
   std::cout << "Parsing completed for " << networkType << " network.\n";
   return true;
}

void XMLParser::LIFNetworkParser(tinyxml2::XMLElement* LIF, tinyxml2::XMLElement* arch, NetworkParameters& params)
{
	if (LIF->FirstChildElement("Cm") && LIF->FirstChildElement("Cm")->QueryDoubleText(&params.Cm) != tinyxml2::XML_SUCCESS)
		std::cerr << "Warning: <Cm> missing or invalid in LIFNetwork\n";
	if (LIF->FirstChildElement("Cf") && LIF->FirstChildElement("Cf")->QueryDoubleText(&params.Cf) != tinyxml2::XML_SUCCESS)
		std::cerr << "Warning: <Cf> missing or invalid in LIFNetwork\n";
	if (LIF->FirstChildElement("VDD") && LIF->FirstChildElement("VDD")->QueryDoubleText(&params.VDD) != tinyxml2::XML_SUCCESS)
		std::cerr << "Warning: <VDD> missing or invalid in LIFNetwork\n";
	if (LIF->FirstChildElement("VTh") && LIF->FirstChildElement("VTh")->QueryDoubleText(&params.VTh) != tinyxml2::XML_SUCCESS)
		std::cerr << "Warning: <VTh> missing or invalid in LIFNetwork\n";
	if (LIF->FirstChildElement("dt") && LIF->FirstChildElement("dt")->QueryDoubleText(&params.dt) != tinyxml2::XML_SUCCESS)
		std::cerr << "Warning: <dt> missing or invalid in LIFNetwork\n";
	if (LIF->FirstChildElement("IR") && LIF->FirstChildElement("IR")->QueryDoubleText(&params.IR) != tinyxml2::XML_SUCCESS)
		std::cerr << "Warning: <IR> missing or invalid in LIFNetwork\n";

	// For LIFNetwork Architecture
	if (arch && params.networkType == NetworkTypes::LIFNetworkType) {
		for (auto* layer = arch->FirstChildElement("Layer"); layer != nullptr; layer = layer->NextSiblingElement("Layer")) {
			int size;
			if (layer->QueryIntAttribute("size", &size) == tinyxml2::XML_SUCCESS)
				params.layerSizes.push_back(size);
			else
				std::cerr << "Warning: Missing or invalid 'size' in LIF <Layer>\n";
		}
	}
}

void XMLParser::BIUNetworkParser(tinyxml2::XMLElement* BIU, tinyxml2::XMLElement* arch, NetworkParameters& params)
{
	if (BIU->FirstChildElement("fclk") && BIU->FirstChildElement("fclk")->QueryDoubleText(&params.fclk) != tinyxml2::XML_SUCCESS)
		std::cerr << "Warning: <fclk> missing or invalid in BIUNetwork\n";
	if (BIU->FirstChildElement("VDD") && BIU->FirstChildElement("VDD")->QueryDoubleText(&params.VDD) != tinyxml2::XML_SUCCESS)
		std::cerr << "Warning: <VDD> missing or invalid in BIUNetwork\n";
	if (BIU->FirstChildElement("VTh") && BIU->FirstChildElement("VTh")->QueryDoubleText(&params.VTh) != tinyxml2::XML_SUCCESS)
		std::cerr << "Warning: <VTh> missing or invalid in BIUNetwork\n";
	if (BIU->FirstChildElement("Cn") && BIU->FirstChildElement("Cn")->QueryDoubleText(&params.Cn) != tinyxml2::XML_SUCCESS)
		std::cerr << "Warning: <Cn> missing or invalid in BIUNetwork\n";
	if (BIU->FirstChildElement("Cu") && BIU->FirstChildElement("Cu")->QueryDoubleText(&params.Cu) != tinyxml2::XML_SUCCESS)
		std::cerr << "Warning: <Cu> missing or invalid in BIUNetwork\n";
	if (BIU->FirstChildElement("RLeak") && BIU->FirstChildElement("RLeak")->QueryDoubleText(&params.Rleak) != tinyxml2::XML_SUCCESS)
		std::cerr << "Warning: <RLeak> missing or invalid in BIUNetwork\n";
	if (BIU->FirstChildElement("refractory") && BIU->FirstChildElement("refractory")->QueryDoubleText(&params.refractory) != tinyxml2::XML_SUCCESS)
		std::cerr << "Warning: <refractory> missing or invalid in BIUNetwork\n";

	if (arch && params.networkType == NetworkTypes::BIUNetworkType) {
		for (auto* layer = arch->FirstChildElement("Layer"); layer != nullptr; layer = layer->NextSiblingElement("Layer")) {
			int size;
			if (layer->QueryIntAttribute("size", &size) == tinyxml2::XML_SUCCESS)
				params.layerSizes.push_back(size);
			else
				std::cerr << "Warning: Missing or invalid 'size' attribute in <Layer>\n";
			auto* synapses = layer->FirstChildElement("synapses");
			if (synapses) {
				auto* weightsElem = synapses->FirstChildElement("weights");
				std::vector<std::vector<double>> layerWeights;
				if (weightsElem) {
					for (auto* rowElem = weightsElem->FirstChildElement("row"); rowElem != nullptr; rowElem = rowElem->NextSiblingElement("row")) {
						const char* rowText = rowElem->GetText();
						if (!rowText) {
							std::cerr << "Warning: Empty <row> in weights\n";
							continue;
						}
						std::istringstream iss(rowText);
						std::vector<double> rowWeights;
						double w;
						while (iss >> w)
							rowWeights.push_back(w);
						layerWeights.push_back(rowWeights);
					}
				}
				else {
					std::cerr << "Warning: <weights> element missing in <synapses>\n";
				}
				params.allWeights.push_back(layerWeights);
			}
			else {
				std::cerr << "Warning: <synapses> missing in <Layer>\n";
			}
		}
	}
}

void XMLParser::YFlashParser(tinyxml2::XMLElement* YFlash, NetworkParameters& params)
{
	auto* weightsElem = YFlash->FirstChildElement("weights");
	std::vector<std::vector<double>> layerWeights;
	if (weightsElem) {
		for (auto* rowElem = weightsElem->FirstChildElement("row"); rowElem != nullptr; rowElem = rowElem->NextSiblingElement("row")) {
			const char* rowText = rowElem->GetText();
			if (!rowText) {
				std::cerr << "Warning: Empty <row> in weights\n";
				continue;
			}
			std::istringstream iss(rowText);
			std::vector<double> rowWeights;
			double w;
			while (iss >> w)
				rowWeights.push_back(w);
			layerWeights.push_back(rowWeights);
		}
	}
	else {
		std::cerr << "Warning: <weights> element missing in <synapses>\n";
	}
	params.YFlashWeights.push_back(layerWeights);
}


static std::string trim(const std::string& str)
{
	const char* whitespace = " \t\n\r\"";
	size_t start = str.find_first_not_of(whitespace);
	size_t end = str.find_last_not_of(whitespace);
	return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}


static ConfigKey getConfigKey(const std::string& key) 
{
	static const std::unordered_map<std::string, ConfigKey> keyMap = 
	{
		{"output_directory", ConfigKey::OutputDirectory},
		{"xml_config_path", ConfigKey::XmlConfigPath},
		{"sup_xml_config_path", ConfigKey::SupXmlConfigPath},
		{"data_input_file", ConfigKey::DataInputFile},
		{"progress_interval_seconds", ConfigKey::ProgressIntervalSeconds}
	};

	auto it = keyMap.find(key);
	return it != keyMap.end() ? it->second : ConfigKey::Unknown;
}

Config XMLParser::parseConfigFromFile(const std::string& filePath)
{
	Config config;
	std::ifstream file(filePath);
	if (!file.is_open()) 
	{
		throw std::runtime_error("Unable to open file: " + filePath);
	}

	std::string line;
	while (std::getline(file, line)) 
	{
		size_t colon = line.find(':');
		if (colon == std::string::npos) continue;

		std::string key = trim(line.substr(0, colon));

		// Skip whitespace after colon
		size_t start = line.find_first_not_of(" \t", colon + 1);

		std::string value;
		if (line[start] == '"') 
		{
			// Value is a quoted string
			size_t end = line.find('"', start + 1);
			value = line.substr(start + 1, end - start - 1);
		}
		else 
		{
			// Value is a number or unquoted
			size_t end = line.find_first_of(", \t\r\n", start);
			value = line.substr(start, end - start);
		}



		switch (getConfigKey(key))
		{
		case ConfigKey::OutputDirectory:
			config.outputDirectory = value;
			break;
		case ConfigKey::XmlConfigPath:
			config.xmlConfigPath = value;
			break;
		case ConfigKey::SupXmlConfigPath:
			config.supXmlConfigPath = value;
			break;
		case ConfigKey::DataInputFile:
			config.dataInputPath = value;
			break;
		case ConfigKey::ProgressIntervalSeconds:
			config.progressIntervalSeconds = std::stoi(value);
			break;
		default:
			std::cerr << "Unknown config key: " << key << std::endl;
			break;
		}
	}
	return config;

}
