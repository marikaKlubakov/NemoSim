#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "LIFLayer.hpp"

// --------- LIF Network Definition ---------
class LIFNetwork 
{

public:
   LIFNetwork(const std::vector<int>& layerSizes, double Cm, double Cf, double Vth, double VDD_, double dt_);

   void feedForward(const std::vector<double>& input) ;
   void printNetworkState(int timestep) const ;
   void printNetworkToFile();

private:
	std::vector<LIFLayer> m_layers;
	double m_VDD, m_dt;
	std::vector<double> vms;
};
// --------- LIF Engine (Main Controller) ---------
class LIFEngine 
{
public:
   static LIFNetwork* createNetwork( const std::vector<int>& layerSizes) 
   {
       double Cm = 1e-12, Cf = 0.01e-12, Vth = 0.6, VDD = 5.0, dt = 0.01;

       return new LIFNetwork(layerSizes, Cm, Cf, Vth, VDD, dt);
   }
};