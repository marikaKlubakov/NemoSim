#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <cmath>
#include <corecrt_math_defines.h>
#include "LIFNetwork.hpp"

// --------- Main Simulation ---------
int main() 
{
   std::vector<int> layerSizes;
   std::cout << "Enter number of layers: ";
   int numLayers;
   std::cin >> numLayers;
   for (int i = 0; i < numLayers; ++i) 
   {
       int neurons;
       std::cout << "Enter neurons in layer " << i << ": ";
       std::cin >> neurons;
       layerSizes.push_back(neurons);
   }
   LIFNetwork* network = LIFEngine::createNetwork(layerSizes);
   if (!network) return 1;
   std::vector<double> input(layerSizes[0], 1.0);

   // Parameters for sine wave
   double frequency = 50.0;  // Hz
   double amplitude = 100e-12; // 100 pA
   double timeStep = 0.0001; // Smaller time step for better resolution
   double time = 0.0;
   int numSteps = 200; // Number of steps to cover 2 seconds

   // Simulate with sine wave input
   for (int t = 0; t < numSteps; ++t) 
   {
	   std::vector<double> input(layerSizes[0]);
	   for (size_t i = 0; i < input.size(); ++i) 
	   {
		   input[i] = amplitude+ amplitude * std::sin(2 * M_PI * frequency * time + M_PI *1.5);
	   }
	   network->feedForward(input);

	   time += timeStep;
   }
   network->printNetworkToFile();
   delete network;
   return 0;
}