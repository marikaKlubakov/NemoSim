#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "LIFLayer.h"

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
