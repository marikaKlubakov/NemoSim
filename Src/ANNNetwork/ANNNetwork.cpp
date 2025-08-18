#include "ANNNetwork.hpp"
#include <cmath>
#include <iostream>
#include <random>

bool g_ann_imc_trace = true;

/* ============================= *
 *   Helper components bodies    *
 * ============================= */

int SAR_TDC::quantize(double dT, double dtLSB) const {
    const int maxc = (1 << nbits_) - 1;
    long long code = static_cast<long long>(std::llround(dT / dtLSB));
    if (code < 0) code = 0;
    if (code > maxc) code = maxc;
    return static_cast<int>(code);
}

/* ============ *
 *     PE       *
 * ============ */

PE::PE(const NetworkParameters::PEBlock& peb, const NetworkParameters& p)
    : yflash_(peb.yflash.isSigned
        ? ANNYFlash(peb.yflash.Wpos, peb.yflash.Wneg)
        : ANNYFlash(peb.yflash.Wpos)),
    rows_(peb.yflash.rows),
    cols_(peb.yflash.cols),
    // Column MUX: fan-in defaults to cols if not provided
    mux_(p.annMuxFanIn > 0 ? p.annMuxFanIn : peb.yflash.cols),
    // VTC / TDC from globals
    vtc_(/*C=*/p.annVtcC, /*Idis=*/p.annVtcIdis, /*Vth=*/p.annVtcVth, /*dt_lsb=*/p.annVtcDtLSB),
    tdc_(/*nbits=*/(p.annTdcBits > 0 ? p.annTdcBits : 6)),
    T0_s_(p.annVtcT0 > 0 ? p.annVtcT0 : 10e-9)
{
    // Basic sanity (non-fatal)
    if (cols_ <= 0 || rows_ <= 0) {
        std::cerr << "[PE] Warning: YFlash has non-positive dims (rows=" << rows_ << ", cols=" << cols_ << ")\n";
    }
    if (mux_.size() != cols_) {
        // It’s fine to share a smaller/larger MUX in behavioral sim; warn if mismatch.
        std::cerr << "[PE] Notice: MUX fan-in (" << mux_.size() << ") != cols (" << cols_ << ")\n";
    }
}

std::vector<double> PE::compute(const std::vector<double>& input) const {
    return yflash_.step(input);
}

std::vector<double> PE::computeBitwise(const std::vector<std::vector<uint8_t>>& activationBits) const {
    const std::vector<double> pmac_cols = yflash_.bitwise_pmac(activationBits);

    std::vector<double> tdc_codes;
    tdc_codes.reserve(pmac_cols.size());

    // Scale abstract column "current" to physical Amps (no member needed)
    const double kCurrentGainA = 7.75e-7; // 0.775 µA per unit → places typical code near mid-scale (6-bit TDC)

    for (int col = 0; col < static_cast<int>(pmac_cols.size()); ++col) {
        const double i_sum = mux_.routeAt(pmac_cols, col);     // abstract current (unitless)
        const double I_phys = kCurrentGainA * i_sum;            // <-- 1-line scale

        // (if you added VTC::probe per my last message, use it; otherwise call currentToDelay)
        const auto fire = vtc_.probe(I_phys, T0_s_);            // shows Vc, fired, dT
        const int  code = tdc_.quantize(fire.dT, vtc_.lsb());

        tdc_codes.push_back(static_cast<double>(code));

        extern bool g_ann_imc_trace;
        if (g_ann_imc_trace) {
            std::cout << "[FIRE] col=" << col
                << " I=" << fire.I_phys << "A"
                << " Vc(T0)=" << fire.Vc_T0 << "V"
                << " Vth=" << fire.Vth
                << " fired=" << (fire.fired ? "1" : "0")
                << " dT=" << fire.dT << "s"
                << " -> code=" << code << "\n";
        }
    }
    return tdc_codes;
}



/* ================= *
 *   ANNNetwork      *
 * ================= */

