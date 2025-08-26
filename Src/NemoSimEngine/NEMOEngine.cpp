#include "NEMOEngine.hpp"

NEMOEngine::NEMOEngine(NetworkParameters params)
{
	switch (params.networkType)
	{
	case NetworkTypes::BIUNetworkType:
		m_pNetwork = new BIUNetwork(params);
		break;

	case NetworkTypes::LIFNetworkType:
		m_pNetwork = new LIFNetwork(params);
		break;
	case NetworkTypes::ANNNetworkType:
		m_pNetwork = new ANNNetwork(params);
		break;

	default:
		throw std::invalid_argument("Unknown network type");
	}
}

void NEMOEngine::runEngine(std::ifstream &inputFile)
{
	m_pNetwork->run(inputFile);
	m_pNetwork->printNetworkToFile();
}