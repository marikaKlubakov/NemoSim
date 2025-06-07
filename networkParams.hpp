#ifndef NETWORKPARAMS_H
#define NETWORKPARAMS_H
#include <vector>
#include <string>
struct NetworkParameters
{
	double Cm, Cf, VDD = 1.2, VTh, dt, IR, Cn = 1e-12, Cu = 4e-15, Rleak = 1e6, fclk = 1e7;
	double gm, req, CGBr, CGBi, CGSr, CGSi, CGDr, CGDi, CDBr, CDBi, CMOS;
	double refractory;
	std::string networkType;
	std::vector<int> layerSizes;
	std::vector<std::vector<std::vector<double>>> allWeights;
};
#endif // NETWORKPARAMS_H