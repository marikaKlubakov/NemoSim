#ifndef SPIKINGNEURON_H
#define SPIKINGNEURON_H

#include <vector>

class SpikingNeuron {
private:
	double Vn;           // Neuron membrane voltage
	double VTH;          // Voltage threshold for spike
	double VDD;          // Supply voltage
	int refractoryTime;  // Number of cycles for refractory period
	int cyclesLeft;      // Remaining refractory cycles
	double Cn, Ctotal;   // Capacitances
	std::vector<double> synapticWeights;
	std::vector<double> synapticInputs;

public:
	SpikingNeuron(double vth, double vdd, int refractory, double cn, double ctotal, std::vector<double> weights);
	void setSynapticInputs(const std::vector<double>& inputs);
	bool update();
	double getVoltage() const;
};

#endif // SPIKINGNEURON_H