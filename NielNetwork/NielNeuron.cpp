#include "NielNeuron.hpp"

SpikingNeuron::SpikingNeuron(double vth, double vdd, int refractory, double cn, double ctotal, std::vector<double> weights)
	: VTH(vth), VDD(vdd), refractoryTime(refractory), Cn(cn), Ctotal(ctotal), synapticWeights(weights) {
	Vn = VDD / 2; // Initialize voltage
	cyclesLeft = 0;
	synapticInputs.resize(weights.size(), 0.0);
}

void SpikingNeuron::setSynapticInputs(const std::vector<double>& inputs) {
	synapticInputs = inputs;
}

bool SpikingNeuron::update() {
	if (cyclesLeft > 0) {
		Vn = VDD / 2; // Hold voltage at reset level
		cyclesLeft--;
		return false;
	}
	double weightedSum = 0;
	for (size_t i = 0; i < synapticWeights.size(); ++i) {
		weightedSum += (synapticWeights[i] * synapticInputs[i] * VDD);
	}
	Vn = (Cn / Ctotal) * Vn + (1.0 / Ctotal) * weightedSum;
	if (Vn >= VTH) {
		Vn = VDD / 2; // Reset neuron voltage
		cyclesLeft = refractoryTime;
		return true; // Spike occurred
	}
	return false; // No spike
}

double SpikingNeuron::getVoltage() const {
	return Vn;
}