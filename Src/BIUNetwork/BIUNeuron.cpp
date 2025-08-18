#include "BIUNeuron.hpp"
#include <vector>
#include <stdexcept> // For std::invalid_argument and std::runtime_error

#define FCLK 1e7  // Clock frequency (10MHz)
#define R_LEAK 1e8  // Leakage resistor
#define VDD_HALF_FACTOR 0.5 // For clarity in voltage calculation

BIUNeuron::BIUNeuron(double vth, double vdd, double refractory, double cn, double cu, std::vector<double> weights)
    : VTH(vth), VDD(vdd), refractoryTime(refractory),
    Cn(cn), Cu(cu), synapticWeights(weights)
{
    Vn = 0;
    cyclesLeft = 0;
    synapticInputs.resize(weights.size(), 0.0);
}

void BIUNeuron::setSynapticInputs(const std::vector<double>& inputs)
{
    if (inputs.size() != synapticWeights.size())
        throw std::invalid_argument("Input size does not match synaptic weights size.");
    synapticInputs = inputs;
    if (!synapticInputs.empty())
        m_Vin.emplace_back(synapticInputs[0]);
}

bool BIUNeuron::update()
{
    m_Vn.emplace_back(Vn);
    if (cyclesLeft > 0)
    {
        Vn = 0;
        cyclesLeft--;
        m_spikes.emplace_back(0);
        return false;
    }
    double dynamicCap = 0;
    double weightedSum = 0;
    for (size_t i = 0; i < synapticWeights.size(); ++i)
    {
        double W = synapticWeights[i];
        double Vin = synapticInputs[i];
        dynamicCap += Cu * W * Vin;
        weightedSum +=  W * Vin * VDD;
    }
    double Ctotal = Cn + dynamicCap;
    if (Ctotal == 0.0)
        throw std::runtime_error("Total capacitance is zero.");

    Vn = (Cn / Ctotal) * Vn
        + (Cu / Ctotal) * weightedSum
        + (Vn - VDD_HALF_FACTOR * VDD) / (Cn * FCLK * R_LEAK);

    if (Vn >= VTH)
    {
        Vn = 0;
        cyclesLeft = static_cast<int>(refractoryTime);
        m_spikes.emplace_back(1);
        return true;
    }

    m_spikes.emplace_back(0);

    return false;
}

double BIUNeuron::getVoltage() const
{
    return Vn;
}