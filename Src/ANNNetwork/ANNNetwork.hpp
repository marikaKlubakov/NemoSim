#pragma once
/**
 * @file ANNNetwork.hpp
 * @brief ANN core initialized from NetworkParameters (Y-Flash + MUX + VTC + TDC + DSA).
 *
 * Data paths:
 *  - Vector (digital emu):       y = W * x            via YFlash::step()
 *  - Bit-serial (IMC behavior):  W ⊙ bitmask → MUX → VTC → TDC (per column code)
 *
 * Construction:
 *  ANNNetwork(const NetworkParameters& params)
 *    - params.annPEs[k].yflash.{rows,cols,isSigned,Wpos,Wneg}
 *    - params.annVtc{C,Idis,Vth,T0,DtLSB}, params.annTdcBits, params.annMuxFanIn
 */

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "networkParams.hpp"
#include "ANNYFlash.hpp"

 // Adjust include path to your project layout if needed.
#include "../Common/BaseNetwork.hpp"

/* ============================= *
 *  Back-end helper components   *
 * ============================= */

 /**
  * @class ColumnMux
  * @brief Stateless N:1 multiplexer for column outputs.
  *
  * Instead of storing a selected index (mutable), the selected `sel` is supplied
  * at call site. This keeps methods const-correct inside const PEs.
  */
class ColumnMux {
public:
    explicit ColumnMux(int fanin) : n_(fanin) {}
    double routeAt(const std::vector<double>& columns, int sel) const {
        if (sel < 0 || sel >= n_) throw std::out_of_range("ColumnMux::routeAt out of range");
        return columns.at(static_cast<size_t>(sel));
    }
    int size() const { return n_; }
private:
    int n_;
};

/**
 * @class VTC
 * @brief Simple integrate-&-fire model: current → delay.
 *
 * Vc(T0) = (i_sum/C)*T0 ; dT = max(0, C*(Vc - Vth)/Idis)
 */
class VTC {
public:
    VTC(double C, double Idis, double Vth, double dt_lsb)
        : C_(C), Idis_(Idis), Vth_(Vth), dtLSB_(dt_lsb) {
    }

    double currentToDelay(double i_sum, double T0) const {
        const double Vc = (i_sum / C_) * T0;
        double dT = (C_ * (Vc - Vth_)) / Idis_;
        if (dT < 0) dT = 0;
        return dT;
    }
    double lsb() const { return dtLSB_; }

    // ---------- ADD THIS ----------
    struct Report {
        double I_phys;   // A
        double T0;       // s
        double C;        // F
        double Idis;     // A
        double Vth;      // V
        double Vc_T0;    // V at end of integration
        bool   fired;    // Vc_T0 > Vth
        double dT;       // s (post-compare delay modeled)
    };
    Report probe(double I_phys, double T0) const;
    // ------------------------------

private:
    double C_, Idis_, Vth_, dtLSB_;
};

/**
 * @class SAR_TDC
 * @brief Parametric N-bit time-to-digital converter (behavioral).
 *
 * code = round(dT / dtLSB) clipped to [0, 2^nbits-1].
 */
class SAR_TDC {
public:
    explicit SAR_TDC(int nbits) : nbits_(nbits) {}
    int quantize(double dT, double dtLSB) const;
    int bits() const { return nbits_; }
private:
    int nbits_;
};

/**
 * @class DSA
 * @brief Digital shift-and-accumulate for bit-serial pMACs (optional helper).
 *
 * For bit index b: acc += (pMAC << b). Call value() after last bit.
 */
class DSA {
public:
    explicit DSA(int out_bits) : acc_(0), out_bits_(out_bits), bit_idx_(0) {}
    void reset() { acc_ = 0; bit_idx_ = 0; }
    void accumulate(int pMAC) { acc_ += (static_cast<int64_t>(pMAC) << bit_idx_); ++bit_idx_; }
    int64_t value() const { return acc_; }
private:
    int64_t acc_;
    int     out_bits_;
    int     bit_idx_;
};

/* ============ *
 *     PE       *
 * ============ */

 /**
  * @class PE
  * @brief Processing Element: one Y-Flash array + shared mixed-signal back-end.
  */
class PE {
public:
    // Build from one PEBlock and the global ANN params.
    PE(const NetworkParameters::PEBlock& peb, const NetworkParameters& p);

    // Vector compute (digital emu): y = W * x (length(x) = cols, length(y) = rows).
    std::vector<double> compute(const std::vector<double>& input) const;

    // Bit-serial (IMC) one bit-cycle: returns a TDC code per column for the given 0/1 mask.
    std::vector<double> computeBitwise(const std::vector<std::vector<uint8_t>>& activationBits) const;

    int rows() const { return rows_; }
    int cols() const { return cols_; }

private:
    // Front-end
    ANNYFlash yflash_;
    int rows_ = 0;
    int cols_ = 0;

    // Back-end chain
    ColumnMux mux_;
    VTC       vtc_;
    SAR_TDC   tdc_;

    // Timing/quantization knobs
    double T0_s_;     ///< precharge/integration window (from params.annVtcT0)
};

/* ================= *
 *   ANNNetwork      *
 * ================= */

class ANNNetwork : public BaseNetwork {
public:
    // === NEW: construct directly from parsed NetworkParameters ===
    explicit ANNNetwork(const NetworkParameters& params);

    // BaseNetwork interface (kept)
    void run(std::ifstream& inputFile) override;  // no-op here
    //std::vector<int64_t> runIMCFromBitplaneFile(std::ifstream& in);
    void printNetworkToFile() override;           // simple stub

    // Convenience: digital vector run; one input per PE
    std::vector<float> run(const std::vector<std::vector<double>>& input);

    // Convenience: one bit-cycle IMC run on a specific PE (returns per-column TDC codes)
    std::vector<double> runBitwise(const std::vector<std::vector<uint8_t>>& activationBits, int pe_idx);

    // Optional convenience: process multiple bitplanes for one PE with DSA accumulate
    int64_t runBitSerialDSA(const std::vector<std::vector<std::vector<uint8_t>>>& bitplanes, int pe_idx);

    // Turn on verbose prints inside the IMC path (MUX/VTC/TDC per column).
    void enableIMCTrace(bool on) { imcTrace_ = on; }

private:
    std::vector<PE> m_VecPEs;

    // Copy of global ANN knobs (useful for helpers/validation)
    int    annBitSerialBits_ = 0;
    int    annDsaOutBits_ = 0;
    bool   imcTrace_ = false;
};
