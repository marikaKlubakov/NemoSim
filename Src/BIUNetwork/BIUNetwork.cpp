#include "BIUNetwork.hpp"
#include "EnergyTable.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

// One log file per DS unit: DS_0, DS_1, ...
static std::vector<std::ofstream> s_dsLogs;

BIUNetwork::BIUNetwork(NetworkParameters params)
{
    m_dsClockMHz = params.DSClockMHz;
    m_dsBitWidth = static_cast<unsigned int>(params.DSBitWidth);
    m_dsMode     = params.DSMode;
    m_verbosity  = params.verbosity; // NEW

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
            m_vecLayers.emplace_back(params.layerSizes[i], params.VDD, params.Cn, params.Cu, params.CPara,
                                     params.allWeights[i], m_energyTable,
                                     params.biuNeuronVTh[i], params.biuNeuronRefractory[i], params.biuNeuronRLeak[i]);
        }
        else
        {
            m_vecLayers.emplace_back(params.layerSizes[i], params.VTh, params.VDD, params.refractory,
                                     params.Cn, params.CPara, params.Cu, params.Rleak,
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

BIUNetwork::~BIUNetwork()
{
    delete m_energyTable;
    m_energyTable = nullptr;
}

void BIUNetwork::run(std::ifstream& inputFile)
{
    if (!inputFile.is_open()) {
        throw std::runtime_error("BIUNetwork Error: Input file is not open");
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
        while (iss >> v)
        {
            if (!std::isfinite(v))
            {
                throw std::runtime_error("BIUNetwork Error: Invalid input value at line " + 
                                        std::to_string(currentLine) + " (NaN or Inf detected)");
            }
            values.push_back(v);
        }
        
        if (values.empty() && !line.empty())
        {
            throw std::runtime_error("BIUNetwork Error: Failed to parse values from line " + 
                                    std::to_string(currentLine));
        }

        // If no DS front-end, fall back to original single-step behavior.
        if (m_dsUnits.empty())
        {
            setInputs(values);      // original path
            update();
            showProgressBar(currentLine, totalLines);
            continue;
        }

        // Set new codes (one per DS) for this line (no tick yet).
        if (values.size() != m_dsUnits.size())
        {
            throw std::runtime_error("input line size is not equal to the number of digital to spike units");
        }
        const size_t n = values.size();
        
        for (size_t i = 0; i < n; ++i)
        {
            m_dsUnits[i].setCode(clampToCode_(values[i]));
        }

        // Gating loop: keep ticking until safety limit reached
        const std::size_t maxSafetyCycles = 32; // safety cap
        std::size_t cycles = 0;

        while (true)
        {
            std::vector<double> dsOut;
            dsOut.reserve(m_dsUnits.size());
           

            // Tick each DS; record first spike occurrence.
            for (size_t i = 0; i < m_dsUnits.size(); ++i)
            {
                bool spk = m_dsUnits[i].tick();
                dsOut.push_back(spk ? 1.0 : 0.0);
            }

            setInputs(dsOut);
            update();

            if (++cycles >= maxSafetyCycles)
            {
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

    auto totalspk = getTotalspikes();
    std::cout << "Total spike ins: " << totalspk << " " << '\n';


}

void BIUNetwork::setInputs(const std::vector<double>& inputs)
{
    if (!m_vecLayers.empty() && !m_dsUnits.empty())
    {
        const size_t n = std::min(inputs.size(), m_dsUnits.size());
        
        for (size_t i = 0; i < n; ++i)
        {
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

std::vector<std::vector<uint8_t>> BIUNetwork::update()
{
    std::vector<std::vector<uint8_t>> allSpikes;
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
            std::string spikesFile = "spikes_" + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";

            if (m_verbosity == Verbosity::Debug)
            {
                std::vector<double> Vns    = m_vecLayers[layerIdx].getVns(neuronIdx);
                std::vector<double> spikes = m_vecLayers[layerIdx].getSpikesVec(neuronIdx);
                std::vector<double> Vin    = m_vecLayers[layerIdx].getVinVec(neuronIdx);

                std::string vnsFile = "vns_" + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";
                std::string vinFile = "vin_" + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";

                std::ofstream vnsOut(vnsFile);
                std::ofstream spikesOut(spikesFile);
                std::ofstream vinOut(vinFile);

                if (vnsOut.is_open())    for (const auto& v : Vns)    vnsOut << v << '\n';
                if (spikesOut.is_open()) for (const auto& s : spikes) spikesOut << s << '\n';
                if (vinOut.is_open())    for (const auto& x : Vin)    vinOut << x << '\n';
            }
            else
            {
                // Info: only spikes
                std::vector<double> spikes = m_vecLayers[layerIdx].getSpikesVec(neuronIdx);
                std::ofstream spikesOut(spikesFile);
                if (spikesOut.is_open()) for (const auto& s : spikes) spikesOut << s << '\n';
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

double BIUNetwork::getTotalspikes()
{
    double sum = 0.0;
    for (const auto& layer : m_vecLayers) sum += layer.getTotalVINS();
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