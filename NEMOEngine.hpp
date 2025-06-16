#ifndef NEMOENGINE_H
#define NEMOENGINE_H
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <cmath>
#include <fstream>
#include "networkParams.hpp"
#include "LIFNetwork.hpp"
#include "BIUNetwork.hpp"

class NEMOEngine
{
public:
    NEMOEngine(NetworkParameters params);
    void createLIFNetwork(NetworkParameters params);
    void createBIUNetwork(NetworkParameters params);
    void runEngine(std::ifstream &inputFile);
    void runLIFEngine(std::ifstream &inputFile);
    void runBIUEngine(std::ifstream &inputFile);
private:
    BIUNetwork* biunetwork = nullptr;
    LIFNetwork* lifnetwork = nullptr;
};
#endif