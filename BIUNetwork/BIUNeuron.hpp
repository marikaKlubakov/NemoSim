#ifndef SPIKINGNEURON_H
#define SPIKINGNEURON_H


#include <vector>
class SpikingNeuron
{
public:
    SpikingNeuron(double vth, double vdd, int refractory, double cn,
        double cl, double cu, std::vector<double> weights);
    void setSynapticInputs(const std::vector<double>& inputs);
    bool update();
    double getVoltage() const;
private:
    double VTH;
    double VDD;
    int refractoryTime;
    double Cn;
    double Cl;
    double Cu;
    double Vn;
    int cyclesLeft;
    std::vector<double> synapticWeights;
    std::vector<double> synapticInputs;
};

#endif // SPIKINGNEURON_H