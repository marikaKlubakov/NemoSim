#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <fstream>
#include "LIFNetwork.hpp"

//implementation of LIFNetwork class

LIFNetwork::LIFNetwork(const std::vector<int>& layerSizes, double Cm, double Cf, double Vth, double VDD_, double dt_, double IR_, std::vector<std::vector<std::vector<double>>>& YFlashWeights)
       : m_VDD(VDD_), m_dt(dt_)
{
	for (size_t i = 0; (i < YFlashWeights.size() ) && YFlashWeights.size()!=0 ; ++i)
	{
		m_yflashVec.emplace_back(YFlashWeights[i]);
	}
	for (int size : layerSizes)
	{
		m_layers.emplace_back(size, Cm, Cf, Vth, m_VDD, m_dt, IR_);
	}
	for (size_t i = 0; (i < m_layers.size() - 1)&& m_yflashVec.size() != 0; ++i)
	{
		m_layers[i+1].initializeWeights(&m_yflashVec[i]);
	}
}

void LIFNetwork::feedForward(std::vector<double>& input) 
{
    if (input.size() != m_layers[0].getLayerSize()) 
	{
        std::cerr << "Input size mismatch!" << std::endl;
        return;
    }
       
    for (size_t l = m_layers.size() - 1 ; l >= 0 ; --l)
	{
		if (l == 0)
		{
			m_layers[0].updateLayer(input);
			break;
		} 
		else
		{
			std::vector<double> nextInputs(m_layers[l - 1].getLayerSize(), 0.0);
			m_layers[l - 1].step(nextInputs);

			m_layers[l].updateLayer(nextInputs);
		}


    }
}
void LIFNetwork::printNetworkState(int timestep) const
{
	std::cout << "Time Step: " << timestep << std::endl;
	for (size_t l = 0; l < m_layers.size(); ++l)
	{
		std::cout << "Layer " << l << ":" << std::endl;
		for (size_t i = 0; i < m_layers[l].getLayerSize(); ++i) 
		{
			std::cout << "  Neuron " << i << " Vm: " << m_layers[l].getVm(i);
			if (m_layers[l].hasSpiked(i)) std::cout << " (Spiked)";
			std::cout << std::endl;
		}
	}
	std::cout << "--------------------------------------" << std::endl;
}


void LIFNetwork::printNetworkToFile()
{

	for (size_t layerIdx = 0; layerIdx < m_layers.size(); ++layerIdx) {
		size_t numNeurons = m_layers[layerIdx].getLayerSize();

		for (size_t neuronIdx = 0; neuronIdx < numNeurons; ++neuronIdx) {
			std::vector<double> vms = m_layers[layerIdx].getVms(neuronIdx);
			std::vector<double> Iin = m_layers[layerIdx].getIinVec(neuronIdx);
			std::vector<double> vout = m_layers[layerIdx].getVoutVec(neuronIdx);

			std::string vmsFile = "vms" + std::to_string(layerIdx) + std::to_string(neuronIdx) + ".txt";
			std::string iinFile = "Iins" + std::to_string(layerIdx) + std::to_string(neuronIdx) + ".txt";
			std::string voutFile = "Vouts" + std::to_string(layerIdx) + std::to_string(neuronIdx) + ".txt";

			std::ofstream vmsOut(vmsFile);
			std::ofstream iinOut(iinFile);
			std::ofstream voutOut(voutFile);

			if (vmsOut.is_open()) {
				for (const auto& value : vms) {
					vmsOut << value << std::endl;
				}
				vmsOut.close();
			}

			if (iinOut.is_open()) {
				for (const auto& value : Iin) {
					iinOut << value << std::endl;
				}
				iinOut.close();
			}

			if (voutOut.is_open()) {
				for (const auto& value : vout) {
					voutOut << value << std::endl;
				}
				voutOut.close();
			}
		}
	}

}
 