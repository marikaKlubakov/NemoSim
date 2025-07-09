#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "LIFLayer.hpp"

//implementation of LIFLayer class

LIFLayer::LIFLayer(int numNeurons, double Cm, double Cf, double Vth, double VDD, double dt, double IR)
{
	m_neurons.reserve(numNeurons);
	m_weights.resize(numNeurons);
	for (int i = 0; i < numNeurons; ++i)
	{
		m_neurons.emplace_back(Cm, Cf, Vth, VDD, dt, IR);
	}
}

void LIFLayer::initializeWeights(YFlash* yflash)
{
	m_yflash = yflash;
}

unsigned int LIFLayer::getLayerSize() const
{
	return m_neurons.size();
}

void LIFLayer::updateLayer(std::vector<double>& input)
{
	if (m_yflash)
	{
		input = m_yflash->step(input);
	}
	for (size_t i = 0; i < input.size(); ++i)
	{
		m_neurons[i].update(input[i]);
	}
}

void LIFLayer::step(std::vector<double>& nextInputs)
{

	for (size_t i = 0; i < m_neurons.size(); ++i)
	{
		if (m_neurons[i].hasSpiked())
		{
			nextInputs[i] = m_neurons[i].getVDD();

		}
		else
		{
			nextInputs[i] = 0;
		}
	}


}

