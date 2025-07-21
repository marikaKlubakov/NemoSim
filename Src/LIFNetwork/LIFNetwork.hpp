#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <sstream>
#include <string>
#include "LIFLayer.hpp"
#include "../NemoSimEngine/networkParams.hpp"
#include "YFlash.hpp"
#include "../Common/BaseNetwork.hpp"
// --------- LIF Network Definition ---------
class LIFNetwork : public BaseNetwork
{
public:
   LIFNetwork(NetworkParameters params);
   void run(std::ifstream& inputFile) override;
   void feedForward(std::vector<double>& input);
   void printNetworkState(int timestep) const;
   void printNetworkToFile();
private:
	std::vector<LIFLayer> m_layers;
	double m_VDD, m_dt;
	std::vector<double> vms;
	std::vector<YFlash> m_yflashVec;
};