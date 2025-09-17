#pragma once
#include <string>
#include <vector>
#include <unordered_map>

/* =========================================================
   Network types (extended with ANNNetworkType)
   ========================================================= */
enum class NetworkTypes {
    LIFNetworkType,
    BIUNetworkType,
    ANNNetworkType   // <-- added
};

/* =========================================================
   Parameters (kept all your existing fields; only added ANN)
   ========================================================= */
struct NetworkParameters {
    // ---- existing fields (unchanged) ----
    NetworkTypes networkType;
    std::string neuronEnergyCsvPath;
    std::string synapsesEnergyCsvPath;
    // LIF / BIU common
    double Cm = 0.0;
    double Cf = 0.0;
    double VDD = 1.0;
    double VTh = 0.0;
    double dt = 0.0;
    double IR = 0.0;

    // BIU-specific
    double fclk = 0.0;
    double Rleak = 0.0;
    double Cn = 0.0;
    double Cu = 0.0;
    double refractory = 0.0;

    // ---------- NEW: BIU per-neuron overrides (per layer) ----------
    // If empty for a given layer, BIULayer should fall back to uniform VTh/refractory.
    // Size invariants (when present):
    //   biuNeuronVTh[i].size()        == layerSizes[i]
    //   biuNeuronRefractory[i].size() == layerSizes[i]
    //   biuNeuronRLeak[i].size()      == layerSizes[i]
    std::vector<std::vector<double>> biuNeuronVTh;
    std::vector<std::vector<int>>    biuNeuronRefractory;
    std::vector<std::vector<double>> biuNeuronRLeak;
    // Topology info
    std::vector<int> layerSizes;

    // BIU synapses
    std::vector<std::vector<std::vector<double>>> allWeights;

    // Legacy “flat” YFlash matrices under <Architecture> (kept)
    std::vector<std::vector<std::vector<double>>> YFlashWeights;

    // =================================================================
    //                       NEW: ANN parameters
    // (Added at the end; no changes to existing fields/types above.)
    // =================================================================

    // Global ANN
    double annVDD = 1.0;
    double annClockHz = 0.0;
    int    annBitSerialBits = 0;

    // Back-end chain
    // MUX
    bool   annMuxShareAcrossColumns = true;
    int    annMuxFanIn = 0;

    // VTC
    double annVtcC = 0.0;
    double annVtcIdis = 0.0;
    double annVtcVth = 0.0;
    double annVtcT0 = 0.0;
    double annVtcDtLSB = 0.0;

    // TDC
    int    annTdcBits = 0;

    // DSA
    int    annDsaOutBits = 0;

    // Architecture: a list of PEs each with one Y-Flash
    struct YFlashBlock {
        int rows = 0;
        int cols = 0;
        bool isSigned = false;  // true → W = Wpos - Wneg
        std::vector<std::vector<double>> Wpos;
        std::vector<std::vector<double>> Wneg; // optional
    };
    struct PEBlock {
        int id = -1;
        YFlashBlock yflash;
    };
    std::vector<PEBlock> annPEs;
};

/* =========================================================
   ConfigKey and Config (kept here, with csvPath preserved)
   ========================================================= */
enum class ConfigKey {
    OutputDirectory,
    XmlConfigPath,
    SupXmlConfigPath,
    DataInputFile,
    NeuronEnergyCsvPath,
    SynapsesEnergyCsvPath,
    ProgressIntervalSeconds,
    Unknown
};

struct Config {
    std::string outputDirectory;
    std::string xmlConfigPath;
    std::string supXmlConfigPath;
    std::string dataInputPath;
    std::string neuronEnergyCsvPath;
    std::string synapsesEnergyCsvPath;
    int         progressIntervalSeconds = 30;
};

/* =========================================================
   Mapping string → enum (kept and extended with ANNNetwork)
   ========================================================= */
static const std::unordered_map<std::string, NetworkTypes> StringToNetworkType = {
    {"LIFNetwork", NetworkTypes::LIFNetworkType},
    {"BIUNetwork", NetworkTypes::BIUNetworkType},
    {"ANNNetwork", NetworkTypes::ANNNetworkType}
};

static const std::unordered_map<std::string, ConfigKey> StringToConfigKey = {
    {"OutputDirectory",        ConfigKey::OutputDirectory},
    {"XmlConfigPath",          ConfigKey::XmlConfigPath},
    {"SupXmlConfigPath",       ConfigKey::SupXmlConfigPath},
    {"DataInputFile",          ConfigKey::DataInputFile},
    {"NeuronEnergyTablePath",                ConfigKey::NeuronEnergyCsvPath},  // <-- preserved
    {"SynapsesEnergyTablePath",                ConfigKey::SynapsesEnergyCsvPath},  // <-- preserved
    {"ProgressIntervalSeconds",ConfigKey::ProgressIntervalSeconds}
};
