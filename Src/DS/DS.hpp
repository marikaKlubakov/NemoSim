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
	double clockFrequencyMHz;
	unsigned int bitWidth;
	unsigned int digitalCode;
	unsigned int counter;
	double clockPeriodNs;
	unsigned int threshold;

	// Neuron-specific parameters
	double spikeWidthNs;
	double minSpikeIntervalNs;
	double lastSpikeTimeNs;
	double currentTimeNs;

	Mode mode;

	void updateThreshold();
};
