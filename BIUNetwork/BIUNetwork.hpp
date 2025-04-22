#ifndef SPIKINGNETWORK_H
#define SPIKINGNETWORK_H

#include <vector>
#include "BIULayer.hpp"

class SpikingNetwork {
private:
	std::vector<SpikingLayer> layers;

public:
	SpikingNetwork(const std::vector<int>& layerSizes, double vth, double vdd, int refractory, double cn, double ctotal, std::vector<double> weights);
	void setInputs(const std::vector<std::vector<std::vector<double>>>& inputs);
	std::vector<std::vector<bool>> update();
};

#endif // SPIKINGNETWORK_H