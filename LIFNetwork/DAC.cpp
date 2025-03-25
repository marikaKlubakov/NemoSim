#include "DAC.h"
#include <cmath>
#include <stdexcept>

DAC::DAC(float referenceCurrent) : referenceCurrent(referenceCurrent) {}

float DAC::convert(int digitalInput) 
{
    if (digitalInput < 0 || digitalInput > 15)		
	{
        throw std::out_of_range("Digital input must be between 0 and 15");
    }
    // Apply logarithmic conversion
    return referenceCurrent * std::log2(digitalInput + 1);
}