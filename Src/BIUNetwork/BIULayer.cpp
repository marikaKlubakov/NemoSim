#include "BIULayer.hpp"
#include "EnergyTable.hpp"
#include <stdexcept>

BIULayer::BIULayer(int numNeurons, double vth, double vdd, double refractory, double cn, double cu, double rleak, std::vector<std::vector<double>> weights, EnergyTable* energyTable)
    : m_energyTable(energyTable)
{
	for (int i = 0; i < numNeurons; ++i)
	{
		m_neurons.emplace_back(vth, vdd, refractory, cn, cu, rleak, weights[i], m_energyTable);
	}
}

BIULayer::BIULayer(int numNeurons, double vdd, double cn, double cu, std::vector<std::vector<double>> weights, EnergyTable * energyTable, const std::vector<double>&vthPerNeuron, const std::vector<int>&refractoryPerNeuron,  const std::vector<double>& rLeakPerNeuron)
	 : m_energyTable(energyTable)
{
	if ((int)vthPerNeuron.size() != numNeurons || (int)refractoryPerNeuron.size() != numNeurons || (int)rLeakPerNeuron.size() != numNeurons)
	{
		throw std::runtime_error("BIULayer: per-neuron vectors must match numNeurons.");
	}
	
	for (int i = 0; i < numNeurons; ++i)
	{
		m_neurons.emplace_back(vthPerNeuron[i], vdd, (double)refractoryPerNeuron[i],cn, cu, rLeakPerNeuron[i], weights[i], m_energyTable);
	}
}

void BIULayer::setInputs(const std::vector<double>& inputs)
{
	for (size_t i = 0; i < m_neurons.size(); ++i)
	{
		m_neurons[i].setSynapticInputs(inputs);
	}
}

std::vector<bool> BIULayer::update()
{
	std::vector<bool> spikes;
	for (auto& neuron : m_neurons)
	{
		spikes.push_back(neuron.update());
	}
	return spikes;
}

unsigned int BIULayer::getLayerSize() const
{
	return m_neurons.size();
}

double BIULayer::getTotalLayerSynapsesEnergy() const
{
    double sum = 0.0;
    for (const auto& neuron : m_neurons) sum += neuron.getTotalSynapticEnergy();
    return sum;
}
double BIULayer::getTotalLayerNeuronsEnergy() const
{
	double sum = 0.0;
	for (const auto& neuron : m_neurons) sum += neuron.getNeuronEnergy();
	return sum;
}