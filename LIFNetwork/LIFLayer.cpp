#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "LIFLayer.hpp"

//implementation of LIFLayer class

LIFLayer:: LIFLayer(int numNeurons, double Cm, double Cf, double Vth, double VDD, double dt) 
{
   m_neurons.reserve(numNeurons);
   m_weights.resize(numNeurons);
   for (int i = 0; i < numNeurons; ++i)
	{
	   m_neurons.emplace_back(Cm, Cf, Vth, VDD, dt);
	}
}

void LIFLayer::initializeWeights(int nextLayerSize)
{
   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_real_distribution<double> dist(-1.0, 1.0);
   for (auto& row : m_weights)
   {
	   row.resize(nextLayerSize);
	   for (double& w : row) 
	   {
		   w = dist(gen);
	   }
   }
}

unsigned int LIFLayer::getLayerSize() const
{
	return m_neurons.size();
}

void LIFLayer::updateLayer(const std::vector<double>& input)
{
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
			for (size_t j = 0; j < m_neurons.size(); ++j)
			{
				nextInputs[j] += m_weights[i][j] * m_neurons[i].getVDD();
			}
		}
	}
}

