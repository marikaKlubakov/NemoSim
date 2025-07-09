#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <cmath>
#include <fstream>
#include "networkParams.hpp"
#include "LIFNetwork.hpp"
#include "BIUNetwork.hpp"
#include "BaseNetwork.hpp"

class NEMOEngine
{
public:
    NEMOEngine(NetworkParameters params);
    //void createNetwork(NetworkParameters params);
    void runEngine(std::ifstream &inputFile);
private:
	BaseNetwork* m_pNetwork = nullptr;
};
