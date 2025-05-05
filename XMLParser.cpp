#include "XMLParser.hpp"
#include <tinyxml2.h>
#include <iostream>
#include <unordered_map>

bool XMLParser::parse(const std::string& filename, NetworkParameters& params)
{
    using namespace tinyxml2;
    XMLDocument doc;

    // Load XML file
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS)
    {
        std::cerr << "Error loading XML file: " << doc.ErrorStr() << std::endl;
        return false;
    }

    auto* root = doc.FirstChildElement("NetworkConfig");
    if (!root)
    {
        std::cerr << "Error: No root element found in XML." << std::endl;
        return false;
    }

    // Identify network type dynamically
    const char* networkType = root->Attribute("type");
    if (!networkType)
    {
        std::cerr << "Error: Network type not specified." << std::endl;
        return false;
    }

    std::cout << "Parsing network type: " << networkType << std::endl;

    // Parse Global Parameters (common across networks)
    auto* global = root->FirstChildElement("Global");
    if (global)
    {
        global->FirstChildElement("Cm")->QueryDoubleText(&params.Cm);
        global->FirstChildElement("Cf")->QueryDoubleText(&params.Cf);
        global->FirstChildElement("VDD")->QueryDoubleText(&params.VDD);
        global->FirstChildElement("VTh")->QueryDoubleText(&params.VTh);
        global->FirstChildElement("dt")->QueryDoubleText(&params.dt);
        global->FirstChildElement("IR")->QueryDoubleText(&params.IR);
    }

    // Parse Architecture (Layer Sizes)
    auto* arch = root->FirstChildElement("Architecture");
    if (arch)
    {
        for (auto* layer = arch->FirstChildElement("Layer"); layer != nullptr; layer = layer->NextSiblingElement("Layer"))
        {
            int size;
            layer->QueryIntAttribute("size", &size);
            params.layerSizes.push_back(size);
        }
    }

    // Print parsed parameters for debugging
    std::cout << "Parsed Parameters:\n";
    std::cout << "Cm = " << params.Cm << "\n";
    std::cout << "Cf = " << params.Cf << "\n";
    std::cout << "VDD = " << params.VDD << "\n";
    std::cout << "VTh = " << params.VTh << "\n";
    std::cout << "dt = " << params.dt << "\n";
    std::cout << "gm = " << params.gm << "\n";
    std::cout << "req = " << params.req << "\n";
    std::cout << "CGBr = " << params.CGBr << "\n";
    std::cout << "CGBi = " << params.CGBi << "\n";
    std::cout << "CGSr = " << params.CGSr << "\n";
    std::cout << "CGSi = " << params.CGSi << "\n";
    std::cout << "CGDr = " << params.CGDr << "\n";
    std::cout << "CGDi = " << params.CGDi << "\n";
    std::cout << "CDBr = " << params.CDBr << "\n";
    std::cout << "CDBi = " << params.CDBi << "\n";
    std::cout << "CMOS = " << params.CMOS << "\n";
    for (const int& size : params.layerSizes)
    {
        std::cout << "Layer size = " << size << "\n";
    }

    std::cout << "Parsing completed for " << networkType << " network." << std::endl;
    return true;
}

