#include <iostream>
#include "NielNetwork.hpp"
#include "NielNeuron.hpp"
int main() {
	
	SpikingNeuron singleNeuron();
	std::vector<int> layerSizes = { 3, 3 }; // Two layers with 3 neurons each
	std::vector<double> weights = { 0.5, 0.3, 0.2 }; // Example weights
	SpikingNetwork network(layerSizes, 1.0, 1.8, 3, 0.5, 1.0, weights);
	std::vector<std::vector<std::vector<double>>> inputPatterns = {
		{{1, 0, 1}, {0, 1, 1}, {1, 1, 1}},
		{{0, 0, 0}, {1, 0, 1}, {0, 1, 0}},
		{{1, 1, 0}, {0, 1, 1}, {1, 0, 1}}
	};
	for (int t = 0; t < 10; ++t) {
		std::vector<std::vector<std::vector<double>>> currentInputs = { inputPatterns[t % inputPatterns.size()] };
		network.setInputs(currentInputs);
		auto spikes = network.update();
		std::cout << "Time step " << t << ":" << std::endl;
		for (size_t i = 0; i < spikes.size(); ++i) {
			std::cout << " Layer " << i << ": ";
			for (bool spike : spikes[i]) {
				std::cout << (spike ? "1 " : "0 ");
			}
			std::cout << std::endl;
		}
	}
	return 0;
}