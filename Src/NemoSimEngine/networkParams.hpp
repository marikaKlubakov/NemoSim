#pragma once

#include <vector>
#include <string>
#include <unordered_map>

enum NetworkTypes
{
	BIUNetworkType,
	LIFNetworkType
};
struct NetworkParameters
{
	double Cm, Cf, VDD = 1.2, VTh, dt, IR, Cn = 1e-12, Cu = 4e-15, Rleak = 1e6, fclk = 1e7;
	double gm, req, CGBr, CGBi, CGSr, CGSi, CGDr, CGDi, CDBr, CDBi, CMOS;
	double refractory;
	NetworkTypes networkType;
	std::vector<int> layerSizes;
	std::vector<std::vector<std::vector<double>>> allWeights;
	std::vector<std::vector<std::vector<double>>> YFlashWeights;
};


const std::unordered_map<NetworkTypes, std::string> NetworkTypeToString = 
{
	{NetworkTypes::BIUNetworkType, "BIUNetwork"},
	{NetworkTypes::LIFNetworkType, "LIFNetwork"}
};


// String to enum
const std::unordered_map<std::string, NetworkTypes> StringToNetworkType = 
{
	{"BIUNetwork", NetworkTypes::BIUNetworkType},
	{"LIFNetwork", NetworkTypes::LIFNetworkType}
};

struct Config 
{
	std::string outputDirectory;
	std::string xmlConfigPath;
	std::string supXmlConfigPath;
	std::string dataInputPath;
	int progressIntervalSeconds;
};


enum class ConfigKey 
{
	OutputDirectory,
	XmlConfigPath,
	SupXmlConfigPath,
	DataInputFile,
	ProgressIntervalSeconds,
	Unknown
};
