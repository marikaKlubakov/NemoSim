
#ifndef LIFNEURON_HPP
#define LIFNEURON_HPP

#include <iostream>
#include <vector>
#include <random>
#include <string>
// --------- LIF Neuron Definition ---------
class LIFNeuron 
{
public:
   LIFNeuron(double Cm_, double Cf_, double Vth_, double VDD_, double dt_, double IR_);
   void update(double Iin);
   double getVm() const { return m_Vm; }
   bool hasSpiked() const { return m_spiked; }
   double getVDD() const { return m_VDD; }
   std::vector<double> getVms() const { return m_vms; }
   std::vector<double> getIinVec() const { return m_Iin; }
   std::vector<double> getVoutVec() const { return m_vout; }
private:
	double m_Cm, m_Cf, m_Vth, m_VDD, m_Vm, m_beta, m_dt, m_IR, m_lastVout;
	bool m_spiked;
	std::vector<double> m_vms;
	std::vector<double> m_Iin;
	std::vector<double> m_vout;
};
#endif