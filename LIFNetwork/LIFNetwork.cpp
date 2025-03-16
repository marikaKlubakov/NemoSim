#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <fstream>
#include "LIFNetwork.h"

//implementation of LIFNetwork class

LIFNetwork::LIFNetwork(const std::vector<int>& layerSizes, double Cm, double Cf, double Vth, double VDD_, double dt_, const std::string& type)
       : m_VDD(VDD_), m_dt(dt_), m_networkType(type)
{
	for (int size : layerSizes)
	{
		m_layers.emplace_back(size, Cm, Cf, Vth, m_VDD, m_dt);
	}
	for (size_t i = 0; i < m_layers.size() - 1; ++i)
	{
		m_layers[i].initializeWeights(m_layers[i + 1].getLayerSize());
	}
}
   void LIFNetwork::feedForward(const std::vector<double>& input) 
   {
       if (input.size() != m_layers[0].getLayerSize()) 
	   {
           std::cerr << "Input size mismatch!" << std::endl;
           return;
       }

       m_layers[0].updateLayer(input);
       
       for (size_t l = 0; l < m_layers.size() - 1; ++l)
	   {
           std::vector<double> nextInputs(m_layers[l + 1].getLayerSize(), 0.0);
           m_layers[l].step(nextInputs);

		   for (size_t i = 0; i < m_layers[l + 1].getLayerSize(); ++i)
		   {
			   m_layers[l + 1].updateLayer(nextInputs);
		   }
       }
   }
   void LIFNetwork::printNetworkState(int timestep) const
   {
	   std::cout << "Time Step: " << timestep << " | Network Type: " << m_networkType << std::endl;
	   for (size_t l = 0; l < m_layers.size(); ++l) {
		   std::cout << "Layer " << l << ":" << std::endl;
		   for (size_t i = 0; i < m_layers[l].getLayerSize(); ++i) {
			   std::cout << "  Neuron " << i << " Vm: " << m_layers[l].getVm(i);
			   if (m_layers[l].hasSpiked(i)) std::cout << " (Spiked)";
			   std::cout << std::endl;
		   }
	   }
	   std::cout << "--------------------------------------" << std::endl;
   }


   void LIFNetwork::printNetworkToFile()
   {
	   std::vector<double> vms = m_layers[0].getVms(0);

	   std::ofstream outputFile("../vms.txt");
	   if (outputFile.is_open()) {
		   for (const auto& value : vms) {
			   outputFile << value << std::endl;
		   }
		   outputFile.close();
		   std::cout << "Values successfully written to file: " << "../vms.txt" << std::endl;
	   }
	   else {
		   std::cerr << "Error opening file: " << "../vms.txt" << std::endl;
	   }
   }
 