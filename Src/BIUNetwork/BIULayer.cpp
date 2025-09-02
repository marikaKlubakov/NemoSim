#include "BIULayer.hpp"
#include "EnergyTable.hpp"

BIULayer::BIULayer(int numNeurons, double vth, double vdd, double refractory, double cn, double cu, std::vector<std::vector<double>> weights, EnergyTable* energyTable)
    : m_energyTable(energyTable)
{
	for (int i = 0; i < numNeurons; ++i)
	{
		m_neurons.emplace_back(vth, vdd, refractory, cn, cu, weights[i], m_energyTable);
	}
}

void BIULayer::setInputs(const std::vector<std::vector<double>>& inputs)
{
	for (size_t i = 0; i < m_neurons.size(); ++i)
	{
		m_neurons[i].setSynapticInputs(inputs[i]);
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