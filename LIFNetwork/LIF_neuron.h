#include <iostream>
#include <vector>
#include <random>
#include <string>
// --------- LIF Neuron Definition ---------
class LIFNeuron {
private:
   double Cm, Cf, Vth, VDD, Vm, beta, dt, IR;
   bool spiked;
   std::vector<double> vms;
public:
   LIFNeuron(double Cm_, double Cf_, double Vth_, double VDD_, double dt_);
   void update(double Iin);
   double getVm() const { return Vm; }
   bool hasSpiked() const { return spiked; }
   double getVDD() const { return VDD; }
   std::vector<double> getVms() const { return vms; }
};
