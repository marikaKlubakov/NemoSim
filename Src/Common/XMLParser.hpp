#pragma once

#include "tinyxml2.h"
#include <vector>
#include <string>
#include "LIFNetwork.hpp"
class XMLParser
{
public:
	static bool parse(const std::string& filename, NetworkParameters& params);
	static void LIFNetworkParser(tinyxml2::XMLElement* LIF, tinyxml2::XMLElement* arch,NetworkParameters& params);
	static void BIUNetworkParser(tinyxml2::XMLElement* BIU, tinyxml2::XMLElement* arch,NetworkParameters& params);
	static void YFlashParser(tinyxml2::XMLElement* YFlash ,NetworkParameters& params);
	static Config parseConfigFromFile(const std::string& filePath);

};
