#include "ANNYFlash.hpp"
#include <algorithm>
#include <numeric>

/* ============ *
 *  Constructors
 * ============ */

ANNYFlash::ANNYFlash(const std::vector<std::vector<double>>& Wpos)
    : Wpos_(Wpos), rows_(static_cast<int>(Wpos.size())),
    cols_(rows_ ? static_cast<int>(Wpos.front().size()) : 0),
    has_signed_(false)
{
    validateDims_();
}

ANNYFlash::ANNYFlash(const std::vector<std::vector<double>>& Wpos,
    const std::vector<std::vector<double>>& Wneg)
    : Wpos_(Wpos), Wneg_(Wneg),
    rows_(static_cast<int>(Wpos.size())),
    cols_(rows_ ? static_cast<int>(Wpos.front().size()) : 0),
    has_signed_(true)
{
    if (static_cast<int>(Wneg_.size()) != rows_ ||
        (rows_ && static_cast<int>(Wneg_.front().size()) != cols_)) {
        throw std::invalid_argument("YFlash: Wpos and Wneg must have the same dimensions.");
    }
    validateDims_();
}

/* ============ *
 *    Public    *
 * ============ */

std::vector<double> ANNYFlash::step(const std::vector<double>& input) const {
    if (static_cast<int>(input.size()) != cols_) {
        throw std::invalid_argument("YFlash::step: input size must equal cols().");
    }

    std::vector<double> out(rows_, 0.0);

    if (!has_signed_) {
        for (int i = 0; i < rows_; ++i) {
            const auto& Wi = Wpos_[i];
            double acc = 0.0;
            for (int j = 0; j < cols_; ++j) {
                acc += Wi[j] * input[j];
            }
            out[i] = acc;
        }
    }
    else {
        for (int i = 0; i < rows_; ++i) {
            const auto& Wp = Wpos_[i];
            const auto& Wn = Wneg_[i];
            double acc = 0.0;
            for (int j = 0; j < cols_; ++j) {
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
    const bool has_rows = static_cast<int>(activationBits.size()) == rows_;
    if (!has_rows) {
        throw std::invalid_argument("YFlash::bitwise_pmac: activationBits must have rows()==rows().");
    }

    const bool broadcast_vec = isBroadcastVector_(activationBits, rows_);
    const bool broadcast_col = isBroadcastColumn_(activationBits, rows_);
    const bool elementwise = (!broadcast_vec && !broadcast_col);

    if (elementwise) {
        // Must be full [rows][cols]
        if (activationBits.empty() || static_cast<int>(activationBits.front().size()) != cols_) {
            throw std::invalid_argument("YFlash::bitwise_pmac: expected [rows][cols] for element-wise mask.");
        }
    }

    std::vector<double> pmac(cols_, 0.0);

    if (!has_signed_) {
        if (elementwise) {
            for (int i = 0; i < rows_; ++i) {
                const auto& Wi = Wpos_[i];
                const auto& Bi = activationBits[i];
                for (int j = 0; j < cols_; ++j) {
                    const double a = (Bi[j] ? 1.0 : 0.0);
                    pmac[j] += Wi[j] * a;
                }
            }
        }
        else if (broadcast_vec || broadcast_col) {
            // single bit per row, broadcast across all columns
            for (int i = 0; i < rows_; ++i) {
                const uint8_t bit = broadcast_vec ? activationBits[i][0]
                    : (activationBits[i].empty() ? 0 : activationBits[i][0]);
                if (!bit) continue;
                const auto& Wi = Wpos_[i];
                for (int j = 0; j < cols_; ++j) {
                    pmac[j] += Wi[j]; // multiply by 1
                }
            }
        }
    }
    else {
        if (elementwise) {
            for (int i = 0; i < rows_; ++i) {
                const auto& Wp = Wpos_[i];
                const auto& Wn = Wneg_[i];
                const auto& Bi = activationBits[i];
                for (int j = 0; j < cols_; ++j) {
                    const double a = (Bi[j] ? 1.0 : 0.0);
                    pmac[j] += (Wp[j] - Wn[j]) * a;
                }
            }
        }
        else if (broadcast_vec || broadcast_col) {
            for (int i = 0; i < rows_; ++i) {
                const uint8_t bit = broadcast_vec ? activationBits[i][0]
                    : (activationBits[i].empty() ? 0 : activationBits[i][0]);
                if (!bit) continue;
                const auto& Wp = Wpos_[i];
                const auto& Wn = Wneg_[i];
                for (int j = 0; j < cols_; ++j) {
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

void ANNYFlash::validateDims_() const {
    // Ensure all rows have equal length (rectangular matrix).
    if (rows_ < 0 || cols_ < 0) {
        throw std::invalid_argument("YFlash: negative dimensions are invalid.");
    }
    if (rows_ == 0) return; // allow empty matrix

    const auto expected_cols = cols_;
    for (const auto& row : Wpos_) {
        if (static_cast<int>(row.size()) != expected_cols) {
            throw std::invalid_argument("YFlash: Wpos is not rectangular.");
        }
    }
    if (has_signed_) {
        for (const auto& row : Wneg_) {
            if (static_cast<int>(row.size()) != expected_cols) {
                throw std::invalid_argument("YFlash: Wneg is not rectangular.");
            }
        }
    }
}

bool ANNYFlash::isBroadcastVector_(const std::vector<std::vector<uint8_t>>& bits, int rows) {
    // True when every row has exactly one element: [rows][1]
    if (static_cast<int>(bits.size()) != rows) return false;
    for (const auto& r : bits) {
        if (r.size() != 1) return false;
    }
    return true;
}

bool ANNYFlash::isBroadcastColumn_(const std::vector<std::vector<uint8_t>>& bits, int rows) {
    // True when input looks like [rows][] (some loaders create empty inner vectors). We also
    // accept [rows] with empty inner vectors as a degenerate "all zeros" broadcast.
    if (static_cast<int>(bits.size()) != rows) return false;
    for (const auto& r : bits) {
        if (!r.empty() && r.size() != 1) return false;
    }
    return true;
}
