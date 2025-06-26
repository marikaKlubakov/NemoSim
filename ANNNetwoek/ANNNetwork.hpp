#ifndef BIUNETWORK_H
#define BIUNETWORK_H
#include <iostream>

#include <vector>
#include "BIULayer.hpp"

class BIUNetwork
{
public:
	BIUNetwork(const std::vector<int>& layerSizes, double vth, double vdd, double refractory, double cn, double cu, std::vector<std::vector<std::vector<double>>> weights);
	void setInputs(const std::vector<std::vector<std::vector<double>>>& inputs);
	std::vector<std::vector<bool>> update();
	void printNetworkToFile();
private:
	std::vector<BIULayer> layers;
};

#endif // BIUNETWORK_H