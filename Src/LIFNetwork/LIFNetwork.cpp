#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <fstream>
#include "LIFNetwork.hpp"

//implementation of LIFNetwork class

LIFNetwork::LIFNetwork(NetworkParameters params)
	: m_VDD(params.VDD), m_dt(params.dt)
{
	if (params.layerSizes.empty())
	{
		throw std::invalid_argument("LIFNetwork: layerSizes must not be empty.");
	}

	for (size_t i = 0; (i < params.YFlashWeights.size()); ++i)
	{
		m_yflashVec.emplace_back(params.YFlashWeights[i]);
	}
	for (int size : params.layerSizes)
	{
		m_layers.emplace_back(size, params.Cm, params.Cf, params.VTh, m_VDD, m_dt, params.IR);
	}
	for (size_t i = 0; (i < m_layers.size() - 1) && m_yflashVec.size() != 0; ++i)
	{
		m_layers[i + 1].initializeWeights(&m_yflashVec[i]);
	}
}

void LIFNetwork::feedForward(std::vector<double>& input)
{
	if (input.size() != m_layers[0].getLayerSize())
	{
		std::cerr << "Input size mismatch!" << std::endl;
		return;
	}

	for (size_t l = m_layers.size() - 1; l >= 0; --l)
	{
		if (l == 0)
		{
			m_layers[0].updateLayer(input);
			break;
		}

		std::vector<double> nextInputs(m_layers[l - 1].getLayerSize(), 0.0);
		m_layers[l - 1].step(nextInputs);

		m_layers[l].updateLayer(nextInputs);



	}
}
void LIFNetwork::printNetworkState(int timestep) const
{
	std::cout << "Time Step: " << timestep << std::endl;
	for (int l = 0; l < m_layers.size(); ++l)
	{
		std::cout << "Layer " << l << ":" << std::endl;
		for (int i = 0; i < m_layers[l].getLayerSize(); ++i)
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
	std::cout << "printing network files " << std::endl;
	for (int layerIdx = 0; layerIdx < m_layers.size(); ++layerIdx) {
		int numNeurons = m_layers[layerIdx].getLayerSize();

		for (int neuronIdx = 0; neuronIdx < numNeurons; ++neuronIdx) {
			std::vector<double> vms = m_layers[layerIdx].getVms(neuronIdx);
			std::vector<double> Iin = m_layers[layerIdx].getIinVec(neuronIdx);
			std::vector<double> vout = m_layers[layerIdx].getVoutVec(neuronIdx);

			std::string vmsFile = "vms_" + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";
			std::string iinFile = "iins_" + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";
			std::string voutFile = "vouts_" + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";

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



 
void LIFNetwork::run(std::ifstream& inputFile)
{
	std::string line;

	if (!inputFile.is_open()) {
		std::cerr << "Unable to open file\n";
		return;
	}

	// use base helper (or keep your original counting code)
	const std::size_t totalLines = countLines(inputFile);

	std::size_t currentLine = 0;
	while (std::getline(inputFile, line)) {
		++currentLine;

		std::vector<double> values;
		std::stringstream ss(line);
		double value;
		while (ss >> value) values.push_back(value);

		feedForward(values);
		showProgressBar(currentLine, totalLines);  // now from BaseNetwork
	}

	std::cout << "\nFinished executing.\n";
	inputFile.close();
}
