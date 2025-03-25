#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "LIFLayer.h"

//implementation of LIFLayer class

LIFLayer:: LIFLayer(int numNeurons, double Cm, double Cf, double Vth, double VDD, double dt, YFlash* yflash)
	:m_VDD(VDD) ,yflash(yflash)
{
   m_neurons.reserve(numNeurons);
   for (int i = 0; i < numNeurons; ++i)
	{
	   m_neurons.emplace_back(Cm, Cf, Vth, VDD, dt, yflash);
	}
}

unsigned int LIFLayer::getLayerSize() const
{
	return m_neurons.size();
}

void LIFLayer::updateLayer(const std::vector<double>& input, int row, int col)
{
	for (size_t i = 0; i < input.size(); ++i)
	{
		double Vgs = input[i];
		double Vds = m_VDD - input[i];
		m_neurons[i].update(Vgs, Vds, i, 0);
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

