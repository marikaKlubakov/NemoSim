#pragma once
#include <vector>
#include <random>
#include "YFlash.hpp"

//#define PES_VECTOR_SIZE 4;

class PE 
{
	std::vector<std::vector<float>> weights;
	std::vector<YFlash> m_VecYflashs;
public:
	PE(int inputSize, int outputSize);
	std::vector<float> compute(const std::vector<float>& input) const;
};

class ANNNetwork 
{
	std::vector <PE> m_VecPEs;
public:
	ANNNetwork();

	std::vector<float> run(const std::vector<std::vector<float>>& input);
};