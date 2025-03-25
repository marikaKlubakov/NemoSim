#ifndef SPIKINGLAYER_H
#define SPIKINGLAYER_H

#include <vector>
#include "NielNeuron.hpp"

class SpikingLayer {
private:
	std::vector<SpikingNeuron> neurons;

public:
	SpikingLayer(int numNeurons, double vth, double vdd, int refractory, double cn, double ctotal, std::vector<double> weights);
	void setInputs(const std::vector<std::vector<double>>& inputs);
	std::vector<bool> update();
};

#endif // SPIKINGLAYER_H