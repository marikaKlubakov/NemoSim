#include "XMLParser.hpp"
#include <tinyxml2.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace tinyxml2;

// ---------------- utils ----------------

static std::string trim(const std::string& str)
{
    const char* whitespace = " \t\n\r\"";
    size_t start = str.find_first_not_of(whitespace);
    size_t end = str.find_last_not_of(whitespace);
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

bool XMLParser::parseDouble(XMLElement* parent, const char* tag, double& out)
{
    if (!parent)
    {
        return false;
    }
    if (auto* e = parent->FirstChildElement(tag))
    {
        return (e->QueryDoubleText(&out) == XML_SUCCESS);
    }
    return false;
}

bool XMLParser::parseInt(XMLElement* parent, const char* tag, int& out)
{
    if (!parent)
    {
        return false;
    }
    if (auto* e = parent->FirstChildElement(tag))
    {
        return (e->QueryIntText(&out) == XML_SUCCESS);
    }
    return false;
}

// ---------------- public: XML entry ----------------

bool XMLParser::parse(const std::string& filename, NetworkParameters& params)
{
    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS)
    {
        std::cerr << "Error loading XML file '" << filename << "': " << doc.ErrorStr() << std::endl;
        return false;
    }

    auto* root = doc.FirstChildElement("NetworkConfig");
    if (!root)
    {
        std::cerr << "Error: No <NetworkConfig> root element found.\n";
        return false;
    }

    const char* typeAttr = root->Attribute("type");
    if (!typeAttr)
    {
        std::cerr << "Error: <NetworkConfig> missing 'type' attribute.\n";
        return false;
    }

    auto it = StringToNetworkType.find(typeAttr);
    if (it == StringToNetworkType.end())
    {
        std::cerr << "Error: Unknown network type '" << typeAttr << "'.\n";
        return false;
    }
    params.networkType = it->second;
    std::cout << "Parsing network type: " << typeAttr << "\n";

    auto* arch = root->FirstChildElement("Architecture");

    if (auto* LIF = root->FirstChildElement("LIFNetwork"))
    {
        LIFNetworkParser(LIF, arch, params);
    }
    if (auto* BIU = root->FirstChildElement("BIUNetwork"))
    {
        BIUNetworkParser(BIU, arch, params);
    }
    if (arch)
    {
		int yFlashIdnex = 0;
        for (auto* yflashElem = arch->FirstChildElement("YFlash"); yflashElem != nullptr; yflashElem = yflashElem->NextSiblingElement("YFlash"), yFlashIdnex++)
        {
            YFlashParser(yflashElem, params, yFlashIdnex);
        }
    }

    if (auto* ANN = root->FirstChildElement("ANNNetwork"))
    {
        ANNNetworkParser(ANN, params);
        if (arch)
        {
            ANNArchitectureParser(arch, params);
        }
    }

    if (params.networkType == NetworkTypes::ANNNetworkType)
    {
        std::cout << "ANN: VDD=" << params.annVDD
            << " ClockHz=" << params.annClockHz
            << " BitSerialBits=" << params.annBitSerialBits << "\n";
        std::cout << "ANN: MUX FanIn=" << params.annMuxFanIn
            << " ShareAcrossColumns=" << (params.annMuxShareAcrossColumns ? "true" : "false") << "\n";
        std::cout << "ANN: VTC C=" << params.annVtcC
            << " Idis=" << params.annVtcIdis
            << " Vth=" << params.annVtcVth
            << " T0=" << params.annVtcT0
            << " dtLSB=" << params.annVtcDtLSB << "\n";
        std::cout << "ANN: TDC Bits=" << params.annTdcBits
            << " DSA OutBits=" << params.annDsaOutBits << "\n";
        std::cout << "ANN: PEs parsed = " << params.annPEs.size() << "\n";
    }

    return true;
}

// ---------------- public: CFG entry ----------------

static ConfigKey getConfigKey(const std::string& key)
{
    static const std::unordered_map<std::string, ConfigKey> keyMap =
    {
        {"output_directory", ConfigKey::OutputDirectory},
        {"xml_config_path", ConfigKey::XmlConfigPath},
        {"sup_xml_config_path", ConfigKey::SupXmlConfigPath},
        {"data_input_file", ConfigKey::DataInputFile},
        {"energy_table_path", ConfigKey::CsvPath},
        {"progress_interval_seconds", ConfigKey::ProgressIntervalSeconds}
    };

    auto it = keyMap.find(key);
    return it != keyMap.end() ? it->second : ConfigKey::Unknown;
}

