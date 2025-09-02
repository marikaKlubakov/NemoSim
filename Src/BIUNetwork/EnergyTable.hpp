#pragma once
#include <vector>
#include <string>

class EnergyTable {
public:
    // --- Synapse energy table (original functionality) ---
    bool loadSynapseEnergyCSV(const std::string& path);
    double getSynapseEnergy(int weight, int spike_rate) const;

    // --- Neuron energy table (new functionality) ---
    bool loadNeuronEnergyCSV(const std::string& path);
    double getNeuronEnergy(double vth, double vn) const; // <-- new signature

private:
    // --- Synapse energy storage (original) ---
    std::vector<std::vector<double>> m_synapseTable;

    // --- Neuron energy storage (new) ---
    std::vector<std::vector<double>> m_neuronTable;
};
