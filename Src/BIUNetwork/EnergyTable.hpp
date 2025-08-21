#pragma once
#include <vector>
#include <string>

class EnergyTable {
public:
    // Loads a CSV file with the described format
    bool loadFromCSV(const std::string& path);

    // Returns energy for given weight (1-based) and spike_rate (1-based)
    double getEnergy(int weight, int spike_rate) const;

    double getEnergyBySpike(int weight, bool spike_in) const;

private:
    std::vector<std::vector<double>> m_table; // [weight-1][spike_rate-1]
};