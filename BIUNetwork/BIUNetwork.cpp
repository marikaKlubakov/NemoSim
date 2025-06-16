#include "BIUNetwork.hpp"
#include <fstream>
BIUNetwork::BIUNetwork(const std::vector<int>& layerSizes, double vth, double vdd, double refractory, double cn, double cu, std::vector<std::vector<std::vector<double>>> weights)
{
	for (size_t i = 0; i < layerSizes.size(); ++i)
	{

		layers.emplace_back(layerSizes[i], vth, vdd, refractory, cn, cu, weights[i]);
	}
}

void BIUNetwork::setInputs(const std::vector<std::vector<std::vector<double>>>& inputs)
{
	layers[0].setInputs(inputs[0]);
}

std::vector<std::vector<bool>> BIUNetwork::update()
{
	std::vector<std::vector<bool>> allSpikes;
	std::vector<std::vector<double>> previousLayerOutput;
	for (size_t i = 0; i < layers.size(); ++i) 
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
			layers[i].setInputs(wrappedInputs);
		}
		auto spikes = layers[i].update();
		allSpikes.push_back(spikes);
	}
	return allSpikes;
}

void BIUNetwork::printNetworkToFile()
{
	std::vector<double> Vns = layers[0].getVns(0);
	std::vector<double> spikes = layers[0].getSpikesVec(0);
	std::vector<double> Vin = layers[0].getVinVec(0);

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