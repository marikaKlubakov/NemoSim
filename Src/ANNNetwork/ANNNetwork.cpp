#include "ANNNetwork.hpp"
#include "YFlash.hpp"
#include <stdexcept>
#include <cmath>

ANNNetwork::ANNNetwork()
{
	for (int i = 0; i < 4; ++i) 
	{
		m_VecPEs.push_back(PE(64,16));
	}

}

std::vector<float> ANNNetwork::run(const std::vector<std::vector<float>>& input)
{
	std::vector<std::vector<float>> outputs;
	for (int i = 0; i < m_VecPEs.size(); ++i)
	{
		outputs.push_back(m_VecPEs[i].compute(input[i]));
	}
	std::vector<float> merged(outputs.size());
	for (int i = 0; i < outputs.size(); ++i)
	{
		for (int j = 0; j < m_VecPEs.size(); ++j)
		{
			merged[i] += outputs[j][i] / 4.0f;
		}
	}
	return merged;
}

PE::PE(int inputSize, int outputSize)
{
		std::default_random_engine gen;
		std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
		weights.resize(outputSize, std::vector<float>(inputSize));
		for (auto& row : weights)
		{
			for (auto& w : row)
			{
				w = dist(gen);
			}
		}
}

inline float relu(float x)
{
	return std::max(0.0f, x);
}
inline std::vector<float> reluVec(const std::vector<float>& x)
{
	std::vector<float> out;
	for (auto val : x) out.push_back(relu(val));
	return out;
}

std::vector<float> PE::compute(const std::vector<float>& input) const
{
	std::vector<float> output(weights.size(), 0.0f);
	for (size_t i = 0; i < weights.size(); ++i)
		for (size_t j = 0; j < input.size(); ++j)
			output[i] += weights[i][j] * input[j];
	return reluVec(output);
}