#include "BIUNetwork.hpp"

SpikingNetwork::SpikingNetwork(const std::vector<int>& layerSizes, double vth, double vdd, int refractory, double cn,
	double cl, double cu, std::vector<double> weights)
{
	for (int size : layerSizes)
	{
		layers.emplace_back(size, vth, vdd, refractory, cn, cl, cu, weights);
	}
}

void SpikingNetwork::setInputs(const std::vector<std::vector<std::vector<double>>>& inputs)
{
	for (size_t i = 0; i < layers.size(); ++i)
	{
		layers[i].setInputs(inputs[i]);
	}
}

std::vector<std::vector<bool>> SpikingNetwork::update()
{
	std::vector<std::vector<bool>> allSpikes;
	for (auto& layer : layers)
	{
		allSpikes.push_back(layer.update());
	}
	return allSpikes;
}