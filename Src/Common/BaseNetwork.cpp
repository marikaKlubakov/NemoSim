#include "BaseNetwork.hpp"
#include <iostream>

#include <iostream>
#include <string>
#include <algorithm> // std::min

void BaseNetwork::showProgressBar(std::size_t current, std::size_t total) const
{
    if (total == 0) return;

    // Clamp current to total
    if (current > total) current = total;

    // Only redraw when the percentage changes (biggest win)
    static int lastPercent = -1;

    const int barWidth = 50;
    const double progress = static_cast<double>(current) / static_cast<double>(total);
    const int percent = static_cast<int>(progress * 100.0);

    if (percent == lastPercent) return;
    lastPercent = percent;

    const int pos = static_cast<int>(barWidth * progress);

    std::string bar;
    bar.reserve(barWidth);

    for (int i = 0; i < barWidth; ++i)
        bar += (i < pos) ? '=' : (i == pos) ? '>' : ' ';

    // Print once (no per-character streaming)
    std::cout << "\r[" << bar << "] " << percent << " %";

    // Flush only at the end (or if you really need live updates, add std::flush here)
    if (current == total) {
        std::cout << "\n";
        std::cout.flush();
        lastPercent = -1; // reset for next run
    }
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
