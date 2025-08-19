#include "BaseNetwork.hpp"
#include <iostream>

void BaseNetwork::showProgressBar(std::size_t current, std::size_t total) const
{
    if (total == 0) return;
    const int barWidth = 50;

    double progress = static_cast<double>(current) / static_cast<double>(total);
    int pos = static_cast<int>(barWidth * progress);

    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << '=';
        else if (i == pos) std::cout << '>';
        else std::cout << ' ';
    }
    std::cout << "] " << static_cast<int>(progress * 100.0) << " %\r";
    std::cout.flush();
}

std::size_t BaseNetwork::countLines(std::istream& in) const
{
    std::size_t lines = 0;
    std::string tmp;
    while (std::getline(in, tmp)) ++lines;
    in.clear();
    in.seekg(0);
    return lines;
}
