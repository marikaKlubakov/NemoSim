#ifndef BIUNEURON_HPP
#define BIUNEURON_HPP
#include <vector>
class BIUNeuron {
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
};
#endif // BIUNEURON_HPP