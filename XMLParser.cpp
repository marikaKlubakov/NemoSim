#include "XMLParser.hpp"
#include <tinyxml2.h>
#include <iostream>
#include <sstream>
#include <vector>
bool XMLParser::parse(const std::string& filename, NetworkParameters& params)
{
	using namespace tinyxml2;
	XMLDocument doc;
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
	const char* networkType = root->Attribute("type");
	if (!networkType)
	{
		std::cerr << "Error: Network type not specified." << std::endl;
		return false;
	}
	params.networkType = networkType;
	std::cout << "Parsing network type: " << networkType << std::endl;
	// Keep LIFNetwork parser unchanged
	auto* LIF = root->FirstChildElement("LIFNetwork");
	if (LIF)
	{
		LIF->FirstChildElement("Cm")->QueryDoubleText(&params.Cm);
		LIF->FirstChildElement("Cf")->QueryDoubleText(&params.Cf);
		LIF->FirstChildElement("VDD")->QueryDoubleText(&params.VDD);
		LIF->FirstChildElement("VTh")->QueryDoubleText(&params.VTh);
		LIF->FirstChildElement("dt")->QueryDoubleText(&params.dt);
		LIF->FirstChildElement("IR")->QueryDoubleText(&params.IR);
	}//handle errors
	// BIUNetwork parameters
	auto* BIU = root->FirstChildElement("BIUNetwork");
	if (BIU)
	{
		if (BIU->FirstChildElement("fclk"))
		{
			BIU->FirstChildElement("fclk")->QueryDoubleText(&params.fclk);
		}
		if (BIU->FirstChildElement("VDD"))
		{
			BIU->FirstChildElement("VDD")->QueryDoubleText(&params.VDD);
		}
		if (BIU->FirstChildElement("VTh"))
		{
			BIU->FirstChildElement("VTh")->QueryDoubleText(&params.VTh);
		}
		if (BIU->FirstChildElement("Cn"))
		{
			BIU->FirstChildElement("Cn")->QueryDoubleText(&params.Cn);
		}
		if (BIU->FirstChildElement("Cu"))
		{
			BIU->FirstChildElement("Cu")->QueryDoubleText(&params.Cu);
		}
		
// 		BIU->FirstChildElement("RLeak")->QueryDoubleText(&params.Rleak);
// 		BIU->FirstChildElement("VDD")->QueryDoubleText(&params.VDD);
// 		BIU->FirstChildElement("VTh")->QueryDoubleText(&params.VTh);
// 		BIU->FirstChildElement("Cn")->QueryDoubleText(&params.Cn);
// 		BIU->FirstChildElement("Cu")->QueryDoubleText(&params.Cu);
		BIU->FirstChildElement("refractory")->QueryDoubleText(&params.refractory);
	}
	// Parse Architecture (layer sizes and BIU weights)
	auto* arch = root->FirstChildElement("Architecture");
	if (arch && strcmp(networkType, "BIUNetwork") == 0)
	{
		for (auto* layer = arch->FirstChildElement("Layer"); layer != nullptr; layer = layer->NextSiblingElement("Layer"))
		{
			int size;
			layer->QueryIntAttribute("size", &size);
			params.layerSizes.push_back(size);
			auto* synapses = layer->FirstChildElement("synapses");
			if (synapses)
			{
				std::vector<std::vector<double>> layerWeights;
				auto* weightsElem = synapses->FirstChildElement("weights");
				if (weightsElem)
				{
					for (auto* rowElem = weightsElem->FirstChildElement("row"); rowElem != nullptr; rowElem = rowElem->NextSiblingElement("row"))
					{
						const char* rowText = rowElem->GetText();
						if (!rowText) continue;
						std::istringstream iss(rowText);
						std::vector<double> rowWeights;
						double w;
						while (iss >> w)
							rowWeights.push_back(w);
						layerWeights.push_back(rowWeights);
					}
				}
				params.allWeights.push_back(layerWeights); // Store per-layer matrix
			}
		}
	}

	arch = root->FirstChildElement("Architecture");
	if (arch && strcmp(networkType, "LIFNetwork") == 0)
	{
		for (auto* layer = arch->FirstChildElement("Layer"); layer != nullptr; layer = layer->NextSiblingElement("Layer"))
		{
			int size;
			layer->QueryIntAttribute("size", &size);
			params.layerSizes.push_back(size);
		}
	}
	// Debug output
	std::cout << "Parsed Parameters:\n";
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
// 	for (const int& size : params.layerSizes)
// 	{
// 		std::cout << "Layer size = " << size << "\n";
// 	}
// 	for (const auto& row : params.weights)
// 	{
// 		for (double w : row)
// 			std::cout << w << " ";
// 		std::cout << "\n";
// 	}
	std::cout << "Parsing completed for " << networkType << " network." << std::endl;
	return true;
}