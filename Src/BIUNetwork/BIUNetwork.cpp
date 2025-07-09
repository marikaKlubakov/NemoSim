#include "BIUNetwork.hpp"
#include <fstream>

BIUNetwork::BIUNetwork(NetworkParameters params)
{
	for (size_t i = 0; i < params.layerSizes.size(); ++i)
	{
		m_vecLayers.emplace_back(params.layerSizes[i], params.VTh, params.VDD, params.refractory, params.Cn, params.Cu, params.allWeights[i]);
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
	std::vector<double> Vns = m_vecLayers[0].getVns(0);
	std::vector<double> spikes = m_vecLayers[0].getSpikesVec(0);
	std::vector<double> Vin = m_vecLayers[0].getVinVec(0);

	std::ofstream outputFile("Vns.txt");
	std::ofstream outputFile2("Spikes.txt");
	std::ofstream outputFile3("Vin.txt");
	if (outputFile.is_open())
	{
		for (const auto& value : Vns)
		{
			outputFile << value << std::endl;
		}
		outputFile.close();
		std::cout << "Values successfully written to file: " << "Vns.txt" << std::endl;
	}
	else
	{
		std::cerr << "Error opening file: " << "Vns.txt" << std::endl;
	}
	
	if (outputFile3.is_open())
	{
		for (const auto& value : Vin)
		{
			outputFile3 << value << std::endl;
		}
		outputFile3.close();
		std::cout << "Values successfully written to file: " << "Vns.txt" << std::endl;
	}
	else
	{
		std::cerr << "Error opening file: " << "Vns.txt" << std::endl;
	}

	if (outputFile2.is_open())
	{
		for (const auto& value : spikes)
		{
			outputFile2 << value << std::endl;
		}
		outputFile2.close();
		std::cout << "Values successfully written to file: " << "Spikes.txt" << std::endl;
	}
	else
	{
		std::cerr << "Error opening file: " << "Spikes.txt" << std::endl;
	}
}