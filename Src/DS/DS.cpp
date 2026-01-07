#include "DS.hpp"
#include <stdexcept>

// ===== Minimal addition: static M matrix (16 rows x 32 cycles) =====
static const int ROWS = 16;
static const int COLS = 32;

static const int M[ROWS][COLS] =
{
	{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
	{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
	{1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0},
	{1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0},
	{1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0},
	{1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0},
	{1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0},
	{1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0},
	{1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0},
	{1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0},
	{1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0},
	{1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0},
	{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

// ===== Existing DS implementation (minimal changes applied) =====

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
    // Clamp to [0..ROWS-1] to match M rows
    if (code >= static_cast<unsigned int>(ROWS))
        m_digitalCode = ROWS - 1;
    else
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
    // Read spike from M[code][cycle] and advance cycle modulo 32
    const int val = M[m_digitalCode][m_counter % COLS];
    const bool spike = (val != 0);

	m_currentTimeNs += m_clockPeriodNs;
	m_counter++;
	return spike;
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
