#include "BIULayer.hpp"

BIULayer::BIULayer(int numNeurons, double vth, double vdd, double refractory, double cn, double cu, std::vector<std::vector<double>> weights)
{
	for (int i = 0; i < numNeurons; ++i)
	{
		neurons.emplace_back(vth, vdd, refractory, cn, cu, weights[i]);
	}
}

void BIULayer::setInputs(const std::vector<std::vector<double>>& inputs)
{
	for (size_t i = 0; i < neurons.size(); ++i)
	{
		neurons[i].setSynapticInputs(inputs[i]);
	}
}

std::vector<bool> BIULayer::update()
{
	std::vector<bool> spikes;
	for (auto& neuron : neurons)
	{
		spikes.push_back(neuron.update());
	}
	return spikes;
}