ANNNetwork::ANNNetwork(const NetworkParameters& params)
    : annBitSerialBits_(params.annBitSerialBits),
    annDsaOutBits_(params.annDsaOutBits)
{
    // Build PEs from params.annPEs (preferred path)
    if (!params.annPEs.empty()) {
        m_VecPEs.reserve(params.annPEs.size());
        for (const auto& peb : params.annPEs) {
            m_VecPEs.emplace_back(peb, params);
        }
    }
    // Fallback: if no <PE> blocks were parsed but legacy flat YFlash matrices exist,
    // wrap each matrix as a PE for convenience.
    else if (!params.YFlashWeights.empty()) {
        for (const auto& W : params.YFlashWeights) {
            NetworkParameters::PEBlock peb;
            peb.id = static_cast<int>(m_VecPEs.size());
            peb.yflash.rows = static_cast<int>(W.size());
            peb.yflash.cols = peb.yflash.rows ? static_cast<int>(W.front().size()) : 0;
            peb.yflash.isSigned = false;
            peb.yflash.Wpos = W;
            m_VecPEs.emplace_back(peb, params);
        }
    }

    if (m_VecPEs.empty()) {
        std::cerr << "[ANNNetwork] Warning: constructed with zero PEs. Check your XML.\n";
    }
}

//void ANNNetwork::run(std::ifstream& inputFile) {
//    if (!inputFile.good()) {
//        std::cerr << "[ANNNetwork::run] Error: input stream is not good().\n";
//        return;
//    }
//
//    // 1) Build one input vector per PE, reading exactly cols(p) numbers
//    std::vector<std::vector<double>> inputs;
//    inputs.reserve(m_VecPEs.size());
//
//    auto nextNumber = [&](double& out) -> bool {
//        std::string line;
//        while (std::getline(inputFile, line)) {
//            // trim
//            auto l = line;
//            // simple trim
//            auto isws = [](unsigned char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; };
//            while (!l.empty() && isws(l.front())) l.erase(l.begin());
//            while (!l.empty() && isws(l.back()))  l.pop_back();
//            if (l.empty()) continue;           // skip blank
//            if (!l.empty() && l[0] == '#') continue; // skip comment
//            try {
//                out = std::stod(l);
//                return true;
//            }
//            catch (...) {
//                std::cerr << "[ANNNetwork::run] Warning: non-numeric line ignored: '" << line << "'\n";
//                // keep scanning
//            }
//        }
//        return false; // EOF or no more valid numbers
//        };
//
//    size_t totalNeeded = 0;
//    for (const auto& pe : m_VecPEs) totalNeeded += static_cast<size_t>(pe.cols());
//
//    for (size_t p = 0; p < m_VecPEs.size(); ++p) {
//        const int cols = m_VecPEs[p].cols();
//        std::vector<double> vin;
//        vin.reserve(cols);
//
//        for (int c = 0; c < cols; ++c) {
//            double v;
//            if (!nextNumber(v)) {
//                std::cerr << "[ANNNetwork::run] Error: insufficient input values. "
//                    "Needed " << totalNeeded << " numeric lines total.\n";
//                return;
//            }
//            vin.push_back(v);
//        }
//        inputs.push_back(std::move(vin));
//    }
//
//    // 2) Compute using the existing vector-path run()
//    auto outputs = run(inputs);
//
//    // 3) Print a concise summary per PE (same reduction: sum of row outputs)
//    for (size_t p = 0; p < outputs.size(); ++p) {
//        std::cout << "[ANNNetwork/run] PE " << p << " -> sum = " << outputs[p] << "\n";
//    }
//
//    // 4) If there are extra numbers in the file, warn (but ignore them)
//    // Try to detect at least one extra numeric token
//    {
//        double junk;
//        if (nextNumber(junk)) {
//            std::cerr << "[ANNNetwork::run] Warning: extra input values present in file; "
//                "ignored after expected " << totalNeeded << " values.\n";
//        }
//    }
//}

