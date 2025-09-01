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

#include "../NemoSimEngine/networkParams.hpp"
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
class ColumnMux
{
public:
    explicit ColumnMux(int fanin) : m_fanin(fanin) {}
    double routeAt(const std::vector<double>& columns, int sel) const 
    {
        if (sel < 0 || sel >= m_fanin) throw std::out_of_range("ColumnMux::routeAt out of range");
        return columns.at(static_cast<size_t>(sel));
    }
    int size() const { return m_fanin; }
private:
    int m_fanin;
};

/**
 * @class VTC
 * @brief Simple integrate-&-fire model: current → delay.
 *
 * Vc(T0) = (i_sum/C)*T0 ; dT = max(0, C*(Vc - Vth)/Idis)
 */
class VTC 
{
public:
    VTC(double C, double Idis, double Vth, double dt_lsb)
        : m_C(C), m_Idis(Idis), m_Vth(Vth), m_dtLSB(dt_lsb) 
    {}

    double currentToDelay(double i_sum, double T0) const 
    {
        const double Vc = (i_sum / m_C) * T0;
        double dT = (m_C * (Vc - m_Vth)) / m_Idis;
        if (dT < 0) dT = 0;
        return dT;
    }
    double lsb() const { return m_dtLSB; }

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
    double m_C, m_Idis, m_Vth, m_dtLSB;
};

/**
 * @class SAR_TDC
 * @brief Parametric N-bit time-to-digital converter (behavioral).
 *
 * code = round(dT / dtLSB) clipped to [0, 2^nbits-1].
 */
class SAR_TDC {
public:
    explicit SAR_TDC(int nbits) : m_nbits(nbits) {}
    int quantize(double dT, double dtLSB) const;
    int bits() const { return m_nbits; }
private:
    int m_nbits;
};

/**
 * @class DSA
 * @brief Digital shift-and-accumulate for bit-serial pMACs (optional helper).
 *
 * For bit index b: acc += (pMAC << b). Call value() after last bit.
 */
class DSA {
public:
    explicit DSA(int out_bits) : m_acc(0), m_out_bits(out_bits), m_bit_idx(0) {}
    void reset() { m_acc = 0; m_bit_idx = 0; }
    void accumulate(int pMAC) { m_acc += (static_cast<int64_t>(pMAC) << m_bit_idx); ++m_bit_idx; }
    int64_t value() const { return m_acc; }
private:
    int64_t m_acc;
    int     m_out_bits;
    int     m_bit_idx;
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

    int rows() const { return m_rows; }
    int cols() const { return m_cols; }

private:
    // Front-end
    ANNYFlash m_yflash;
    int m_rows = 0;
    int m_cols = 0;

    // Back-end chain
    ColumnMux m_mux;
    VTC       m_vtc;
    SAR_TDC   m_tdc;

    // Timing/quantization knobs
    double m_T0_s;     ///< precharge/integration window (from params.annVtcT0)
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
    void enableIMCTrace(bool on) { m_imcTrace = on; }

private:
    std::vector<PE> m_VecPEs;

    // Copy of global ANN knobs (useful for helpers/validation)
    int    m_annBitSerialBits = 0;
    int    m_annDsaOutBits = 0;
    bool   m_imcTrace = false;
};
