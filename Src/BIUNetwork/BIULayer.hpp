#pragma once

#include <vector>
#include <stdexcept>
#include "BIUNeuron.hpp"

class EnergyTable; // Forward declaration

class BIULayer
{
public:
	BIULayer(int numNeurons, double vth, double vdd, double refractory, double cn, double cu, double cpara, double rleak, std::vector<std::vector<double>> weights, EnergyTable* energyTable = nullptr);
	BIULayer(int numNeurons, double vdd, double cn, double cu, double cpara, std::vector<std::vector<double>> weights, EnergyTable * energyTable, const std::vector<double>&vthPerNeuron, const std::vector<int>&refractoryPerNeuron, const std::vector<double>& rLeakPerNeuron);
	void setInputs(const std::vector<double>& inputs);
	std::vector<uint8_t> update();
	std::vector<double> getVns(int index) const
	{
		if (index < 0 || index >= static_cast<int>(m_neurons.size()))
			throw std::out_of_range("BIULayer: neuron index out of range");
		return m_neurons[index].getVns();
	}
	std::vector<double> getSpikesVec(int index) const
	{
		if (index < 0 || index >= static_cast<int>(m_neurons.size()))
			throw std::out_of_range("BIULayer: neuron index out of range");
		return m_neurons[index].getSpikesVec();
	}
	std::vector<double> getVinVec(int index) const
	{
		if (index < 0 || index >= static_cast<int>(m_neurons.size()))
			throw std::out_of_range("BIULayer: neuron index out of range");
		return m_neurons[index].getVinVec();
	}
	unsigned int getLayerSize() const;
	double getTotalLayerSynapsesEnergy() const;
	double getTotalLayerNeuronsEnergy() const;
	double getTotalVINS() const;
private:
	std::vector<BIUNeuron> m_neurons;
    EnergyTable* m_energyTable = nullptr;
};
