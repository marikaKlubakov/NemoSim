#pragma once

#include <vector>
#include "BIUNeuron.hpp"

class BIULayer
{
public:
	BIULayer(int numNeurons, double vth, double vdd, double refractory, double cn, double cu, std::vector<std::vector<double>> weights);
	void setInputs(const std::vector<std::vector<double>>& inputs);
	std::vector<bool> update();
	std::vector<double> getVns(int index) const { return neurons[index].getVns(); }
	std::vector<double> getSpikesVec(int index) const { return neurons[index].getSpikesVec(); }
	std::vector<double> getVinVec(int index) const { return neurons[index].getVinec(); }
private:
	std::vector<BIUNeuron> neurons;
};
