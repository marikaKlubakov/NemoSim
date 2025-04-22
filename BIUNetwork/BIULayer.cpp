#include "BIULayer.hpp"

SpikingLayer::SpikingLayer(int numNeurons, double vth, double vdd, int refractory, double cn, double ctotal, std::vector<double> weights) {
	for (int i = 0; i < numNeurons; ++i) {
		neurons.emplace_back(vth, vdd, refractory, cn, ctotal, weights);
	}
}

void SpikingLayer::setInputs(const std::vector<std::vector<double>>& inputs) {
	for (size_t i = 0; i < neurons.size(); ++i) {
		neurons[i].setSynapticInputs(inputs[i]);
	}
}

std::vector<bool> SpikingLayer::update() {
	std::vector<bool> spikes;
	for (auto& neuron : neurons) {
		spikes.push_back(neuron.update());
	}
	return spikes;
}