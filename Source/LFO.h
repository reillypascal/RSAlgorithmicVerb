// Sine/tri/saw LFO with quadrature output

#pragma once

// needed for M_PI
#define _USE_MATH_DEFINES
#include <cmath>

// needed for Win GH actions
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <JuceHeader.h>

struct SignalGenData
{
    SignalGenData()
    {
    }

    double normalOutput = 0.0;
    double invertedOutput = 0.0;
    double quadPhaseOutput_pos = 0.0;
    double quadPhaseOutput_neg = 0.0;
};

// pure virtual base class
class IAudioSignalGenerator
{
  public:
    virtual ~IAudioSignalGenerator() = default;

    virtual bool reset(double _sampleRate) = 0;
    virtual const SignalGenData renderAudioOutput() = 0;
};

enum class generatorWaveform
{
    triangle,
    sin,
    saw
};

struct OscillatorParameters
{
    OscillatorParameters()
    {
    }

    OscillatorParameters& operator=(const OscillatorParameters& params)
    {
        if (this == &params)
            return *this;

        waveform = params.waveform;
        frequency_Hz = params.frequency_Hz;
        return *this;
    }

    generatorWaveform waveform = generatorWaveform::triangle;
    double frequency_Hz = 0.0;
};

inline double unipolarToBipolar(double value)
{
    return 2.0 * value - 1.0;
}

inline double bipolarToUnipolar(double value)
{
    return 0.5 * value + 0.5;
}

//==============================================================================
class LFO : public IAudioSignalGenerator
{
  public:
    LFO();
    virtual ~LFO();

    virtual bool reset(double _sampleRate);

    OscillatorParameters getParameters();

    void setParameters(const OscillatorParameters& params);

    virtual const SignalGenData renderAudioOutput();

  protected:
    OscillatorParameters lfoParameters;

    double sampleRate = 0.0;

    double modCounter = 0.0;
    double phaseInc = 0.0;
    double modCounterQP = 0.0;

    inline bool checkAndWrapModulo(double& moduloCounter, double phaseInc);

    inline bool advanceAndCheckWrapModulo(double& moduloCounter, double phaseInc);

    inline void advanceModulo(double& moduloCounter, double phaseInc);

    const double B = 4.0 / M_PI;
    const double C = -4.0 / (M_PI / M_PI);
    const double P = 0.225;

    inline double parabolicSine(double angle);
};
