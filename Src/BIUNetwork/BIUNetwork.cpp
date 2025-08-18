#include "BIUNetwork.hpp"
#include "EnergyTable.hpp" // Add this include
#include <fstream>
BIUNetwork::BIUNetwork(NetworkParameters params)
{
	energyTable = new EnergyTable();

	for (size_t i = 0; i < params.layerSizes.size(); ++i)
	{
		m_vecLayers.emplace_back(params.layerSizes[i], params.VTh, params.VDD, params.refractory, params.Cn, params.Cu, params.allWeights[i]);
	}
	if (!params.csvPath.empty())
	{
		if (!energyTable->loadFromCSV(params.csvPath)) {
			throw std::runtime_error("Failed to load energy table from: " + params.csvPath);
		}
	}
}

void BIUNetwork::run(std::ifstream& inputFile)
{
	std::string line;
	int t = 0;
	if (inputFile.is_open())
	{
		while (std::getline(inputFile, line))
		{
			// Format input to match the required structure
			double value = std::stod(line); // Convert string to double
			std::vector<double> input;
			std::vector<std::vector<std::vector<double>>> currentInput = { { { value } } };// split the values of the line into the vector
			setInputs(currentInput);
			auto spikes = update();
			std::cout << "Timestep " << t << ": ";
			for (const auto& layerSpikes : spikes)
			{
				for (bool spike : layerSpikes)
				{
					std::cout << (spike ? "1" : "0") << " ";
				}
			}
			std::cout << std::endl;
		}
		inputFile.close();
	}
	else
	{
		std::cerr << "Unable to open file" << std::endl;
	}
	inputFile.close();
}

void BIUNetwork::setInputs(const std::vector<std::vector<std::vector<double>>>& inputs)
{
	m_vecLayers[0].setInputs(inputs[0]);
}

std::vector<std::vector<bool>> BIUNetwork::update()
{
	std::vector<std::vector<bool>> allSpikes;
	std::vector<std::vector<double>> previousLayerOutput;
	for (size_t i = 0; i < m_vecLayers.size(); ++i) 
	{
		// If not the first layer, use spikes from previous layer as inputs
		if (i > 0)
		{
			std::vector<double> inputs;
			for (bool spike : allSpikes[i - 1]) 
			{
				inputs.push_back(spike ? 1.0 : 0.0);  // Convert spike to input current
			}
			std::vector<std::vector<double>> wrappedInputs = { inputs };
			m_vecLayers[i].setInputs(wrappedInputs);
		}
		auto spikes = m_vecLayers[i].update();
		allSpikes.push_back(spikes);
	}
	return allSpikes;
}

void BIUNetwork::printNetworkToFile()
{
    for (size_t layerIdx = 0; layerIdx < m_vecLayers.size(); ++layerIdx) {
        size_t numNeurons = m_vecLayers[layerIdx].getLayerSize();

        for (size_t neuronIdx = 0; neuronIdx < numNeurons; ++neuronIdx) {
            std::vector<double> Vns = m_vecLayers[layerIdx].getVns(neuronIdx);
            std::vector<double> spikes = m_vecLayers[layerIdx].getSpikesVec(neuronIdx);
            std::vector<double> Vin = m_vecLayers[layerIdx].getVinVec(neuronIdx);

            std::string vnsFile = "vns" + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";
            std::string spikesFile = "spikes" + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";
            std::string vinFile = "vin" + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";

            std::ofstream vnsOut(vnsFile);
            std::ofstream spikesOut(spikesFile);
            std::ofstream vinOut(vinFile);

            if (vnsOut.is_open()) {
                for (const auto& value : Vns) {
                    vnsOut << value << std::endl;
                }
                vnsOut.close();
            }

            if (spikesOut.is_open()) {
                for (const auto& value : spikes) {
                    spikesOut << value << std::endl;
                }
                spikesOut.close();
            }

            if (vinOut.is_open()) {
                for (const auto& value : Vin) {
                    vinOut << value << std::endl;
                }
                vinOut.close();
            }
        }
    }
}