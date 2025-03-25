#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <cmath>
#include "LIFNetwork.h"

// --------- Main Simulation ---------
int main() 
{
   std::string networkType;
   std::cout << "Enter network type (feedforward/recurrent): ";
   std::cin >> networkType;
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
   LIFNetwork* network = LIFEngine::createNetwork(networkType, layerSizes);
   if (!network)
   {
	   return 1;
   }
   std::vector<double> input(layerSizes[0], 1.0);

   // Parameters for sine wave
   double frequency = 1.0;  // Hz
   double amplitude = 2.0;
   double timeStep = 0.01;
   double time = 0.0;
   // Simulate with sine wave input
   for (int t = 0; t < 1000000; ++t) 
   {
	   std::vector<double> input(layerSizes[0]);
	   for (size_t i = 0; i < input.size(); ++i) 
	   {
		   //input[i] = amplitude * std::sin(2 * 3.14159265358979323846 * frequency * time);
		   input[i] = t % 2;
	   }
	   network->feedForward(input);
	   
	   time += timeStep* 0.000001;
   }
   network->printNetworkToFile();
   delete network;
   return 0;
}