#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "LIFNeuron.h"

LIFNeuron::LIFNeuron(double Cm_, double Cf_, double Vth_, double VDD_, double dt_, YFlash* yflash_)
	: m_Cm(Cm_), m_Cf(Cf_), m_Vth(Vth_), m_VDD(VDD_), m_dt(dt_) , m_yflash(yflash_)
{
	m_Vm = 0.0;
	m_beta = m_Cm / (m_Cm + m_Cf);
	m_spiked = false;
	m_IR = 4;
}
void LIFNeuron::update(double Vgs, double Vds, int row, int col) 
{
    double I_yflash = m_yflash->computeCurrent(Vgs, Vds, row, col);
    double Vm_prime = m_Vm + (m_dt / m_Cm) * (I_yflash - m_IR * (m_Vm > m_Vth));
    if (Vm_prime >= m_Vth) 
	{
        m_Vm = m_beta * m_VDD;
        m_spiked = true;
    } 
	else
	{
        m_Vm = Vm_prime;
        m_spiked = false;
    }

    m_vms.emplace_back(m_Vm);
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
