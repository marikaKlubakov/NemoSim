#pragma once
#include <vector>

class EnergyTable; // Forward declaration

class BIUNeuron 
{
public:
	BIUNeuron(double vth, double vdd, double refractory,
		double cn, double cu, double cpara, double rLeak,
		std::vector<double> weights, EnergyTable* energyTable); 
	void setSynapticInputs(const std::vector<double>& inputs);
	bool update();
	double getVoltage() const;
	std::vector<double> getVns() const { return m_Vns; }
	std::vector<double> getSpikesVec() const { return m_spikes; }
	std::vector<double> getVinec() const { return m_Vins; }
    void setEnergyTable(EnergyTable* table) { m_energyTable = table; }
    double getTotalSynapticEnergy() const;
    double getNeuronEnergy() const;
	double m_vin_sum = 0;
private:
	double m_VTH;
	double m_VDD;
	int m_refractoryTime;
	double m_Cn;
	double m_Cu;
	double m_Vn;
	double m_RLeak;          
	int cyclesLeft;
	std::vector<double> m_synapticWeights;
	std::vector<double> m_synapticInputs;
	std::vector<double> m_synapticEnergy;
	double m_neuronEnergy = 0;

	//output vetors for vn and spikes
	std::vector<double> m_spikes;
	std::vector<double> m_Vins;
	std::vector<double> m_Vns;
	double m_Cpara;

    EnergyTable* m_energyTable = nullptr; // Pointer to shared energy table
};
