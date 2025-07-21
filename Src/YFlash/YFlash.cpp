
#include "YFlash.hpp"
#include <iostream>
#include <stdexcept>

YFlash::YFlash(const std::vector<std::vector<double>>& input_matrix)
    : weights(input_matrix), rows(input_matrix.size()),
      cols(input_matrix.empty() ? 0 : input_matrix[0].size()) {}

std::vector<double> YFlash::step(const std::vector<double>& voltages) const 
{
    if (voltages.size() != cols) 
    {
        throw std::invalid_argument("Voltage vector size does not match matrix column size.");
    }

    std::vector<double> currents(rows, 0.0);
    for (size_t i = 0; i < rows; ++i) 
    {
        for (size_t j = 0; j < cols; ++j) 
        {
            currents[i] += weights[i][j] * voltages[j];
        }
    }
    return currents;
}

void YFlash::print() const 
{
    for (const auto& row : weights) 
    {
        for (double val : row) 
        {
            std::cout << val << " ";
        }
        std::cout << "";
    }
}
