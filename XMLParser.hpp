#ifndef XMLPARSER_H
#define XMLPARSER_H
#include "tinyxml2.h"
#include <vector>
#include <string>
#include "LIFNetwork.hpp"
class XMLParser {
public:
	static bool parse(const std::string& filename, NetworkParameters& params);
};

#endif // XMLPARSER_H