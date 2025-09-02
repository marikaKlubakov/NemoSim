#include "YFlash.hpp"
#include <iostream>
#include <stdexcept>
#include <sstream>

/**
 * @brief Construct a Y-Flash array from a 2D weight matrix.
 *        Validates that the matrix is non-empty and rectangular.
 */
YFlash::YFlash(const std::vector<std::vector<double>>& input_matrix, int index)
    : m_index(index)
{
    if (input_matrix.empty())
    {
        std::ostringstream oss;
        oss << "YFlash[" << m_index << "]: input matrix must not be empty.";
        throw std::invalid_argument(oss.str());
    }
    size_t expected_cols = input_matrix[0].size();
    if (expected_cols == 0)
    {
        std::ostringstream oss;
        oss << "YFlash[" << m_index << "]: input matrix must have at least one column.";
        throw std::invalid_argument(oss.str());
    }
    for (size_t i = 1; i < input_matrix.size(); ++i)
    {
        if (input_matrix[i].size() != expected_cols)
        {
            std::ostringstream oss;
            oss << "YFlash[" << m_index << "]: row " << i
                << " has " << input_matrix[i].size()
                << " columns, expected " << expected_cols << ".";
            throw std::invalid_argument(oss.str());
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
    if (voltages.size() != m_rows)
    {
        std::ostringstream oss;
        oss << "YFlash[" << m_index << "]: input vector size (" << voltages.size()
            << ") does not match number of rows (" << m_rows << ").";
        throw std::invalid_argument(oss.str());
    }

    std::vector<double> currents(m_cols, 0.0);
    for (size_t i = 0; i < m_rows; ++i)
    {
        for (size_t j = 0; j < m_cols; ++j)
        {
            currents[j] += m_weights[i][j] * voltages[i];
        }
    }
    return currents;
}

/**
 * @brief Print the weight matrix to std::cout.
 */
void YFlash::print() const
{
    std::cout << "YFlash[" << m_index << "] weights:\n";
    for (const auto& row : m_weights)
    {
        for (double val : row)
        {
            std::cout << val << " ";
        }
        std::cout << "";
    }
}
