#ifndef SPIKINGNETWORK_H
#define SPIKINGNETWORK_H

#include <vector>
#include "BIULayer.hpp"

class SpikingNetwork
{
public:
	SpikingNetwork(const std::vector<int>& layerSizes, double vth, double vdd, int refractory, double cn, double cl, double cu, std::vector<double> weights);
	void setInputs(const std::vector<std::vector<std::vector<double>>>& inputs);
	std::vector<std::vector<bool>> update();
private:
	std::vector<SpikingLayer> layers;
};

#endif // SPIKINGNETWORK_H