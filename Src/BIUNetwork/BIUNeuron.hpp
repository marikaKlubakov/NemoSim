#pragma once
#include <vector>

class EnergyTable; // Forward declaration

class BIUNeuron 
{
public:
	BIUNeuron(double vth, double vdd, double refractory, double cn, double cu, std::vector<double> weights);
	void setSynapticInputs(const std::vector<double>& inputs);
	bool update();
	double getVoltage() const;
	std::vector<double> getVns() const { return m_Vn; }
	std::vector<double> getSpikesVec() const { return m_spikes; }
	std::vector<double> getVinec() const { return m_Vin; }
private:
	double VTH;
	double VDD;
	int refractoryTime;
	double Cn;
	double Cu;
	double Vn;
	int cyclesLeft;
	std::vector<double> synapticWeights;
	std::vector<double> synapticInputs;

	//output vetors for vn and spikes
	std::vector<double> m_spikes;
	std::vector<double> m_Vin;
	std::vector<double> m_Vn;

	// Additional members
	std::vector<int> synapseSpikeCounts; // spike count per synapse (for spike rate)
	std::vector<double> synapseEnergyConsumed; // total energy per synapse

	// Add a static pointer to the shared energy table:
	static const EnergyTable* energyTable;
};
