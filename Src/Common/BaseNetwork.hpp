#pragma once
// --------- LIF Network Definition ---------
class BaseNetwork
{
public:
	virtual  ~BaseNetwork(){}
    virtual void run(std::ifstream& inputFile) = 0;
    virtual  void printNetworkToFile() = 0;

protected:
	BaseNetwork() = default;
};
