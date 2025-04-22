#include "XMLParser.hpp"
#include <tinyxml2.h>
#include <iostream>
#include <unordered_map>

bool XMLParser::parse(const std::string& filename, NetworkParameters& params) {
    using namespace tinyxml2;
    XMLDocument doc;

    // Load XML file
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "Error loading XML file: " << doc.ErrorStr() << std::endl;
        return false;
    }

    auto* root = doc.FirstChildElement("NetworkConfig");
    if (!root) {
        std::cerr << "Error: No root element found in XML." << std::endl;
        return false;
    }

    // Identify network type dynamically
    const char* networkType = root->Attribute("type");
    if (!networkType) {
        std::cerr << "Error: Network type not specified." << std::endl;
        return false;
    }

    std::cout << "Parsing network type: " << networkType << std::endl;

    // Use a map to store parameters dynamically
    std::unordered_map<std::string, double> parsedParams;

    // Parse Global Parameters (common across networks)
    auto* global = root->FirstChildElement("Global");
    if (global) {
        for (auto* elem = global->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement()) {
            parsedParams[elem->Name()] = std::stod(elem->GetText());
        }
    }

    // Parse Network-Specific Parameters
    auto* networkConfig = root->FirstChildElement(networkType);
    if (networkConfig) {
        for (auto* elem = networkConfig->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement()) {
            parsedParams[elem->Name()] = std::stod(elem->GetText());
        }
    }
    else {
        std::cerr << "Error: No configuration found for network type " << networkType << std::endl;
        return false;
    }

    // Parse Architecture (Layer Sizes)
    auto* arch = root->FirstChildElement("Architecture");
    if (arch) {
        for (auto* layer = arch->FirstChildElement("Layer"); layer != nullptr; layer = layer->NextSiblingElement("Layer")) {
            int size;
            layer->QueryIntAttribute("size", &size);
            params.layerSizes.push_back(size);
        }
    }

    // Print parsed parameters for debugging
    std::cout << "Parsed Parameters:\n";
    for (const std::pair<const std::string, double>& param : parsedParams) {
        std::cout << param.first << " = " << param.second << std::endl;
    }


    std::cout << "Parsing completed for " << networkType << " network." << std::endl;
    return true;
}
