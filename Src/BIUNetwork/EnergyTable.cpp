#include "EnergyTable.hpp"
#include <fstream>
#include <sstream>

bool EnergyTable::loadFromCSV(const std::string& path) {
    table.clear();
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
        table.push_back(row);
    }
    return !table.empty();
}

double EnergyTable::getEnergy(int weight, int spike_rate) const {
    if (weight < 1 || spike_rate < 1 ||
        weight > static_cast<int>(table.size()) ||
        spike_rate > static_cast<int>(table[0].size()))
        return 0.0;
    return table[weight - 1][spike_rate - 1];
}