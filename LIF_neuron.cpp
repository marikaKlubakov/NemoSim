#include <iostream>
#include <vector>
#include <random>
#include <string>
// --------- LIF Neuron Definition ---------
class LIFNeuron {
private:
   double Cm, Cf, Vth, VDD, Vm, beta, dt, IR;
   bool spiked;
public:
   LIFNeuron(double Cm_, double Cf_, double Vth_, double VDD_, double dt_)
       : Cm(Cm_), Cf(Cf_), Vth(Vth_), VDD(VDD_), dt(dt_) {
       Vm = 0.0;
       beta = Cm / (Cm + Cf);
       spiked = false;
   }
   void update(double Iin) {
       double Vm_prime = Vm + (dt / Cm) * (Iin - IR * (Vm > Vth));
       if (Vm_prime >= Vth) {
           Vm = beta * VDD;
           spiked = true;
       } else {
           Vm = Vm_prime;
           spiked = false;
       }
   }
   double getVm() const { return Vm; }
   bool hasSpiked() const { return spiked; }
};
// --------- LIF Layer Definition ---------
class LIFLayer {
public:
   std::vector<LIFNeuron> neurons;
   std::vector<std::vector<double>> weights;
   LIFLayer(int numNeurons, double Cm, double Cf, double Vth, double VDD, double dt) {
       neurons.reserve(numNeurons);
       weights.resize(numNeurons);
       for (int i = 0; i < numNeurons; ++i) {
           neurons.emplace_back(Cm, Cf, Vth, VDD, dt);
       }
   }
   void initializeWeights(int nextLayerSize) {
       std::random_device rd;
       std::mt19937 gen(rd());
       std::uniform_real_distribution<double> dist(-1.0, 1.0);
       for (auto& row : weights) {
           row.resize(nextLayerSize);
           for (double& w : row) {
               w = dist(gen);
           }
       }
   }
};
// --------- LIF Network Definition ---------
class LIFNetwork {
private:
   std::vector<LIFLayer> layers;
   double VDD, dt;
   std::string networkType;
public:
   LIFNetwork(const std::vector<int>& layerSizes, double Cm, double Cf, double Vth, double VDD_, double dt_, const std::string& type)
       : VDD(VDD_), dt(dt_), networkType(type) {
       for (int size : layerSizes) {
           layers.emplace_back(size, Cm, Cf, Vth, VDD, dt);
       }
       for (size_t i = 0; i < layers.size() - 1; ++i) {
           layers[i].initializeWeights(layers[i + 1].neurons.size());
       }
   }
   void feedForward(const std::vector<double>& input) {
       if (input.size() != layers[0].neurons.size()) {
           std::cerr << "Input size mismatch!" << std::endl;
           return;
       }
       for (size_t i = 0; i < input.size(); ++i) {
           layers[0].neurons[i].update(input[i]);
       }
       for (size_t l = 0; l < layers.size() - 1; ++l) {
           std::vector<double> nextInputs(layers[l + 1].neurons.size(), 0.0);
           for (size_t i = 0; i < layers[l].neurons.size(); ++i) {
               if (layers[l].neurons[i].hasSpiked()) {
                   for (size_t j = 0; j < layers[l + 1].neurons.size(); ++j) {
                       nextInputs[j] += layers[l].weights[i][j] * VDD;
                   }
               }
           }
           for (size_t i = 0; i < layers[l + 1].neurons.size(); ++i) {
               layers[l + 1].neurons[i].update(nextInputs[i]);
           }
       }
   }
   void printNetworkState() const {
       std::cout << "Network Type: " << networkType << std::endl;
       for (size_t l = 0; l < layers.size(); ++l) {
           std::cout << "Layer " << l << ":" << std::endl;
           for (size_t i = 0; i < layers[l].neurons.size(); ++i) {
               std::cout << "  Neuron " << i << " Vm: " << layers[l].neurons[i].getVm();
               if (layers[l].neurons[i].hasSpiked()) std::cout << " (Spiked)";
               std::cout << std::endl;
           }
       }
       std::cout << "--------------------------------------" << std::endl;
   }
};
// --------- LIF Engine (Main Controller) ---------
class LIFEngine {
public:
   static LIFNetwork* createNetwork(const std::string& type, const std::vector<int>& layerSizes) {
       double Cm = 1.0, Cf = 0.1, Vth = 1.0, VDD = 5.0, dt = 0.01;
       if (type == "feedforward") {
           return new LIFNetwork(layerSizes, Cm, Cf, Vth, VDD, dt, "Feedforward");
       }
       else if (type == "recurrent") {
           return new LIFNetwork(layerSizes, Cm, Cf, Vth, VDD, dt, "Recurrent");
       }
       else {
           std::cerr << "Unknown network type: " << type << std::endl;
           return nullptr;
       }
   }
};
// --------- Main Simulation ---------
int main() {
   std::string networkType;
   std::cout << "Enter network type (feedforward/recurrent): ";
   std::cin >> networkType;
   std::vector<int> layerSizes;
   std::cout << "Enter number of layers: ";
   int numLayers;
   std::cin >> numLayers;
   for (int i = 0; i < numLayers; ++i) {
       int neurons;
       std::cout << "Enter neurons in layer " << i << ": ";
       std::cin >> neurons;
       layerSizes.push_back(neurons);
   }
   LIFNetwork* network = LIFEngine::createNetwork(networkType, layerSizes);
   if (!network) return 1;
   std::vector<double> input(layerSizes[0], 1.0);
   for (int t = 0; t < 100; ++t) {
       network->feedForward(input);
       network->printNetworkState();
   }
   delete network;
   return 0;
}