#include "ANNYFlash.hpp"
#include <algorithm>
#include <numeric>

/* ============ *
 *  Constructors
 * ============ */

ANNYFlash::ANNYFlash(const std::vector<std::vector<double>>& Wpos)
    : m_Wpos(Wpos), m_rows(static_cast<int>(Wpos.size())),
    m_cols(m_rows ? static_cast<int>(Wpos.front().size()) : 0),
    m_has_signed(false)
{
    validateDims_();
}

ANNYFlash::ANNYFlash(const std::vector<std::vector<double>>& Wpos,
    const std::vector<std::vector<double>>& Wneg)
    : m_Wpos(Wpos), m_Wneg(Wneg),
    m_rows(static_cast<int>(Wpos.size())),
    m_cols(m_rows ? static_cast<int>(Wpos.front().size()) : 0),
    m_has_signed(true)
{
    if (static_cast<int>(m_Wneg.size()) != m_rows ||
        (m_rows && static_cast<int>(m_Wneg.front().size()) != m_cols)) 
    {
        throw std::invalid_argument("YFlash: Wpos and Wneg must have the same dimensions.");
    }
    validateDims_();
}

/* ============ *
 *    Public    *
 * ============ */

std::vector<double> ANNYFlash::step(const std::vector<double>& input) const {
    if (static_cast<int>(input.size()) != m_cols) 
    {
        throw std::invalid_argument("YFlash::step: input size must equal cols().");
    }

    std::vector<double> out(m_rows, 0.0);

    if (!m_has_signed) {
        for (int i = 0; i < m_rows; ++i) {
            const auto& Wi = m_Wpos[i];
            double acc = 0.0;
            for (int j = 0; j < m_cols; ++j) {
                acc += Wi[j] * input[j];
            }
            out[i] = acc;
        }
    }
    else {
        for (int i = 0; i < m_rows; ++i) {
            const auto& Wp = m_Wpos[i];
            const auto& Wn = m_Wneg[i];
            double acc = 0.0;
            for (int j = 0; j < m_cols; ++j) {
                acc += (Wp[j] - Wn[j]) * input[j];
            }
            out[i] = acc;
        }
    }

    return out;
}

std::vector<double> ANNYFlash::bitwise_pmac(const std::vector<std::vector<uint8_t>>& activationBits) const {
    // Handle supported shapes:
    //  A) [rows][cols]           → element-wise mask
    //  B) [rows][1] or [rows]()  → broadcast a single bit per row to all columns
    const bool has_rows = static_cast<int>(activationBits.size()) == m_rows;
    if (!has_rows) 
    {
        throw std::invalid_argument("YFlash::bitwise_pmac: activationBits must have rows()==rows().");
    }

    const bool broadcast_vec = isBroadcastVector_(activationBits, m_rows);
    const bool broadcast_col = isBroadcastColumn_(activationBits, m_rows);
    const bool elementwise = (!broadcast_vec && !broadcast_col);

    if (elementwise) 
    {
        // Must be full [rows][cols]
        if (activationBits.empty() || static_cast<int>(activationBits.front().size()) != m_cols) 
        {
            throw std::invalid_argument("YFlash::bitwise_pmac: expected [rows][cols] for element-wise mask.");
        }
    }

    std::vector<double> pmac(m_cols, 0.0);

    if (!m_has_signed) 
    {
        if (elementwise) 
        {
            for (int i = 0; i < m_rows; ++i) 
            {
                const auto& Wi = m_Wpos[i];
                const auto& Bi = activationBits[i];
                for (int j = 0; j < m_cols; ++j) 
                {
                    const double a = (Bi[j] ? 1.0 : 0.0);
                    pmac[j] += Wi[j] * a;
                }
            }
        }
        else if (broadcast_vec || broadcast_col) 
        {
            // single bit per row, broadcast across all columns
            for (int i = 0; i < m_rows; ++i) 
            {
                const uint8_t bit = broadcast_vec ? activationBits[i][0]
                    : (activationBits[i].empty() ? 0 : activationBits[i][0]);
                if (!bit) continue;
                const auto& Wi = m_Wpos[i];
                for (int j = 0; j < m_cols; ++j) 
                {
                    pmac[j] += Wi[j]; // multiply by 1
                }
            }
        }
    }
    else {
        if (elementwise) {
            for (int i = 0; i < m_rows; ++i) 
            {
                const auto& Wp = m_Wpos[i];
                const auto& Wn = m_Wneg[i];
                const auto& Bi = activationBits[i];
                for (int j = 0; j < m_cols; ++j) 
                {
                    const double a = (Bi[j] ? 1.0 : 0.0);
                    pmac[j] += (Wp[j] - Wn[j]) * a;
                }
            }
        }
        else if (broadcast_vec || broadcast_col) 
        {
            for (int i = 0; i < m_rows; ++i) 
            {
                const uint8_t bit = broadcast_vec ? activationBits[i][0]
                    : (activationBits[i].empty() ? 0 : activationBits[i][0]);
                if (!bit) continue;
                const auto& Wp = m_Wpos[i];
                const auto& Wn = m_Wneg[i];
                for (int j = 0; j < m_cols; ++j) 
                {
                    pmac[j] += (Wp[j] - Wn[j]); // multiply by 1
                }
            }
        }
    }

    return pmac;
}

/* ============ *
 *   Private    *
 * ============ */

void ANNYFlash::validateDims_() const 
{
    // Ensure all rows have equal length (rectangular matrix).
    if (m_rows < 0 || m_cols < 0) 
    {
        throw std::invalid_argument("YFlash: negative dimensions are invalid.");
    }
    if (m_rows == 0) return; // allow empty matrix

    const auto expected_cols = m_cols;
    for (const auto& row : m_Wpos) 
    {
        if (static_cast<int>(row.size()) != expected_cols) {
            throw std::invalid_argument("YFlash: Wpos is not rectangular.");
        }
    }
    if (m_has_signed) 
    {
        for (const auto& row : m_Wneg) 
        {
            if (static_cast<int>(row.size()) != expected_cols) {
                throw std::invalid_argument("YFlash: Wneg is not rectangular.");
            }
        }
    }
}

bool ANNYFlash::isBroadcastVector_(const std::vector<std::vector<uint8_t>>& bits, int rows) 
{
    // True when every row has exactly one element: [rows][1]
    if (static_cast<int>(bits.size()) != rows) return false;
    for (const auto& r : bits) 
    {
        if (r.size() != 1) return false;
    }
    return true;
}

bool ANNYFlash::isBroadcastColumn_(const std::vector<std::vector<uint8_t>>& bits, int rows) 
{
    // True when input looks like [rows][] (some loaders create empty inner vectors). We also
    // accept [rows] with empty inner vectors as a degenerate "all zeros" broadcast.
    if (static_cast<int>(bits.size()) != rows) return false;
    for (const auto& r : bits) 
    {
        if (!r.empty() && r.size() != 1) return false;
    }
    return true;
}
