#pragma once
/**
 * @file YFlash.hpp
 * @brief Behavioral model of a Y-Flash crossbar array used for in-memory compute.
 *
 * The class supports two configurations:
 *   1) Unsigned (single-cell) weights:   W[i][j] >= 0
 *   2) Signed (dual-cell) weights:       W = W_pos - W_neg
 *
 * Exposed interfaces:
 *   - step(input):         Standard vector-matrix multiply (digital emulation).
 *   - bitwise_pmac(bits):  Bit-serial partial MACs per column (IMC-style).
 *
 * Dimensions:
 *   - rows()    → number of wordlines (input vector length for step()).
 *   - cols()    → number of bitlines (number of outputs for step(); number of columns returned).
 */

#include <cstdint>
#include <stdexcept>
#include <vector>

class ANNYFlash {
public:
    /**
     * @brief Construct an unsigned (single-cell) Y-Flash array.
     * @param Wpos  Weight matrix of size [rows][cols]. Values are treated as non-negative.
     */
    explicit ANNYFlash(const std::vector<std::vector<double>>& Wpos);

    /**
     * @brief Construct a signed (dual-cell) Y-Flash array with W = Wpos - Wneg.
     * @param Wpos  Positive sub-array weights [rows][cols].
     * @param Wneg  Negative sub-array weights [rows][cols]. Must match Wpos dimensions.
     */
    ANNYFlash(const std::vector<std::vector<double>>& Wpos,
        const std::vector<std::vector<double>>& Wneg);

    /// @return number of rows (wordlines).
    int rows() const noexcept { return rows_; }

    /// @return number of columns (bitlines).
    int cols() const noexcept { return cols_; }

    /// @return true if the instance models signed weights (dual cell).
    bool isSigned() const noexcept { return has_signed_; }

    /**
     * @brief Dense vector × matrix multiply (digital emulation of the array).
     *
     * Computes y = W * x, where:
     *   - x has length == cols()
     *   - y has length == rows()
     *
     * @param input  Input vector x of length cols().
     * @return Output vector y of length rows().
     * @throws std::invalid_argument on size mismatch.
     */
    std::vector<double> step(const std::vector<double>& input) const;

    /**
     * @brief Bit-serial partial MAC (pMAC) per column for a single bit-cycle.
     *
     * Conceptually, for each column j we compute:
     *   pmac[j] = sum_i ( W_eff[i][j] * a_bit[i][j] )
     * where W_eff = Wpos - Wneg if signed, else Wpos; and a_bit ∈ {0,1}.
     *
     * @param activationBits  0/1 mask with shape:
     *        - Preferred: [rows()][cols()] (element-wise gating)
     *        - Supported broadcast: [rows()][1] or [rows()] (same bit per row across all columns)
     *
     * @return Vector of length cols(): one partial sum (treated as a “column current”) per column.
     * @throws std::invalid_argument on incompatible shapes.
     */
    std::vector<double> bitwise_pmac(const std::vector<std::vector<uint8_t>>& activationBits) const;

private:
    // Core storage
    std::vector<std::vector<double>> Wpos_;
    std::vector<std::vector<double>> Wneg_;   // only used when has_signed_ == true

    int rows_ = 0;
    int cols_ = 0;
    bool has_signed_ = false;

    // Internal helpers
    void validateDims_() const;
    static bool isBroadcastVector_(const std::vector<std::vector<uint8_t>>& bits, int rows);
    static bool isBroadcastColumn_(const std::vector<std::vector<uint8_t>>& bits, int rows);
};
