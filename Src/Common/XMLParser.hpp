#pragma once
#include <string>
#include <vector>
#include "LIFNetwork.hpp"

namespace tinyxml2 { class XMLElement; }

/**
 * @brief XML + CFG parser for NemoSim networks.
 *
 * Supports:
 *  - <LIFNetwork> ... (legacy)
 *  - <BIUNetwork> ... (legacy)
 *  - <ANNNetwork> + <Architecture><PE><YFlash>...</YFlash></PE> (new)
 *
 * Also parses a sidecar config file (key=value), using ConfigKey and the
 * StringToConfigKey map defined in networkParams.hpp.
 */
class XMLParser {
public:
    // Main XML parse entry
    bool parse(const std::string& filename, NetworkParameters& params);

    // Parse .cfg or .ini-style key=value file into Config
    Config parseConfigFromFile(const std::string& filePath);

private:
    // ------- existing parsers (kept; implemented here in a minimal, safe way) -------
    void LIFNetworkParser(tinyxml2::XMLElement* LIF, tinyxml2::XMLElement* arch, NetworkParameters& params);
    void BIUNetworkParser(tinyxml2::XMLElement* BIU, tinyxml2::XMLElement* arch, NetworkParameters& params);
    void YFlashParser(tinyxml2::XMLElement* YFlash, NetworkParameters& params, int yFlashIdnex);

    // --------------------- NEW: ANN parsers ---------------------
    void ANNNetworkParser(tinyxml2::XMLElement* ANN, NetworkParameters& params);
    void ANNArchitectureParser(tinyxml2::XMLElement* arch, NetworkParameters& params);
    void ANNParsePE(tinyxml2::XMLElement* peElem, NetworkParameters& params);
    void ANNParseYFlash(tinyxml2::XMLElement* yfElem, NetworkParameters::YFlashBlock& yb, int pe_id);

    // --------------------- small helpers ------------------------
    static bool parseDouble(tinyxml2::XMLElement* parent, const char* tag, double& out);
    static bool parseInt(tinyxml2::XMLElement* parent, const char* tag, int& out);
};
