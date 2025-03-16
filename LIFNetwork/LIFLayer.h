#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "LIF_neuron.h"

// --------- LIF Layer Definition ---------
class LIFLayer {

public:
  
   LIFLayer(int numNeurons, double Cm, double Cf, double Vth, double VDD, double dt) ;
   void initializeWeights(int nextLayerSize) ;
   unsigned int getLayerSize() const;
   void updateLayer(const std::vector<double>& input);
   void step(std::vector<double>& nextInputs);
   double getVm(int index) const { return m_neurons[index].getVm(); }
   std::vector<double> getVms(int index) const { return m_neurons[index].getVms(); }
   bool hasSpiked(int index) const { return m_neurons[index].hasSpiked(); }
private:
	std::vector<LIFNeuron> m_neurons;
	std::vector<std::vector<double>> m_weights;
};
