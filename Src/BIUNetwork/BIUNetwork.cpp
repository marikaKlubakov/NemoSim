#include "BIUNetwork.hpp"
#include "EnergyTable.hpp"
#include <fstream>
#include <sstream>
#include <vector>

// One log file per DS unit: DS_0, DS_1, ...
static std::vector<std::ofstream> s_dsLogs;

BIUNetwork::BIUNetwork(NetworkParameters params)
{
    m_dsClockMHz = params.DSClockMHz;
    m_dsBitWidth = static_cast<unsigned int>(params.DSBitWidth);
    m_dsMode     = params.DSMode;

    m_energyTable = new EnergyTable();
    if (!params.allWeights.empty() && !params.allWeights[0].empty())
    {
        size_t inputCount = params.allWeights[0][0].size(); // number of inputs per neuron
        initFrontEndDS_(inputCount);
    }
    for (size_t i = 0; i < params.layerSizes.size(); ++i)
    {
        bool havePerNeuron =
            (i < params.biuNeuronVTh.size() && i < params.biuNeuronRefractory.size() &&
             params.biuNeuronVTh[i].size() == (size_t)params.layerSizes[i] &&
             params.biuNeuronRefractory[i].size() == (size_t)params.layerSizes[i] &&
             params.biuNeuronRLeak[i].size() == (size_t)params.layerSizes[i]);

        if (havePerNeuron)
        {
            m_vecLayers.emplace_back(params.layerSizes[i], params.VDD, params.Cn, params.Cu,
                                     params.allWeights[i], m_energyTable,
                                     params.biuNeuronVTh[i], params.biuNeuronRefractory[i], params.biuNeuronRLeak[i]);
        }
        else
        {
            m_vecLayers.emplace_back(params.layerSizes[i], params.VTh, params.VDD, params.refractory,
                                     params.Cn, params.Cu, params.Rleak,
                                     params.allWeights[i], m_energyTable);
        }
    }
    if (!params.synapsesEnergyCsvPath.empty())
    {
        if (!m_energyTable->loadSynapseEnergyCSV(params.synapsesEnergyCsvPath)) {
            throw std::runtime_error("Failed to load synapses energy table from: " + params.synapsesEnergyCsvPath);
        }
    }
    if (!params.neuronEnergyCsvPath.empty())
    {
        if (!m_energyTable->loadNeuronEnergyCSV(params.neuronEnergyCsvPath)) {
            throw std::runtime_error("Failed to load neuron energy table from: " + params.neuronEnergyCsvPath);
        }
    }
}

void BIUNetwork::run(std::ifstream& inputFile)
{
    if (!inputFile.is_open()) {
        std::cerr << "Unable to open file\n";
        return;
    }

    const std::size_t totalLines = countLines(inputFile);

    std::string line;
    std::size_t currentLine = 0;

    while (std::getline(inputFile, line))
    {
        ++currentLine;

        // Parse raw values for this line (digital codes for DS)
        std::istringstream iss(line);
        std::vector<double> values;
        double v;
        while (iss >> v) values.push_back(v);

        // If no DS front-end, fall back to original single-step behavior.
        if (m_dsUnits.empty())
        {
            setInputs(values);      // original path
            update();
            showProgressBar(currentLine, totalLines);
            continue;
        }

        // Set new codes (one per DS) for this line (no tick yet).
        const size_t n = std::min(values.size(), m_dsUnits.size());
        for (size_t i = 0; i < n; ++i)
        {
            m_dsUnits[i].setCode(clampToCode_(values[i]));
        }
        // If fewer codes than DS units: leave remaining codes unchanged or set to zero
        for (size_t i = n; i < m_dsUnits.size(); ++i)
        {
            m_dsUnits[i].setCode(0);
        }

        // Gating loop: keep ticking until ALL DS units have produced at least one spike.
        std::vector<bool> haveSpiked(m_dsUnits.size(), false);
        const std::size_t maxSafetyCycles = 1'000'000; // safety cap
        std::size_t cycles = 0;

        while (true)
        {
            std::vector<double> dsOut;
            dsOut.reserve(m_dsUnits.size());
            bool all = true;

            // Tick each DS; record first spike occurrence.
            for (size_t i = 0; i < m_dsUnits.size(); ++i)
            {
                bool spk = m_dsUnits[i].tick();
                if (spk) haveSpiked[i] = true;
                if (!haveSpiked[i]) all = false;
                dsOut.push_back(spk ? 1.0 : 0.0);
            }

            setInputs(dsOut);
            update();

            if (all) break;
            if (++cycles > maxSafetyCycles)
            {
                std::cerr << "Warning: DS gating exceeded safety cycles on line " << currentLine
                          << ". Proceeding to next line.\n";
                break;
            }
        }

        showProgressBar(currentLine, totalLines);
    }

    std::cout << "\nFinished executing.\n";
    inputFile.close();

    auto totalSynapsesEnergy = getTotalSynapsesEnergy();
    std::cout << "Total synaptic energy: " << totalSynapsesEnergy << " fJ" << '\n';

    auto totalNeuronsEnergy = getTotalNeuronsEnergy();
    std::cout << "Total neurons energy: " << totalNeuronsEnergy << " fJ" << '\n';
}

