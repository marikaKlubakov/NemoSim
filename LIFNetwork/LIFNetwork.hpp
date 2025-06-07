#include <iostream>

#ifndef LIFNETWORK_HPP
#define LIFNETWORK_HPP

#include <vector>
#include <random>
#include <string>
#include "LIFLayer.hpp"
#include "../networkParams.hpp"

// --------- LIF Network Definition ---------
class LIFNetwork 
{
public:
   LIFNetwork(const std::vector<int>& layerSizes, double Cm, double Cf, double Vth, double VDD_, double dt_, double IR_);
   void feedForward(const std::vector<double>& input) ;
   void printNetworkState(int timestep) const ;
   void printNetworkToFile();
private:
	std::vector<LIFLayer> m_layers;
	double m_VDD, m_dt;
	std::vector<double> vms;
};
#endif