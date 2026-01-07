#pragma once
#include <vector>

class EnergyTable; // Forward declaration

class BIUNeuron 
{
public:
	BIUNeuron(double vth, double vdd, double refractory,
		double cn, double cu, double cpara, double rLeak,
		const std::vector<double>& weights, EnergyTable* energyTable); 
	void setSynapticInputs(const std::vector<double>& inputs);
	bool update();
	double getVoltage() const;
	std::vector<double> getVns() const { return m_Vns; }
	std::vector<double> getSpikesVec() const { return m_spikes; }
	std::vector<double> getVinVec() const { return m_Vins; }
    void setEnergyTable(EnergyTable* table) { m_energyTable = table; }
    double getTotalSynapticEnergy() const;
    double getNeuronEnergy() const;
	double m_vin_sum = 0;
private:
	double m_VTH;
	double m_VDD = 1.2;
	int m_refractoryTime;
	double m_Cn = 170e-15;
	double m_Cu = 0.6e-15;
	double m_Vn;
	double m_RLeak = 1e6;
	double m_Cpara = 5.5e-15;
	int cyclesLeft;
	std::vector<double> m_synapticWeights;
	std::vector<double> m_synapticInputs;
	std::vector<double> m_synapticEnergy;
	double m_neuronEnergy = 0;

	//output vetors for vn and spikes
	std::vector<double> m_spikes;
	std::vector<double> m_Vins;
	std::vector<double> m_Vns;


    EnergyTable* m_energyTable = nullptr; // Pointer to shared energy table
};
