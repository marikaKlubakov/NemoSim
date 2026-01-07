#pragma once
#include <iostream>

#include <vector>
#include "BIULayer.hpp"
#include "../Common/BaseNetwork.hpp"
#include "../NemoSimEngine/networkParams.hpp"
#include "../DS/DS.hpp"

class EnergyTable; // Forward declaration

class BIUNetwork : public BaseNetwork
{
public:
	explicit BIUNetwork(NetworkParameters params);
	~BIUNetwork();
	void run(std::ifstream& inputFile) override;
	void printNetworkToFile() override;
	double getTotalNeuronsEnergy();
	double getTotalSynapsesEnergy();
	double getTotalspikes();
private:
	Verbosity m_verbosity = Verbosity::Info; // NEW
	std::vector<BIULayer> m_vecLayers;
	void setInputs(const std::vector<double>& inputs);
	std::vector<std::vector<uint8_t>> update();
	EnergyTable* m_energyTable = nullptr; // Pointer to energy table for energy calculations
	// ===== DS front-end (one DS per input channel) =====
	std::vector<DS> m_dsUnits;            // created to match layer-0 fan-in
	unsigned int m_dsBitWidth = 4;        // default: 4-bit codes (0..255)
	double m_dsClockMHz = 10.0;           // default DS clock
	DS::Mode m_dsMode = DS::ThresholdMode;
	void initFrontEndDS_(size_t inputCount);
	unsigned int clampToCode_(double x) const; // map file value -> [0..(1<<bw)-1]
};
