#include "BIUNetwork.hpp"
#include "EnergyTable.hpp"
#include <fstream>
#include <sstream>

BIUNetwork::BIUNetwork(NetworkParameters params)
{
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
            m_vecLayers.emplace_back(params.layerSizes[i], params.VTh, params.VDD, params.refractory, params.Cn, params.Cu, params.Rleak, params.allWeights[i], m_energyTable);
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
    int t = 0;

    while (std::getline(inputFile, line)) {
        ++currentLine;
        ++t;

        // Parse all values in the line
        std::istringstream iss(line);
        std::vector<double> values;
        double value;
        while (iss >> value) {
            values.push_back(value);
        }

        setInputs(values);

        auto spikes = update();

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
        // Route through DS: file provides codes (ideally 0..255 for 8-bit)
        std::vector<double> dsOut;
        const size_t n = std::min(inputs.size(), m_dsUnits.size());
        dsOut.reserve(n);
        for (size_t i = 0; i < n; ++i) 
        {
            unsigned int code = clampToCode_(inputs[i]);
            m_dsUnits[i].setCode(code);
            bool spk = m_dsUnits[i].tick();
            dsOut.push_back(spk ? 1.0 : 0.0);
        }
        // If fewer inputs provided than DS units, pad with zeros
        while (dsOut.size() < m_dsUnits.size()) dsOut.push_back(0.0);
        m_vecLayers[0].setInputs(dsOut);
    } 
    else 
    {
        // Fallback: original behavior (no DS)
        m_vecLayers[0].setInputs(inputs);
    }
}

std::vector<std::vector<bool>> BIUNetwork::update()
{
	std::vector<std::vector<bool>> allSpikes;
	std::vector<std::vector<double>> previousLayerOutput;
	for (size_t i = 0; i < m_vecLayers.size(); ++i) 
	{
		// If not the first layer, use spikes from previous layer as inputs
		if (i > 0)
		{
			std::vector<double> inputs;
			for (bool spike : allSpikes[i - 1]) 
			{
				inputs.push_back(spike ? 1.0 : 0.0);  // Convert spike to input current
			}
		
			m_vecLayers[i].setInputs(inputs);
		}
		auto spikes = m_vecLayers[i].update();
		allSpikes.push_back(spikes);
	}
	return allSpikes;
}

void BIUNetwork::printNetworkToFile()
{
    for (size_t layerIdx = 0; layerIdx < m_vecLayers.size(); ++layerIdx) {
        size_t numNeurons = m_vecLayers[layerIdx].getLayerSize();

        for (size_t neuronIdx = 0; neuronIdx < numNeurons; ++neuronIdx) {
            std::vector<double> Vns = m_vecLayers[layerIdx].getVns(neuronIdx);
            std::vector<double> spikes = m_vecLayers[layerIdx].getSpikesVec(neuronIdx);
            std::vector<double> Vin = m_vecLayers[layerIdx].getVinVec(neuronIdx);

            std::string vnsFile = "vns_" + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";
            std::string spikesFile = "spikes_" + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";
            std::string vinFile = "vin_" + std::to_string(layerIdx) + "_" + std::to_string(neuronIdx) + ".txt";

            std::ofstream vnsOut(vnsFile);
            std::ofstream spikesOut(spikesFile);
            std::ofstream vinOut(vinFile);

            if (vnsOut.is_open()) {
                for (const auto& value : Vns) {
                    vnsOut << value << std::endl;
                }
                vnsOut.close();
            }

            if (spikesOut.is_open()) {
                for (const auto& value : spikes) {
                    spikesOut << value << std::endl;
                }
                spikesOut.close();
            }

            if (vinOut.is_open()) {
                for (const auto& value : Vin) {
                    vinOut << value << std::endl;
                }
                vinOut.close();
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
    for (size_t i = 0; i < inputCount; ++i)
    {
        m_dsUnits.emplace_back(m_dsClockMHz, m_dsBitWidth, m_dsMode);
        m_dsUnits.back().setCode(0);
    }
}

unsigned int BIUNetwork::clampToCode_(double x) const
{
    if (m_dsBitWidth >= 31) return 0; // safety
    unsigned int maxCode = (1u << m_dsBitWidth) - 1u; // = 255 for 8-bit
    if (x < 0.0) return 0u;
    if (x > (double)maxCode) return maxCode;
    return static_cast<unsigned int>(x); // assume file already gives integers 0..255
}