Config XMLParser::parseConfigFromFile(const std::string& filePath)
{
    Config config;
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open file: " + filePath);
    }

    std::string line;
    while (std::getline(file, line))
    {
        size_t colon = line.find(':');
        if (colon == std::string::npos) continue;

        std::string key = trim(line.substr(0, colon));

        // Skip whitespace after colon
        size_t start = line.find_first_not_of(" \t", colon + 1);

        std::string value;
        if (line[start] == '"')
        {
            // Value is a quoted string
            size_t end = line.find('"', start + 1);
            value = line.substr(start + 1, end - start - 1);
        }
        else
        {
            // Value is a number or unquoted
            size_t end = line.find_first_of(", \t\r\n", start);
            value = line.substr(start, end - start);
        }



        switch (getConfigKey(key))
        {
        case ConfigKey::OutputDirectory:
            config.outputDirectory = value;
            break;
        case ConfigKey::XmlConfigPath:
            config.xmlConfigPath = value;
            break;
        case ConfigKey::SupXmlConfigPath:
            config.supXmlConfigPath = value;
            break;
        case ConfigKey::DataInputFile:
            config.dataInputPath = value;
            break;
        case ConfigKey::CsvPath:
            config.EnergyCsvFile = value;
            break;
        case ConfigKey::ProgressIntervalSeconds:
            config.progressIntervalSeconds = std::stoi(value);
            break;
        default:
            std::cerr << "Unknown config key: " << key << std::endl;
            break;
        }
    }
    return config;

}


// ---------------- legacy parsers (kept, minimal but safe) ----------------

void XMLParser::LIFNetworkParser(XMLElement* LIF, XMLElement* arch, NetworkParameters& params) {
    parseDouble(LIF, "Cm", params.Cm);
    parseDouble(LIF, "Cf", params.Cf);
    parseDouble(LIF, "VDD", params.VDD);
    parseDouble(LIF, "VTh", params.VTh);
    parseDouble(LIF, "dt", params.dt);
    parseDouble(LIF, "IR", params.IR);

    if (arch && params.networkType == NetworkTypes::LIFNetworkType) 
    {
        int layerIdx = 0;
        for (auto* layer = arch->FirstChildElement("Layer"); layer != nullptr; layer = layer->NextSiblingElement("Layer"), ++layerIdx) 
        {
            int size;
            if (layer->QueryIntAttribute("size", &size) == tinyxml2::XML_SUCCESS)
            { 
                params.layerSizes.push_back(size);
            }
            else
            {
                std::ostringstream oss;
                oss << "Error: Missing or invalid 'size' in LIF <Layer> at index " << layerIdx;
                throw std::runtime_error(oss.str());
            }
        }
    }
}

void XMLParser::BIUNetworkParser(XMLElement* BIU, XMLElement* arch, NetworkParameters& params) 
{
    parseDouble(BIU, "fclk", params.fclk);
    parseDouble(BIU, "VTh", params.VTh);
    parseDouble(BIU, "RLeak", params.Rleak);
    parseDouble(BIU, "VDD", params.VDD);
    parseDouble(BIU, "Cn", params.Cn);
    parseDouble(BIU, "Cu", params.Cu);
    parseDouble(BIU, "refractory", params.refractory);

    if (arch && params.networkType == NetworkTypes::BIUNetworkType)
    {
        int layerIdx = 0;
        for (auto* layer = arch->FirstChildElement("Layer"); layer != nullptr; layer = layer->NextSiblingElement("Layer"), ++layerIdx) 
        {
            int size;
            if (layer->QueryIntAttribute("size", &size) == tinyxml2::XML_SUCCESS)
            {
                params.layerSizes.push_back(size);
            }    
            else
            {
                std::ostringstream oss;
                oss << "Error: Missing or invalid 'size' in BIU <Layer> at index " << layerIdx;
                throw std::runtime_error(oss.str());
            }
            auto* synapses = layer->FirstChildElement("synapses");
            if (synapses) 
            {
                auto* weightsElem = synapses->FirstChildElement("weights");
                std::vector<std::vector<double>> layerWeights;
                if (weightsElem) 
                {
                    int rowIdx = 0;
                    for (auto* rowElem = weightsElem->FirstChildElement("row"); rowElem != nullptr; rowElem = rowElem->NextSiblingElement("row"), ++rowIdx) 
                    {
                        const char* rowText = rowElem->GetText();
                        if (!rowText) 
                        {
                            std::ostringstream oss;
                            oss << "Error: Empty <row> in weights at layer " << layerIdx << ", row " << rowIdx;
                            throw std::runtime_error(oss.str());
                        }
                        std::istringstream iss(rowText);
                        std::vector<double> rowWeights;
                        double w;
                        while (iss >> w)
                            rowWeights.push_back(w);
                        layerWeights.push_back(rowWeights);
                    }
                }
                else 
                {
                    std::ostringstream oss;
                    oss << "Error: <weights> element missing in <synapses> at layer " << layerIdx;
                    throw std::runtime_error(oss.str());
                }
                params.allWeights.push_back(layerWeights);
            }
            else 
            {
                std::ostringstream oss;
                oss << "Error: <synapses> missing in <Layer> at index " << layerIdx;
                throw std::runtime_error(oss.str());
            }
        }
    }
}

