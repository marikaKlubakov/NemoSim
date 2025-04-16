#include "XMLParser.hpp"
#include <tinyxml2.h>
#include <iostream>

bool XMLParser::parse(const std::string& filename, NetworkParameters& params) {
	using namespace tinyxml2;
	XMLDocument doc;
	if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
		std::cerr << "Error loading XML file: " << doc.ErrorStr() << std::endl;
		return false;
	}
	auto* root = doc.FirstChildElement("NetworkConfig");
	if (!root) return false;

	// Parse Global
	auto* global = root->FirstChildElement("Global");
	params.Cm = std::stod(global->FirstChildElement("Cm")->GetText());
	params.Cf = std::stod(global->FirstChildElement("Cf")->GetText());
	params.VDD = std::stod(global->FirstChildElement("VDD")->GetText());
	params.VTh = std::stod(global->FirstChildElement("VTh")->GetText());
	params.dt = std::stod(global->FirstChildElement("dt")->GetText());

	// Parse YFlash
	auto* yflash = root->FirstChildElement("YFlash");
	params.gm = std::stod(yflash->FirstChildElement("gm")->GetText());
	params.req = std::stod(yflash->FirstChildElement("req")->GetText());
	params.CGBr = std::stod(yflash->FirstChildElement("CGBr")->GetText());
	params.CGBi = std::stod(yflash->FirstChildElement("CGBi")->GetText());
	params.CGSr = std::stod(yflash->FirstChildElement("CGSr")->GetText());
	params.CGSi = std::stod(yflash->FirstChildElement("CGSi")->GetText());
	params.CGDr = std::stod(yflash->FirstChildElement("CGDr")->GetText());
	params.CGDi = std::stod(yflash->FirstChildElement("CGDi")->GetText());
	params.CDBr = std::stod(yflash->FirstChildElement("CDBr")->GetText());
	params.CDBi = std::stod(yflash->FirstChildElement("CDBi")->GetText());
	params.CMOS = std::stod(yflash->FirstChildElement("CMOS")->GetText());

	// Parse Architecture
	auto* arch = root->FirstChildElement("Architecture");
	for (auto* layer = arch->FirstChildElement("Layer"); layer != nullptr; layer = layer->NextSiblingElement("Layer")) {
		int size;
		layer->QueryIntAttribute("size", &size);
		params.layerSizes.push_back(size);
	}
	return true;
}
