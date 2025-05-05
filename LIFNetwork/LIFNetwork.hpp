#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "LIFLayer.hpp"

struct NetworkParameters
{
	double Cm, Cf, VDD, VTh, dt, IR;
	double gm, req, CGBr, CGBi, CGSr, CGSi, CGDr, CGDi, CDBr, CDBi, CMOS;
	std::vector<int> layerSizes;
};
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
// --------- LIF Engine (Main Controller) ---------
class LIFEngine 
{
public:
   static LIFNetwork* createNetwork(NetworkParameters params)
   {
       return new LIFNetwork(params.layerSizes, params.Cm, params.Cf, params.VTh, params.VDD, params.dt, params.IR);
   }
};