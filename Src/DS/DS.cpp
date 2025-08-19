#include "DS.hpp"
#include <stdexcept>

DS::DS(double clkFreqMHz, unsigned int bw, Mode m)
	: clockFrequencyMHz(clkFreqMHz), bitWidth(bw), digitalCode(0), counter(0),
	spikeWidthNs(100.0), minSpikeIntervalNs(200.0), lastSpikeTimeNs(-200.0), currentTimeNs(0.0),
	mode(m)
{
	if (clockFrequencyMHz <= 0) throw std::invalid_argument("Clock frequency must be positive.");
	clockPeriodNs = 1000.0 / clockFrequencyMHz;
	updateThreshold();
}

void DS::setCode(unsigned int code)
{
	if (code >= (1u << bitWidth)) throw std::out_of_range("Digital code out of range.");
	digitalCode = code;
	updateThreshold();
}

void DS::reset()
{
	counter = 0;
	currentTimeNs = 0.0;
	lastSpikeTimeNs = -minSpikeIntervalNs;
}

void DS::updateThreshold()
{
	if (mode == ThresholdMode)
	{
		unsigned int maxValue = (1u << bitWidth);
		threshold = maxValue - digitalCode;
	}
	else if (mode == FrequencyMode)
	{
		unsigned int base = 16;
		threshold = 1 + (base - digitalCode);
	}
}

bool DS::tick()
{
	currentTimeNs += clockPeriodNs;
	counter++;
	if (counter >= threshold)
	{
		counter = 0;
		lastSpikeTimeNs = currentTimeNs;
		return true;
	}
	return false;
}

bool DS::isSpikeActive() const
{
	return (currentTimeNs - lastSpikeTimeNs) < spikeWidthNs;
}

double DS::getSpikePeriodNs() const
{
	return clockPeriodNs * threshold;
}

double DS::getSpikeRateMHz() const
{
	return 1000.0 / getSpikePeriodNs();
}

void DS::setMode(Mode m)
{
	mode = m;
	updateThreshold();
}
