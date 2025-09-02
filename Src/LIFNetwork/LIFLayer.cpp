#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <stdexcept>
#include <sstream> // Add this for stringstream
#include "LIFLayer.hpp"

//implementation of LIFLayer class

LIFLayer::LIFLayer(int numNeurons, double Cm, double Cf, double Vth, double VDD, double dt, double IR)
{
    if (numNeurons <= 0) {
        std::ostringstream oss;
        oss << "LIFLayer: numNeurons must be positive. Got: " << numNeurons;
        throw std::invalid_argument(oss.str());
    }
    m_neurons.reserve(numNeurons);
    m_weights.resize(numNeurons);
    for (int i = 0; i < numNeurons; ++i)
    {
        m_neurons.emplace_back(Cm, Cf, Vth, VDD, dt, IR);
    }
}

void LIFLayer::initializeWeights(YFlash* yflash)
{
    if (!yflash) {
        throw std::invalid_argument("LIFLayer::initializeWeights: yflash pointer is null.");
    }
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
        if (input.size() != m_neurons.size()) {
            std::ostringstream oss;
            oss << "LIFLayer::updateLayer: YFlash output size (" << input.size()
                << ") does not match number of neurons (" << m_neurons.size() << ").";
            throw std::runtime_error(oss.str());
        }
    }
    for (size_t i = 0; i < input.size(); ++i)
    {
        m_neurons[i].update(input[i]);
    }
}

void LIFLayer::step(std::vector<double>& nextInputs)
{
    if (nextInputs.size() != m_neurons.size()) {
        std::ostringstream oss;
        oss << "LIFLayer::step: nextInputs size (" << nextInputs.size()
            << ") does not match number of neurons (" << m_neurons.size() << ").";
        throw std::invalid_argument(oss.str());
    }
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

