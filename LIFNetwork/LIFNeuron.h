#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "YFlash.h"

// --------- LIF Neuron Definition ---------
class LIFNeuron 
{
private:
   double m_Cm, m_Cf, m_Vth, m_VDD, m_Vm, m_beta, m_dt, m_IR;
   bool m_spiked;
   YFlash* m_yflash;
   std::vector<double> m_vms;
public:
   LIFNeuron(double Cm_, double Cf_, double Vth_, double VDD_, double dt_, YFlash* yflash_);
   void update(double Vgs, double Vds, int row, int col);
   double getVm() const { return m_Vm; }
   bool hasSpiked() const { return m_spiked; }
   double getVDD() const { return m_VDD; }
   std::vector<double> getVms() const { return m_vms; }
};
