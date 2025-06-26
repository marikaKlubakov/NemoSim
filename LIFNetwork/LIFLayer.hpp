
#ifndef LIFLAYER_HPP
#define LIFLAYER_HPP

#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "LIFNeuron.hpp"
#include "YFlash.hpp"
// --------- LIF Layer Definition ---------
class LIFLayer 
{
public:
   LIFLayer(int numNeurons, double Cm, double Cf, double Vth, double VDD, double dt, double IR);
   void initializeWeights(YFlash* yflash) ;
   unsigned int getLayerSize() const;
   void updateLayer(std::vector<double>& input);
   void step(std::vector<double>& nextInputs);
   double getVm(int index) const { return m_neurons[index].getVm(); }
   std::vector<double> getVms(int index) const { return m_neurons[index].getVms(); }
   std::vector<double> getIinVec(int index) const { return m_neurons[index].getIinVec(); }
   std::vector<double> getVoutVec(int index) const { return m_neurons[index].getVoutVec(); }
   bool hasSpiked(int index) const { return m_neurons[index].hasSpiked(); }
private:
	std::vector<LIFNeuron> m_neurons;
	std::vector<std::vector<double>> m_weights;
	YFlash* m_yflash = nullptr;
};
#endif