void XMLParser::YFlashParser(XMLElement* YFlash, NetworkParameters& params, int yFlashIndex)
{
    auto* weightsElem = YFlash->FirstChildElement("weights");
    std::vector<std::vector<double>> layerWeights;
    if (weightsElem)
    {
        int rowIdx = 0;
        for (auto* rowElem = weightsElem->FirstChildElement("row");
            rowElem != nullptr;
            rowElem = rowElem->NextSiblingElement("row"), ++rowIdx)
        {
            const char* rowText = rowElem->GetText();
            if (!rowText)
            {
                std::ostringstream oss;
                oss << "Error: Empty <row> in <weights> at row "
                    << rowIdx << " (YFlash index " << yFlashIndex << ")";
                throw std::runtime_error(oss.str());
            }

            std::istringstream iss(rowText);
            std::vector<double> rowWeights;
            double w;
            while (iss >> w)
                rowWeights.push_back(w);

            layerWeights.push_back(rowWeights);
        }
    }
    else
    {
        std::ostringstream oss;
        oss << "Error: <weights> element missing in <synapses> "
            << "(YFlash index " << yFlashIndex << ")";
        throw std::runtime_error(oss.str());
    }
    params.YFlashWeights.push_back(layerWeights);
}


// ---------------- NEW: ANN parsers ----------------

void XMLParser::ANNNetworkParser(XMLElement* ANN, NetworkParameters& params)
{
    // Globals
    parseDouble(ANN, "VDD", params.annVDD);
    parseDouble(ANN, "ClockHz", params.annClockHz);
    parseInt(ANN, "BitSerialBits", params.annBitSerialBits);

    // MUX
    if (auto* mux = ANN->FirstChildElement("MUX")) 
    {
        if (auto* s = mux->FirstChildElement("ShareAcrossColumns")) 
        {
            if (const char* t = s->GetText()) 
            {
                std::string v = t; std::transform(v.begin(), v.end(), v.begin(), ::tolower);
                params.annMuxShareAcrossColumns = (v == "true" || v == "1" || v == "yes");
            }
        }
        parseInt(mux, "FanIn", params.annMuxFanIn);
    }

    // VTC
    if (auto* vtc = ANN->FirstChildElement("VTC")) 
    {
        parseDouble(vtc, "C", params.annVtcC);
        parseDouble(vtc, "Idis", params.annVtcIdis);
        parseDouble(vtc, "Vth", params.annVtcVth);
        parseDouble(vtc, "T0", params.annVtcT0);
        parseDouble(vtc, "dtLSB", params.annVtcDtLSB);
    }

    // TDC
    if (auto* tdc = ANN->FirstChildElement("TDC")) 
    {
        parseInt(tdc, "Bits", params.annTdcBits);
    }

    // DSA
    if (auto* dsa = ANN->FirstChildElement("DSA")) 
    {
        parseInt(dsa, "OutBits", params.annDsaOutBits);
    }
}

