#pragma once

#include <vector>
#include "BIUNeuron.hpp"

class EnergyTable; // Forward declaration

class BIULayer
{
public:
	BIULayer(int numNeurons, double vth, double vdd, double refractory, double cn, double cu, std::vector<std::vector<double>> weights, EnergyTable* energyTable = nullptr);
	void setInputs(const std::vector<std::vector<double>>& inputs);
	std::vector<bool> update();
	std::vector<double> getVns(int index) const { return m_neurons[index].getVns(); }
	std::vector<double> getSpikesVec(int index) const { return m_neurons[index].getSpikesVec(); }
	std::vector<double> getVinVec(int index) const { return m_neurons[index].getVinec(); }
	unsigned int getLayerSize() const;
	double getTotalLayerEnergy() const;
private:
	std::vector<BIUNeuron> m_neurons;
    EnergyTable* m_energyTable = nullptr;
};
