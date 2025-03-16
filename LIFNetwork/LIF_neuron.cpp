#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "LIF_neuron.h"

   LIFNeuron::LIFNeuron(double Cm_, double Cf_, double Vth_, double VDD_, double dt_)
       : Cm(Cm_), Cf(Cf_), Vth(Vth_), VDD(VDD_), dt(dt_) {
       Vm = 0.0;
       beta = Cm / (Cm + Cf);
       spiked = false;
       IR = 4;
   }
   void LIFNeuron::update(double Iin)
   {
       double Vm_prime = Vm + (dt / Cm) * (Iin - IR * (Vm > Vth));
       if (Vm_prime >= Vth) 
	   {
           Vm = beta * VDD;
           spiked = true;
       } 
	   else
		{
           Vm = Vm_prime;
           spiked = false;
        }

       vms.emplace_back(Vm);
   }


// --------- LIF Engine (Main Controller) ---------
// class LIFEngine {
// public:
//    static LIFNetwork* createNetwork(const std::string& type, const std::vector<int>& layerSizes) {
//        double Cm = 1.0, Cf = 0.1, Vth = 1.0, VDD = 5.0, dt = 0.01;
//        if (type == "feedforward") {
//            return new LIFNetwork(layerSizes, Cm, Cf, Vth, VDD, dt, "Feedforward");
//        }
//        else if (type == "recurrent") {
//            return new LIFNetwork(layerSizes, Cm, Cf, Vth, VDD, dt, "Recurrent");
//        }
//        else {
//            std::cerr << "Unknown network type: " << type << std::endl;
//            return nullptr;
//        }
//    }
// };
