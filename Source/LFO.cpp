/*
  ==============================================================================

	Sine/tri/saw LFO with quadrature output

  ==============================================================================
*/

#include "LFO.h"
LFO::LFO() { srand(static_cast<uint32_t>(time(NULL))); }

LFO::~LFO() = default;

bool LFO::reset(double _sampleRate)
{
	sampleRate = _sampleRate;
	phaseInc = lfoParameters.frequency_Hz / sampleRate;
	
	modCounter = 0.0;
	modCounterQP = 0.25;
	
	return true;
}

OscillatorParameters LFO::getParameters() { return lfoParameters; }

void LFO::setParameters(const OscillatorParameters& params)
{
	if (params.frequency_Hz != lfoParameters.frequency_Hz)
		phaseInc = params.frequency_Hz / sampleRate;
	
	lfoParameters = params;
}

const SignalGenData LFO::renderAudioOutput()
{
	checkAndWrapModulo(modCounter, phaseInc);
	
	modCounterQP = modCounter;
	
	advanceAndCheckWrapModulo(modCounterQP, 0.25);
	
	SignalGenData output;
	
	if (lfoParameters.waveform == generatorWaveform::kSin)
	{
		double angle = modCounter * 2.0 * M_PI - M_PI;
		
//        output.normalOutput = parabolicSine(-angle);
		output.normalOutput = std::sin(-angle);
		
		angle = modCounterQP * 2.0 * M_PI - M_PI;
		
//        output.quadPhaseOutput_pos = parabolicSine(-angle);
		output.quadPhaseOutput_pos = std::sin(-angle);
	}
	else if (lfoParameters.waveform == generatorWaveform::kTriangle)
	{
		// bipolar saw
		output.normalOutput = unipolarToBipolar(modCounter);
		// bipolar triangle from saw
		output.normalOutput = 2.0 * fabs(output.normalOutput) - 1.0;
		
		output.quadPhaseOutput_pos = unipolarToBipolar(modCounterQP);
		
		output.quadPhaseOutput_pos = 2.0 * fabs(output.quadPhaseOutput_pos) -1.0;
	}
	else if (lfoParameters.waveform == generatorWaveform::kSaw)
	{
		output.normalOutput = unipolarToBipolar(modCounter);
		
		output.quadPhaseOutput_pos = unipolarToBipolar(modCounterQP);
	}
	
	output.quadPhaseOutput_neg = -output.quadPhaseOutput_pos;
	output.invertedOutput = -output.normalOutput;
	
	advanceModulo(modCounter, phaseInc);
	
	return output;
}

inline bool LFO::checkAndWrapModulo(double& moduloCounter, double phaseInc)
{
	if (phaseInc > 0 && moduloCounter >= 1.0)
	{
		moduloCounter -= 1.0;
		return true;
	}
	
	if (phaseInc < 0 && moduloCounter <= 0.0)
	{
		moduloCounter += 1.0;
		return true;
	}
	
	return false;
}

inline bool LFO::advanceAndCheckWrapModulo(double& moduloCounter, double phaseInc)
{
	moduloCounter += phaseInc;
	
	if (phaseInc > 0 && moduloCounter >= 1.0)
	{
		moduloCounter -= 1.0;
		return true;
	}
	
	if (phaseInc < 0 && moduloCounter >= 1.0)
	{
		moduloCounter += 1.0;
		return true;
	}
	
	return false;
}

inline void LFO::advanceModulo(double& moduloCounter, double phaseInc) { moduloCounter += phaseInc; }

inline double LFO::parabolicSine(double angle)
{
	double y = B * angle + C * angle * fabs(angle);
	y = P * (y * fabs(y) - y) + y;
	return y;
}
