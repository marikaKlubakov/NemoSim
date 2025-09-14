#pragma once
#include <iostream>

#include <vector>
#include "BIULayer.hpp"
#include "../Common/BaseNetwork.hpp"
#include "../NemoSimEngine/networkParams.hpp"

class EnergyTable; // Forward declaration

class BIUNetwork : public BaseNetwork
{
public:
	BIUNetwork(NetworkParameters params);
	void run(std::ifstream& inputFile) override;
	void printNetworkToFile() override;
	double getTotalNeuronsEnergy();
	double getTotalSynapsesEnergy();
private:
	std::vector<BIULayer> m_vecLayers;
	void setInputs(const std::vector<std::vector<std::vector<double>>>& inputs);
	std::vector<std::vector<bool>> update();
	EnergyTable* m_energyTable = nullptr; // Pointer to energy table for energy calculations
};