void BIUNetwork::setInputs(const std::vector<double>& inputs)
{
    if (!m_vecLayers.empty() && !m_dsUnits.empty())
    {
        const size_t n = std::min(inputs.size(), m_dsUnits.size());
        
        for (size_t i = 0; i < n; ++i)
        {
            if (i < s_dsLogs.size() && s_dsLogs[i].is_open())
                s_dsLogs[i] << (inputs[i] ? 1 : 0) << '\n';
        }
        m_vecLayers[0].setInputs(inputs);
    }
    else
    {
        if (!m_vecLayers.empty())
            m_vecLayers[0].setInputs(inputs);
    }
}

std::vector<std::vector<bool>> BIUNetwork::update()
{
    std::vector<std::vector<bool>> allSpikes;
    for (size_t i = 0; i < m_vecLayers.size(); ++i)
    {
        if (i > 0)
        {
            std::vector<double> inputs;
            inputs.reserve(allSpikes[i - 1].size());
            for (bool spike : allSpikes[i - 1])
                inputs.push_back(spike ? 1.0 : 0.0);
            m_vecLayers[i].setInputs(inputs);
        }
        auto spikes = m_vecLayers[i].update();
        allSpikes.push_back(spikes);
    }
    return allSpikes;
}

void BIUNetwork::printNetworkToFile()
{
    for (size_t layerIdx = 0; layerIdx < m_vecLayers.size(); ++layerIdx)
    {
        size_t numNeurons = m_vecLayers[layerIdx].getLayerSize();

        for (size_t neuronIdx = 0; neuronIdx < numNeurons; ++neuronIdx)
        {
            std::vector<double> Vns    = m_vecLayers[layerIdx].getVns(neuronIdx);
            std::vector<double> spikes = m_vecLayers[layerIdx].getSpikesVec(neuronIdx);
            std::vector<double> Vin    = m_vecLayers[layerIdx].getVinVec(neuronIdx);

            std::string vnsFile    = "vns_"    + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";
            std::string spikesFile = "spikes_" + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";
            std::string vinFile    = "vin_"    + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";

            std::ofstream vnsOut(vnsFile);
            std::ofstream spikesOut(spikesFile);
            std::ofstream vinOut(vinFile);

            if (vnsOut.is_open()) {
                for (const auto& value : Vns) vnsOut << value << '\n';
            }
            if (spikesOut.is_open()) {
                for (const auto& value : spikes) spikesOut << value << '\n';
            }
            if (vinOut.is_open()) {
                for (const auto& value : Vin) vinOut << value << '\n';
            }
        }
    }
}

double BIUNetwork::getTotalNeuronsEnergy()
{
    double sum = 0.0;
    for (const auto& layer : m_vecLayers) sum += layer.getTotalLayerNeuronsEnergy();
    return sum;
}

double BIUNetwork::getTotalSynapsesEnergy()
{
    double sum = 0.0;
    for (const auto& layer : m_vecLayers) sum += layer.getTotalLayerSynapsesEnergy();
    return sum;
}

// ===== Helpers for DS front-end =====
void BIUNetwork::initFrontEndDS_(size_t inputCount)
{
    m_dsUnits.clear();
    m_dsUnits.reserve(inputCount);

    // (Re)open log files
    s_dsLogs.clear();
    s_dsLogs.resize(inputCount);
    for (size_t i = 0; i < inputCount; ++i)
    {
        m_dsUnits.emplace_back(m_dsClockMHz, m_dsBitWidth, m_dsMode);
        m_dsUnits.back().setCode(0);
        s_dsLogs[i].open("DS_" + std::to_string(i), std::ios::out | std::ios::trunc);
        if (!s_dsLogs[i].is_open())
            std::cerr << "Warning: could not open DS_" << i << " for writing.\n";
    }
}

unsigned int BIUNetwork::clampToCode_(double x) const
{
    if (m_dsBitWidth >= 31) return 0; // safety
    unsigned int maxCode = (1u << m_dsBitWidth) - 1u;
    if (x < 0.0) return 0u;
    if (x > (double)maxCode) return maxCode;
    return static_cast<unsigned int>(x);
}