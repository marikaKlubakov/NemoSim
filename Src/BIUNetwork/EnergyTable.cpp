#include "EnergyTable.hpp"
#include <fstream>
#include <sstream>

bool EnergyTable::loadFromCSV(const std::string& path) {
    m_table.clear();
    std::ifstream file(path);
    if (!file.is_open()) return false;
    std::string line;
    // Skip header
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<double> row;
        // Skip weight label
        std::getline(ss, cell, ',');
        while (std::getline(ss, cell, ',')) {
            row.push_back(std::stod(cell));
        }
        m_table.push_back(row);
    }
    return !m_table.empty();
}

double EnergyTable::getEnergy(int weight, int spike_rate) const {
    if (weight < 1 || spike_rate < 1 ||
        weight > static_cast<int>(m_table.size()) ||
        spike_rate > static_cast<int>(m_table[0].size()))
        return 0.0;
    return m_table[weight - 1][spike_rate - 1];
}

double EnergyTable::getEnergyBySpike(int weight, bool spike_in) const {
    // Assume spike_in == true → spike_rate = 2, false → spike_rate = 1
    int spike_rate = spike_in ? 1: 2;
    return getEnergy(weight, spike_rate);
}