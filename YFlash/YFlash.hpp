
#ifndef YFLASH_HPP
#define YFLASH_HPP

#include <vector>

class YFlash {
private:
    std::vector<std::vector<double>> weights;
    size_t rows, cols;

public:
    YFlash(const std::vector<std::vector<double>>& input_matrix);
    std::vector<double> step(const std::vector<double>& voltages) const;
    void print() const;
};

#endif // YFLASH_HPP
