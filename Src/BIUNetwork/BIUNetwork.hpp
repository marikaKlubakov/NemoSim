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
private:
	std::vector<BIULayer> m_vecLayers;
	void setInputs(const std::vector<std::vector<std::vector<double>>>& inputs);
	std::vector<std::vector<bool>> update();
	EnergyTable* energyTable = nullptr; // Pointer to energy table for energy calculations
};
