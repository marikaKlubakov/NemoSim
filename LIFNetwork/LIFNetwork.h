#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "LIFLayer.h"

// --------- LIF Network Definition ---------
class LIFNetwork {

public:
	LIFNetwork(const std::vector<int>& layerSizes, double Cm, double Cf, double Vth, double VDD_, double dt_,
		double req_, double gm_, double CGBr_, double CGBi_, double CGSr_, double CGSi_,
		double CGDr_, double CGDi_, double CDBr_, double CDBi_, double CMOS_);

   void feedForward(const std::vector<double>& input) ;
   void printNetworkState(int timestep) const ;
   void printNetworkToFile();

private:
	std::vector<LIFLayer> m_layers;
	YFlash yflash;
	double m_VDD, m_dt;
	std::vector<double> vms;
};
// --------- LIF Engine (Main Controller) ---------
class LIFEngine {
public:
   static LIFNetwork* createNetwork(const std::string& type, const std::vector<int>& layerSizes) {
       double Cm = 0.000001, Cf = 0.000000001, Vth = 1.0, VDD = 5.0, dt = 0.000001;
       if (type == "feedforward") {
           return new LIFNetwork(layerSizes, Cm, Cf, Vth, VDD, dt, "Feedforward");
       }
       else if (type == "recurrent") {
           return new LIFNetwork(layerSizes, Cm, Cf, Vth, VDD, dt, "Recurrent");
       }
       else {
           std::cerr << "Unknown network type: " << type << std::endl;
           return nullptr;
       }
   }
};