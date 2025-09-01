#include "YFlash.hpp"
#include <iostream>
#include <stdexcept>

/**
 * @brief Construct a Y-Flash array from a 2D weight matrix.
 *        Validates that the matrix is non-empty and rectangular.
 */
YFlash::YFlash(const std::vector<std::vector<double>>& input_matrix)
{
    if (input_matrix.empty()) 
    {
        throw std::invalid_argument("YFlash: input matrix must not be empty.");
    }
    size_t expected_cols = input_matrix[0].size();
    if (expected_cols == 0) 
    {
        throw std::invalid_argument("YFlash: input matrix must have at least one column.");
    }
    for (size_t i = 1; i < input_matrix.size(); ++i) 
    {

        if (input_matrix[i].size() != expected_cols) {
            throw std::invalid_argument("YFlash: all rows in the input matrix must have the same number of columns.");
        }
    }
    m_weights = input_matrix;
    m_rows = input_matrix.size();
    m_cols = expected_cols;
}

/**
 * @brief Perform a digital vector-matrix multiplication: y = W * x.
 *        Throws if input vector size does not match the number of columns.
 */
std::vector<double> YFlash::step(const std::vector<double>& voltages) const 
{

    std::vector<double> currents(m_rows, 0.0);
    for (size_t i = 0; i < m_rows; ++i)
    {
        for (size_t j = 0; j < m_cols; ++j)
        {
            currents[i] += m_weights[i][j] * voltages[j];
        }
    }
    return currents;
}

/**
 * @brief Print the weight matrix to std::cout.
 */
void YFlash::print() const 
{
    for (const auto& row : m_weights) 
    {
        for (double val : row) 
        {
            std::cout << val << " ";
        }
        std::cout << "";
    }
}
