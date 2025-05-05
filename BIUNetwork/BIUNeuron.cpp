#include "BIUNeuron.hpp"

SpikingNeuron::SpikingNeuron(double vth, double vdd, int refractory, double cn,
    double cl, double cu, std::vector<double> weights)
    : VTH(vth), VDD(vdd), refractoryTime(refractory),
    Cn(cn), Cl(cl), Cu(cu), synapticWeights(weights)
{
    Vn = VDD / 2;
    cyclesLeft = 0;
    synapticInputs.resize(weights.size(), 0.0);
}
void SpikingNeuron::setSynapticInputs(const std::vector<double>& inputs)
{
    synapticInputs = inputs;
}
bool SpikingNeuron::update()
{
    if (cyclesLeft > 0)
    {
        Vn = VDD / 2;
        cyclesLeft--;
        return false;
    }
    double weightedSum = 0;
    double dynamicCap = 0;
    for (size_t i = 0; i < synapticWeights.size(); ++i)
    {
        double Vin = synapticInputs[i];
        double W = synapticWeights[i];
        weightedSum += W * Vin * VDD;
        dynamicCap += Cu * W * Vin;
    }
    double Ctotal = Cn + Cl + dynamicCap;
    Vn = (Cn / Ctotal) * Vn + (1.0 / Ctotal) * weightedSum;
    if (Vn >= VTH)
    {
        Vn = VDD / 2;
        cyclesLeft = refractoryTime;
        return true;
    }
    return false;
}
double SpikingNeuron::getVoltage() const
{
    return Vn;
}