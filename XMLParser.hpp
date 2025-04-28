#ifndef XMLPARSER_H
#define XMLPARSER_H
#include "tinyxml2.h"
#include <vector>
#include <string>

struct NetworkParameters {
	double Cm, Cf, VDD, VTh, dt;
	double gm, req, CGBr, CGBi, CGSr, CGSi, CGDr, CGDi, CDBr, CDBi, CMOS;
	std::vector<int> layerSizes;
};

class XMLParser {
public:
	static bool parse(const std::string& filename, NetworkParameters& params);
};

#endif // XMLPARSER_H