void XMLParser::ANNArchitectureParser(XMLElement* arch, NetworkParameters& params)
{
    // Preferred: <PE> blocks
    for (auto* pe = arch->FirstChildElement("PE"); pe != nullptr; pe = pe->NextSiblingElement("PE")) 
    {
        ANNParsePE(pe, params);
    }

    // Fallback: allow bare <YFlash> directly under <Architecture>
    for (auto* yf = arch->FirstChildElement("YFlash"); yf != nullptr; yf = yf->NextSiblingElement("YFlash")) 
    {
        NetworkParameters::PEBlock peb;
        peb.id = static_cast<int>(params.annPEs.size());
        ANNParseYFlash(yf, peb.yflash);
        params.annPEs.push_back(std::move(peb));
    }
}

void XMLParser::ANNParsePE(XMLElement* peElem, NetworkParameters& params)
{
    NetworkParameters::PEBlock pe;
    int peIdx = static_cast<int>(params.annPEs.size());
    peElem->QueryIntAttribute("id", &pe.id);
    if (auto* yf = peElem->FirstChildElement("YFlash")) 
    {
        ANNParseYFlash(yf, pe.yflash);
        params.annPEs.push_back(std::move(pe));
    }
    else 
    {
        std::ostringstream oss;
        oss << "Error: <PE> without <YFlash> encountered at PE index " << peIdx;
        throw std::runtime_error(oss.str());
    }
}

void XMLParser::ANNParseYFlash(XMLElement* yfElem, NetworkParameters::YFlashBlock& yb)
{
    yfElem->QueryIntAttribute("rows", &yb.rows);
    yfElem->QueryIntAttribute("cols", &yb.cols);

    if (const char* s = yfElem->Attribute("signed")) 
    {
        std::string v = s; std::transform(v.begin(), v.end(), v.begin(), ::tolower);
        yb.isSigned = (v == "true" || v == "1" || v == "yes");
    }

    auto parseWeights = [](XMLElement* weightsElem, int& rowIdxOut) -> std::vector<std::vector<double>>
    {
        std::vector<std::vector<double>> W;
        if (!weightsElem) return W;
        int rowIdx = 0;
        for (auto* rowElem = weightsElem->FirstChildElement("row"); rowElem != nullptr; rowElem = rowElem->NextSiblingElement("row"), ++rowIdx) 
        {
            std::vector<double> r;
            const char* txt = rowElem->GetText();
            if (!txt) 
            {
                std::ostringstream oss;
                oss << "Error: Empty <row> in YFlash weights at row " << rowIdx;
                throw std::runtime_error(oss.str());
            }
            std::istringstream iss(txt);
            double x; while (iss >> x) r.push_back(x);
            W.push_back(std::move(r));
        }
        rowIdxOut = rowIdx;
        return W;
    };

    for (auto* w = yfElem->FirstChildElement("weights"); w != nullptr; w = w->NextSiblingElement("weights")) 
    {
        bool isPos = false, isNeg = false;

        if (const char* posAttr = w->Attribute("pos")) 
        {
            std::string v = posAttr; std::transform(v.begin(), v.end(), v.begin(), ::tolower);
            isPos = (v == "true" || v == "1" || v == "yes");
        }
        if (const char* negAttr = w->Attribute("neg")) 
        {
            std::string v = negAttr; std::transform(v.begin(), v.end(), v.begin(), ::tolower);
            isNeg = (v == "true" || v == "1" || v == "yes");
        }

        int rowIdx = 0;
        if (isPos || (!yb.isSigned && yb.Wpos.empty())) 
        {
            yb.Wpos = parseWeights(w, rowIdx);
        }
        if (isNeg) {
            yb.Wneg = parseWeights(w, rowIdx);
        }
    }

    if (!yb.Wpos.empty()) {
        int cols = static_cast<int>(yb.Wpos.front().size());
        for (size_t r = 0; r < yb.Wpos.size(); ++r) 
        {
            if (static_cast<int>(yb.Wpos[r].size()) != cols) 
            {
                std::ostringstream oss;
                oss << "Error: YFlash Wpos is not rectangular at row " << r;
                throw std::runtime_error(oss.str());
            }
        }
    }
    if (yb.isSigned && !yb.Wneg.empty() && yb.Wneg.size() != yb.Wpos.size()) 
    {
        std::ostringstream oss;
        oss << "Error: YFlash Wneg rows != Wpos rows (Wneg rows: " << yb.Wneg.size()
            << ", Wpos rows: " << yb.Wpos.size() << ")";
        throw std::runtime_error(oss.str());
    }
}
