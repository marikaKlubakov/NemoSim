#include "XMLParser.hpp"
#include <tinyxml2.h>
#include <iostream>
#include <sstream>
#include <vector>
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
   params.networkType = networkType;
   std::cout << "Parsing network type: " << networkType << std::endl;
   // --- LIFNetwork parameters ---
   auto* LIF = root->FirstChildElement("LIFNetwork");
   if (LIF) {
       if (LIF->FirstChildElement("Cm") && LIF->FirstChildElement("Cm")->QueryDoubleText(&params.Cm) != XML_SUCCESS)
           std::cerr << "Warning: <Cm> missing or invalid in LIFNetwork\n";
       if (LIF->FirstChildElement("Cf") && LIF->FirstChildElement("Cf")->QueryDoubleText(&params.Cf) != XML_SUCCESS)
           std::cerr << "Warning: <Cf> missing or invalid in LIFNetwork\n";
       if (LIF->FirstChildElement("VDD") && LIF->FirstChildElement("VDD")->QueryDoubleText(&params.VDD) != XML_SUCCESS)
           std::cerr << "Warning: <VDD> missing or invalid in LIFNetwork\n";
       if (LIF->FirstChildElement("VTh") && LIF->FirstChildElement("VTh")->QueryDoubleText(&params.VTh) != XML_SUCCESS)
           std::cerr << "Warning: <VTh> missing or invalid in LIFNetwork\n";
       if (LIF->FirstChildElement("dt") && LIF->FirstChildElement("dt")->QueryDoubleText(&params.dt) != XML_SUCCESS)
           std::cerr << "Warning: <dt> missing or invalid in LIFNetwork\n";
       if (LIF->FirstChildElement("IR") && LIF->FirstChildElement("IR")->QueryDoubleText(&params.IR) != XML_SUCCESS)
           std::cerr << "Warning: <IR> missing or invalid in LIFNetwork\n";
   }
   // --- BIUNetwork parameters ---
   auto* BIU = root->FirstChildElement("BIUNetwork");
   if (BIU) {
       if (BIU->FirstChildElement("fclk") && BIU->FirstChildElement("fclk")->QueryDoubleText(&params.fclk) != XML_SUCCESS)
           std::cerr << "Warning: <fclk> missing or invalid in BIUNetwork\n";
       if (BIU->FirstChildElement("VDD") && BIU->FirstChildElement("VDD")->QueryDoubleText(&params.VDD) != XML_SUCCESS)
           std::cerr << "Warning: <VDD> missing or invalid in BIUNetwork\n";
       if (BIU->FirstChildElement("VTh") && BIU->FirstChildElement("VTh")->QueryDoubleText(&params.VTh) != XML_SUCCESS)
           std::cerr << "Warning: <VTh> missing or invalid in BIUNetwork\n";
       if (BIU->FirstChildElement("Cn") && BIU->FirstChildElement("Cn")->QueryDoubleText(&params.Cn) != XML_SUCCESS)
           std::cerr << "Warning: <Cn> missing or invalid in BIUNetwork\n";
       if (BIU->FirstChildElement("Cu") && BIU->FirstChildElement("Cu")->QueryDoubleText(&params.Cu) != XML_SUCCESS)
           std::cerr << "Warning: <Cu> missing or invalid in BIUNetwork\n";
       if (BIU->FirstChildElement("RLeak") && BIU->FirstChildElement("RLeak")->QueryDoubleText(&params.Rleak) != XML_SUCCESS)
           std::cerr << "Warning: <RLeak> missing or invalid in BIUNetwork\n";
       if (BIU->FirstChildElement("refractory") && BIU->FirstChildElement("refractory")->QueryDoubleText(&params.refractory) != XML_SUCCESS)
           std::cerr << "Warning: <refractory> missing or invalid in BIUNetwork\n";
   }
   // --- Architecture Parsing ---
   auto* arch = root->FirstChildElement("Architecture");
   if (arch && strcmp(networkType, "BIUNetwork") == 0) {
       for (auto* layer = arch->FirstChildElement("Layer"); layer != nullptr; layer = layer->NextSiblingElement("Layer")) {
           int size;
           if (layer->QueryIntAttribute("size", &size) == XML_SUCCESS)
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
               } else {
                   std::cerr << "Warning: <weights> element missing in <synapses>\n";
               }
               params.allWeights.push_back(layerWeights);
           } else {
               std::cerr << "Warning: <synapses> missing in <Layer>\n";
           }
       }
   }
   // For LIFNetwork Architecture
   if (arch && strcmp(networkType, "LIFNetwork") == 0) {
       for (auto* layer = arch->FirstChildElement("Layer"); layer != nullptr; layer = layer->NextSiblingElement("Layer")) {
           int size;
           if (layer->QueryIntAttribute("size", &size) == XML_SUCCESS)
               params.layerSizes.push_back(size);
           else
               std::cerr << "Warning: Missing or invalid 'size' in LIF <Layer>\n";
       }
   }

   auto* YFlash = arch->FirstChildElement("YFlash");
   if (arch && YFlash)
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