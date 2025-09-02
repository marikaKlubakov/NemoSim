#pragma once
/**
 * @file YFlash.hpp
 * @brief Behavioral model of a single (unsigned) Y-Flash crossbar array for in-memory compute.
 *
 * This class models a 2D array of weights, supporting digital emulation of vector-matrix multiplication.
 * It is intended for use in neuromorphic and in-memory computing simulations.
 *
 * Key features:
 *  - Construction from a rectangular matrix of weights (all non-negative).
 *  - Digital emulation of vector-matrix multiplication: y = W * x.
 *  - Access to the underlying weights and array dimensions.
 *  - Utility to print the weight matrix.
 */

#include <vector>
#include <cstddef>
#include <stdexcept>
#include <iostream>

class YFlash {
public:
    /**
     * @brief Construct a Y-Flash array from a 2D weight matrix.
     * @param input_matrix  Rectangular matrix [rows][cols] of weights (must be non-empty).
     * @throws std::invalid_argument if the matrix is empty or not rectangular.
     */
    YFlash(const std::vector<std::vector<double>>& input_matrix, int index = -1); // Add index parameter

    /**
     * @brief Perform a digital vector-matrix multiplication: y = W * x.
     * @param voltages  Input vector of length equal to the number of columns.
     * @return Output vector of length equal to the number of rows.
     * @throws std::invalid_argument if input size does not match the number of columns.
     */
    std::vector<double> step(const std::vector<double>& voltages) const;

    /**
     * @brief Print the weight matrix to std::cout.
     */
    void print() const;

    void setIndex(int index) { m_index = index; }
    int getIndex() const { return m_index; }

    /// The underlying weight matrix [rows][cols].
    std::vector<std::vector<double>> m_weights;

    /// Number of rows (wordlines).
    size_t m_rows;

    /// Number of columns (bitlines).
    size_t m_cols;
private:
    int m_index = -1; // Add this member
};