void ANNNetwork::run(std::ifstream& inputFile) {
    enableIMCTrace(true);
    std::vector<int64_t> macs(m_VecPEs.size(), 0);
    if (!inputFile.good()) {
        std::cerr << "[ANNNetwork::runIMCFromBitplaneFile] bad input stream\n";
    }

    auto nextToken = [&](int& bit) -> bool {
        // read next non-comment, non-empty token ("0" or "1"), space/newline separated
        std::string tok;
        while (inputFile >> tok) {
            if (tok.size() && tok[0] == '#') {
                std::string discard;
                std::getline(inputFile, discard); // skip rest of comment line
                continue;
            }
            // trim quotes/whitespace
            while (!tok.empty() && (tok.back() == '\r' || tok.back() == '\n')) tok.pop_back();
            if (tok == "0" || tok == "1") { bit = (tok[0] == '1') ? 1 : 0; return true; }
            // ignore other tokens (blank/garbage)
        }
        return false; // EOF
        };

    for (size_t p = 0; p < m_VecPEs.size(); ++p) {
        const int rows = m_VecPEs[p].rows();
        const int cols = m_VecPEs[p].cols();
        if (rows <= 0 || cols <= 0) {
            std::cerr << "[ANNNetwork::runIMCFromBitplaneFile] PE " << p << " has invalid dims\n";
            continue;
        }

        DSA acc(annDsaOutBits_ > 0 ? annDsaOutBits_ : (annBitSerialBits_ + 8));
        for (int b = annBitSerialBits_ - 1; b >= 0; --b) { // MSB → LSB
            // read one bit-plane grid: rows × cols of 0/1
            std::vector<std::vector<uint8_t>> grid(rows, std::vector<uint8_t>(cols, 0));
            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < cols; ++c) {
                    int v;
                    if (!nextToken(v)) {
                        std::cerr << "[ANNNetwork::runIMCFromBitplaneFile] EOF while reading PE " << p
                            << " bit " << b << " grid\n";
                       // return macs;
                    }
                    grid[r][c] = static_cast<uint8_t>(v);
                }
            }

            // one IMC bit-cycle → per-column TDC codes (hits MUX → VTC → TDC)
            auto codes = m_VecPEs[p].computeBitwise(grid);

            // reduce to one pMAC for this bit (sum of codes; adapt if you want another reducer)
            int pMAC = 0;
            for (double c : codes) pMAC += static_cast<int>(c);
            acc.accumulate(pMAC);
        }
        macs[p] = acc.value();
    }

    for (size_t p = 0; p < macs.size(); ++p) {
        std::cout << "PE " << p << " IMC-MAC = " << macs[p] << "\n";
    }
}

void ANNNetwork::printNetworkToFile() {
    std::cout << "[ANNNetwork] PEs: " << m_VecPEs.size()
        << " | BitSerialBits: " << annBitSerialBits_
        << " | DSA OutBits: " << annDsaOutBits_ << "\n";
}

std::vector<float> ANNNetwork::run(const std::vector<std::vector<double>>& input) {
    std::vector<float> outputs;
    outputs.reserve(m_VecPEs.size());

    const size_t n = std::min(m_VecPEs.size(), input.size());
    for (size_t i = 0; i < n; ++i) {
        const auto y = m_VecPEs[i].compute(input[i]);
        // Simple reduction: sum row outputs; adjust to your usage if needed
        double s = 0.0;
        for (double v : y) s += v;
        outputs.push_back(static_cast<float>(s));
    }
    return outputs;
}

std::vector<double> ANNNetwork::runBitwise(const std::vector<std::vector<uint8_t>>& activationBits, int pe_idx) {
    if (pe_idx < 0 || pe_idx >= static_cast<int>(m_VecPEs.size())) {
        throw std::out_of_range("ANNNetwork::runBitwise - PE index out of range.");
    }
    return m_VecPEs[pe_idx].computeBitwise(activationBits);
}

int64_t ANNNetwork::runBitSerialDSA(
    const std::vector<std::vector<std::vector<uint8_t>>>& bitplanes, int pe_idx)
{
    if (pe_idx < 0 || pe_idx >= static_cast<int>(m_VecPEs.size())) {
        throw std::out_of_range("ANNNetwork::runBitSerialDSA - PE index out of range.");
    }
    const int bits = static_cast<int>(bitplanes.size());
    DSA acc(annDsaOutBits_ > 0 ? annDsaOutBits_ : bits + 8); // heuristic headroom

    for (int b = 0; b < bits; ++b) {
        // One bit-cycle → per-column TDC codes
        auto codes = m_VecPEs[pe_idx].computeBitwise(bitplanes[b]);

        // Reduce columns to a single pMAC for this bit (sum; choose your own reducer if needed)
        int pMAC = 0;
        for (double c : codes) pMAC += static_cast<int>(c);

        acc.accumulate(pMAC);
    }
    return acc.value();
}

VTC::Report VTC::probe(double I_phys, double T0) const {
    Report r{};
    r.I_phys = I_phys;
    r.T0 = T0;
    r.C = C_;
    r.Idis = Idis_;
    r.Vth = Vth_;

    // Integrate to Vc(T0)
    r.Vc_T0 = (I_phys / C_) * T0;

    // Our simple fire model: if Vc > Vth, emit a delay dT proportional to overdrive
    double dT = (C_ * (r.Vc_T0 - Vth_)) / Idis_;
    if (dT < 0) { dT = 0; r.fired = false; }
    else { r.fired = (r.Vc_T0 > Vth_); }
    r.dT = dT;
    return r;
}
