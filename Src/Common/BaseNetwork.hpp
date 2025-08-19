#pragma once
#include <fstream>
#include <string>

class BaseNetwork
{
public:
    virtual ~BaseNetwork() {}
    virtual void run(std::ifstream& inputFile) = 0;
    virtual void printNetworkToFile() = 0;

protected:
    BaseNetwork() = default;

    // ---- Shared utilities for derived classes ----
    void showProgressBar(std::size_t current, std::size_t total) const;
    std::size_t countLines(std::istream& in) const;
};