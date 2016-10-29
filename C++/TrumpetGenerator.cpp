#include "TrumpetGenerator.h"
#include <math.h>
#include <iostream>

// Patterns for choosing which valve switch statement to use.
enum Pattern : bool { pattern_a, pattern_b };
// The current on/off status of the valves (1,2,3).
enum ValveStatus { v000, v001, v010, v011, v100, v101, v110, v111 };

TrumpetGenerator::TrumpetGenerator()
{
    attackCurve = makeLine(0.2, 1, 0.075);
    decayCurve = makeLine(1, 0.8, 0.3);
    releaseCurve = makeLine(0.8, 0, 0.2);
}

TrumpetGenerator::~TrumpetGenerator()
{

}

std::vector<double> TrumpetGenerator::generateTrumpet(double frequency, double seconds)
{
    int numInitialSamples = seconds * SAMPLE_RATE + 1;
    int numSamples = numInitialSamples * 2;
    unsigned long i;
    std::vector<double> trumpetSound(numSamples);

    for (i = 0; i < envelope.size(); i++)
    {
        addSinusoidal(0, seconds, frequency * (i + 1), envelope[i], trumpetSound);
    }

    // FM.
    fmmod(trumpetSound, 0.01, frequency);

    // Initial attack.
    for (i = 0; i < attackCurve.size(); i++)
    {
        trumpetSound[i] *= attackCurve[i];
    }

    // Decay.
    for (i = 0; i < decayCurve.size(); i++)
    {
        trumpetSound[attackCurve.size() + i] *= decayCurve[i];
    }

    // Sustain.
    int sustainLength = 0;
    for (i = attackCurve.size() + decayCurve.size(); i < numInitialSamples - releaseCurve.size(); i++)
    {
        trumpetSound[i] *= 0.8;
        sustainLength++;
    }

    // Release.
    for (i = 0; i < releaseCurve.size(); i++)
    {
        trumpetSound[attackCurve.size() + decayCurve.size() + sustainLength + i] *= releaseCurve[i];
    }

    // Reverb.
    std::vector<double> partTrumpet(numInitialSamples);
    std::vector<double> partTrumpetScaled(numInitialSamples);
    int j;
    for (i = 0; i < 40; i++)
    {
        for (j = 0; j < numInitialSamples; j++)
        {
            partTrumpetScaled[j] = partTrumpet[j] / ((i + 1) * 1);
        }
        addWave(0 + i * 0.05, partTrumpetScaled, trumpetSound);
    }

    for (i = numInitialSamples; i < numSamples; i++)
    {
        trumpetSound[i] = 0;
    }

    return trumpetSound;
}

double TrumpetGenerator::mapToFrequency(double vibrationFrequency, std::array<bool, 3> valves)
{
    int bitwiseValve = 0;
    // Big-endian is easier to read in this context.
    int lastIndex = valves.size() - 1;
    for (int i = lastIndex; i >= 0; i--)
    {
        if (valves[i])
        {
            bitwiseValve += 1 << (lastIndex - i);
        }
    }
    Pattern pattern;
    double baseFrequency = 261.63; // C4

    double noteMultiplier;
    int octave = log2(vibrationFrequency / baseFrequency);
    double octaveMultiplier = 1 << octave;
    double octavePlace = vibrationFrequency / (baseFrequency * octaveMultiplier);
    octave += 4;
    if (octave < 4)
    {
        return 0;
    }
    else if (octavePlace < 1.5)
    {
        pattern = pattern_a;
    }
    else
    {
        pattern = pattern_b;
    }
    if (octave >= 7)
    {
        // Cap at C7.
        noteMultiplier = 1;
        octaveMultiplier = 1 << 3;
    }
    else if (pattern == pattern_a)
    {
        switch(bitwiseValve)
        {
        case v000: noteMultiplier = 1; break; // C
        case v111: noteMultiplier = 1.05943508; break; // C#
        case v101: noteMultiplier = 1.122424798; break; // D
        case v011: noteMultiplier = 1.189198486; break; // Eb
        case v110: noteMultiplier = 1.259909032; break; // E
        case v100: noteMultiplier = 1.334823988; break; // F
        case v010: noteMultiplier = 1.414172687; break; // F#
        default: return 0;
        }
    }
    else
    {
        switch(bitwiseValve)
        {
        case v000: noteMultiplier = 1.498299125; break; // G
        case v011: noteMultiplier = 1.58735619; break; // G#
        case v110: noteMultiplier = 1.681764324; break; // A
        case v100: noteMultiplier = 1.781752857; break; // Bb
        case v010: noteMultiplier = 1.887704009; break; // B
        default: return 0;
        }
    }
    return baseFrequency * octaveMultiplier * noteMultiplier;
}

void TrumpetGenerator::addSinusoidal(double startTime, double seconds, double frequency, double amplitude, std::vector<double> &originalSound)
{
    std::vector<double> newWave = generateSinusoidal(amplitude, frequency, seconds);
    addWave(startTime, newWave, originalSound);
}

void TrumpetGenerator::addWave(double startTime, std::vector<double> newWave, std::vector<double> &originalSound)
{
    int startSamples = secondsToSamples(startTime);
    int numSamples = newWave.size();

    for (int i = 0; i < numSamples; i++)
    {
        int originalIndex = i + startSamples;
        originalSound[originalIndex] += newWave[i];
    }
}

std::vector<double> TrumpetGenerator::generateSinusoidal(double amplitude, double frequency, double seconds)
{
    int length = secondsToSamples(seconds);
    std::vector<double> x(length);

    int i;
    for (i = 0; i < length; i++)
    {
        x[i] = amplitude * sin((double)i / SAMPLE_RATE * 2 * M_PI * frequency);
    }

    double maxAmplitude = 0;
    for (i = 0; i < length; i++)
    {
        maxAmplitude = fmax(maxAmplitude, fabs(x[i]));
    }

    return x;
}

int TrumpetGenerator::secondsToSamples(double seconds)
{
    return (int)(SAMPLE_RATE * seconds);
}

double TrumpetGenerator::samplesToSeconds(int samples)
{
    return samples / SAMPLE_RATE;
}

std::vector<double> TrumpetGenerator::makeLine(double startY, double endY, double seconds)
{
    int length = secondsToSamples(seconds);
    std::vector<double> y(length);

    for (int i = 0; i < length; i++)
    {
        y[i] = startY + (endY - startY) * ((double)i / SAMPLE_RATE) / seconds;
    }

    return y;
}

void TrumpetGenerator::fmmod(std::vector<double> &sound, double frequency, double freqDev)
{
    double cumsum = 0;
    for (int i = 0; i < sound.size(); i++)
    {
        cumsum += sound[i];
        double newValue = 25 * cos(2 * M_PI * (frequency * ((double)i / SAMPLE_RATE) + freqDev * (cumsum / SAMPLE_RATE)));
        sound[i] = newValue;
    }
}

