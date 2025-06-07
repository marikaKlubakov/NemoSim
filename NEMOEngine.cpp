#include "NEMOEngine.hpp"

NEMOEngine::NEMOEngine(NetworkParameters params)
{
	if (params.networkType == "BIUNetwork")
	{
		createBIUNetwork(params);
	}
	else if (params.networkType == "LIFNetwork")
	{
		createLIFNetwork(params);
	}
}

void NEMOEngine::createLIFNetwork(NetworkParameters params)
{
    lifnetwork = new LIFNetwork(params.layerSizes, params.Cm, params.Cf, params.VTh, params.VDD, params.dt, params.IR);
}

void NEMOEngine::createBIUNetwork(NetworkParameters params)
{   //const std::vector<int>& layerSizes, double vth, double vdd, int refractory, double cn, double cu, std::vector<double> weights
    biunetwork = new BIUNetwork(params.layerSizes, params.VTh, params.VDD, params.refractory, params.Cn, params.Cu,  params.allWeights);
}

void NEMOEngine::runEngine(std::ifstream &inputFile)
{
    if (biunetwork)
    {
        runBIUEngine(inputFile);
    }

    if (lifnetwork)
    {
        runLIFEngine(inputFile);
    }
}

void NEMOEngine::runLIFEngine(std::ifstream &inputFile)
{
	std::string line;

	if (inputFile.is_open())
	{
		while (std::getline(inputFile, line))
		{
			double value = std::stod(line); // Convert string to double
			std::vector<double> input;
			input.push_back(value);
			lifnetwork->feedForward(input);
		}
		inputFile.close();
	}
	else
	{
		std::cerr << "Unable to open file" << std::endl;
	}
	inputFile.close();

	lifnetwork->printNetworkToFile();
}

void NEMOEngine::runBIUEngine(std::ifstream &inputFile)
{
	std::string line;
	int t = 0;
	if (inputFile.is_open())
	{
		while (std::getline(inputFile, line))
		{// Format input to match the required structure
			double value = std::stod(line); // Convert string to double
			std::vector<double> input;
			std::vector<std::vector<std::vector<double>>> currentInput = { { { value } } };
			biunetwork->setInputs(currentInput);
			auto spikes = biunetwork->update();
			std::cout << "Timestep " << t << ": ";
			for (const auto& layerSpikes : spikes)
			{
				for (bool spike : layerSpikes)
				{
					std::cout << (spike ? "1" : "0") << " ";
				}
			}
			std::cout << std::endl;
			
			
		}
		inputFile.close();
	}
	else
	{
		std::cerr << "Unable to open file" << std::endl;
	}
	inputFile.close();

	biunetwork->printNetworkToFile();
}
