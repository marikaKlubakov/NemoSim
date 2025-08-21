#include "DS.hpp"
#include <stdexcept>

DS::DS(double clkFreqMHz, unsigned int bw, Mode m)
	: m_clockFrequencyMHz(clkFreqMHz), m_bitWidth(bw), m_digitalCode(0), m_counter(0),
	m_spikeWidthNs(100.0), m_minSpikeIntervalNs(200.0), m_lastSpikeTimeNs(-200.0), m_currentTimeNs(0.0),
	m_mode(m)
{
	if (m_clockFrequencyMHz <= 0) throw std::invalid_argument("Clock frequency must be positive.");
	m_clockPeriodNs = 1000.0 / m_clockFrequencyMHz;
	updateThreshold();
}

void DS::setCode(unsigned int code)
{
	if (code >= (1u << m_bitWidth)) throw std::out_of_range("Digital code out of range.");
	m_digitalCode = code;
	updateThreshold();
}

void DS::reset()
{
	m_counter = 0;
	m_currentTimeNs = 0.0;
	m_lastSpikeTimeNs = -m_minSpikeIntervalNs;
}

void DS::updateThreshold()
{
	if (m_mode == ThresholdMode)
	{
		unsigned int maxValue = (1u << m_bitWidth);
		m_threshold = maxValue - m_digitalCode;
	}
	else if (m_mode == FrequencyMode)
	{
		unsigned int base = 16;
		m_threshold = 1 + (base - m_digitalCode);
	}
}

bool DS::tick()
{
	m_currentTimeNs += m_clockPeriodNs;
	m_counter++;
	if (m_counter >= m_threshold)
	{
		m_counter = 0;
		m_lastSpikeTimeNs = m_currentTimeNs;
		return true;
	}
	return false;
}

bool DS::isSpikeActive() const
{
	return (m_currentTimeNs - m_lastSpikeTimeNs) < m_spikeWidthNs;
}

double DS::getSpikePeriodNs() const
{
	return m_clockPeriodNs * m_threshold;
}

double DS::getSpikeRateMHz() const
{
	return 1000.0 / getSpikePeriodNs();
}

void DS::setMode(Mode m)
{
	m_mode = m;
	updateThreshold();
}
