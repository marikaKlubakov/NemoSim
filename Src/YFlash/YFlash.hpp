#pragma once

#include <vector>

class YFlash 
{
public:
	YFlash(const std::vector<std::vector<double>>& input_matrix);
	std::vector<double> step(const std::vector<double>& voltages) const;
	void print() const;
private:
    std::vector<std::vector<double>> weights;
    size_t rows, cols;
};

