#pragma once

#include <vector>
#include "BIUNeuron.hpp"

class EnergyTable; // Forward declaration

class BIULayer
{
public:
	BIULayer(int numNeurons, double vth, double vdd, double refractory, double cn, double cu, double rleak, std::vector<std::vector<double>> weights, EnergyTable* energyTable = nullptr);
	BIULayer(int numNeurons, double vdd, double cn, double cu, std::vector<std::vector<double>> weights, EnergyTable * energyTable, const std::vector<double>&vthPerNeuron, const std::vector<int>&refractoryPerNeuron, const std::vector<double>& rLeakPerNeuron);
	void setInputs(const std::vector<double>& inputs);
	std::vector<bool> update();
	std::vector<double> getVns(int index) const { return m_neurons[index].getVns(); }
	std::vector<double> getSpikesVec(int index) const { return m_neurons[index].getSpikesVec(); }
	std::vector<double> getVinVec(int index) const { return m_neurons[index].getVinec(); }
	unsigned int getLayerSize() const;
	double getTotalLayerSynapsesEnergy() const;
	double getTotalLayerNeuronsEnergy() const;
private:
	std::vector<BIUNeuron> m_neurons;
    EnergyTable* m_energyTable = nullptr;
};
