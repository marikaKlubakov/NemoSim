#include "BIUNeuron.hpp"
#include "EnergyTable.hpp"
#include <vector>
#include <stdexcept> // For std::invalid_argument and std::runtime_error
#include <cmath> // exp

#define FCLK 1e7  // Clock frequency (10MHz)
#define R_LEAK 1e8  // Leakage resistor
#define VDD_HALF_FACTOR 0.5 // For clarity in voltage calculation

BIUNeuron::BIUNeuron(double vth, double vdd, double refractory,
    double cn, double cu, double cpara, double rLeak,
    std::vector<double> weights, EnergyTable* energyTable)
    : m_VTH(vth), m_VDD(vdd), m_refractoryTime(refractory),
    m_Cn(cn), m_Cu(cu), m_Cpara(cpara), m_RLeak(rLeak),
    m_synapticWeights(weights), m_energyTable(energyTable)
{
    m_Vn = 0;
    cyclesLeft = 0;
    m_synapticInputs.resize(weights.size(), 0.0);
    m_synapticEnergy.resize(weights.size(), 0.0);
}

void BIUNeuron::setSynapticInputs(const std::vector<double>& inputs)
{
    if (inputs.size() != m_synapticWeights.size())
        throw std::invalid_argument("Input size does not match synaptic weights size.");
    m_synapticInputs = inputs;
    for (size_t i = 0; i < m_synapticInputs.size(); ++i)
    {
	    m_synapticEnergy[i] += m_energyTable->getSynapseEnergy(static_cast<int>(m_synapticWeights[i]), m_synapticInputs[i] > 0);
    }
    if (!m_synapticInputs.empty())
        m_Vins.emplace_back(m_synapticInputs[0]);
}

bool BIUNeuron::update()
{
    m_neuronEnergy += m_energyTable->getNeuronEnergy(m_VTH, m_Vn);
    m_Vns.emplace_back(m_Vn);

    if (cyclesLeft > 0)
    {
        m_Vn = 0;
        cyclesLeft--;
        m_spikes.emplace_back(0);
        return false;
    }

    const size_t Nu = m_synapticWeights.size();
    const double Cstatic = m_Cn + static_cast<double>(Nu) * m_Cpara;

    // Ctotal = Cn + Nu*Cpara + sum_i spike_i * (Cu * Wi)
    double Ctotal = Cstatic;
    for (size_t i = 0; i < Nu; ++i)
    {
        const double spike = (m_synapticInputs[i] > 0.0) ? 1.0 : 0.0;
        m_vin_sum += spike;
        Ctotal += spike * (m_Cu * m_synapticWeights[i]);
    }

    if (Ctotal == 0.0)
        throw std::runtime_error("Total capacitance is zero.");

    // exp(-1 / (R * (Cn + Nu*Cpara) * fclk))
    const double decay = std::exp(-1.0 / (m_RLeak * Cstatic * FCLK));

    // First term: ((Cn+Nu*Cpara)/Ctotal * Vn(t)) * decay
    double vn_next = (Cstatic / Ctotal) * m_Vn * decay;

    // Second term: sum_i [ (Cu/Ctotal) * spike_i * (Wi * (VDD * WS_i)) ]
    // Here WS_i = +1.0 by default.
    for (size_t i = 0; i < Nu; ++i)
    {
        const double spike = (m_synapticInputs[i] > 0.0) ? 1.0 : 0.0;
        const double Wi = m_synapticWeights[i];
        const double WSi = 1.0; // TODO: replace if you have per-synapse sign/selector
        vn_next += (m_Cu / Ctotal) * spike * (Wi * (m_VDD * WSi));
    }

    m_Vn = vn_next;

    if (m_Vn >= m_VTH)
    {
        m_Vn = 0;
        cyclesLeft = static_cast<int>(m_refractoryTime);
        m_spikes.emplace_back(1);
        return true;
    }

    m_spikes.emplace_back(0);
    return false;
}

double BIUNeuron::getVoltage() const
{
    return m_Vn;
}

double BIUNeuron::getTotalSynapticEnergy() const
{
    double sum = 0.0;
    for (double e : m_synapticEnergy) sum += e;
    return sum;
}

double BIUNeuron::getNeuronEnergy() const
{
    return m_neuronEnergy;
}