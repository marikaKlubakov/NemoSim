#pragma once

class DS
{
public:
	enum Mode
	{
		ThresholdMode,
		FrequencyMode
	};
public:
	DS(double clkFreqMHz, unsigned int bw, Mode m = ThresholdMode);

	void setCode(unsigned int code);
	void reset();
	bool tick();
	bool isSpikeActive() const;

	double getSpikePeriodNs() const;
	double getSpikeRateMHz() const;
	void setMode(Mode m);

	
private:
	double m_clockFrequencyMHz;
	unsigned int m_bitWidth;
	unsigned int m_digitalCode;
	unsigned int m_counter;
	double m_clockPeriodNs;
	unsigned int m_threshold;

	// Neuron-specific parameters
	double m_spikeWidthNs;
	double m_minSpikeIntervalNs;
	double m_lastSpikeTimeNs;
	double m_currentTimeNs;

	Mode m_mode;

	void updateThreshold();
};
