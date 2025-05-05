#ifndef SPIKINGLAYER_H
#define SPIKINGLAYER_H

#include <vector>
#include "BIUNeuron.hpp"

class SpikingLayer
{
public:
	SpikingLayer(int numNeurons, double vth, double vdd, int refractory, double cn, double cl, double cu, std::vector<double> weights);
	void setInputs(const std::vector<std::vector<double>>& inputs);
	std::vector<bool> update();
private:
	std::vector<SpikingNeuron> neurons;
};

#endif // SPIKINGLAYER_H