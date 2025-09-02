#include "EnergyTable.hpp"
#include <cmath>
#include <fstream>
#include <sstream>
#include <limits>

// ==================== Synapse energy table (original) ====================

bool EnergyTable::loadSynapseEnergyCSV(const std::string& path) {
    m_synapseTable.clear();
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
        m_synapseTable.push_back(row);
    }
    return !m_synapseTable.empty();
}

double EnergyTable::getSynapseEnergy(int weight, int spike_rate) const {
    if (weight < 1 || spike_rate < 1 ||
        weight > static_cast<int>(m_synapseTable.size()) ||
        spike_rate > static_cast<int>(m_synapseTable[0].size()))
        return 0.0;
    return m_synapseTable[weight - 1][spike_rate - 1];
}

// ==================== Neuron energy table (new) ====================

bool EnergyTable::loadNeuronEnergyCSV(const std::string& path) {
    m_neuronTable.clear();
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
            if (cell.empty())
                row.push_back(std::numeric_limits<double>::quiet_NaN());
            else
                row.push_back(std::stod(cell));
        }
        m_neuronTable.push_back(row);
    }
    return !m_neuronTable.empty();
}

// Helper: map vth and vn to table indices
static int vthToRow(double vth) {
    // Table rows: 100, 200, ..., 1000 (step 100)
    int idx = static_cast<int>(std::round(vth / 100.0));
    if (idx < 1) idx = 1;
    if (idx > 10) idx = 10;
    return idx - 1; // zero-based
}

static int vnToCol(double vn) {
    // Table columns: 0-50, 50-100, ..., 950-1000 (step 50)
    int idx = static_cast<int>(vn / 50.0);
    if (idx < 0) idx = 0;
    if (idx > 19) idx = 19;
    return idx; // zero-based
}

double EnergyTable::getNeuronEnergy(double vth, double vn) const {
    if (m_neuronTable.empty()) return 0.0;
	// Convert V to mV and map to indices
    int row = vthToRow(vth * 1000);
    int col = vnToCol(vn * 1000);
    if (row < 0 || row >= static_cast<int>(m_neuronTable.size())) return 0.0;
    if (col < 0 || col >= static_cast<int>(m_neuronTable[row].size())) return 0.0;
    return m_neuronTable[row][col];
}