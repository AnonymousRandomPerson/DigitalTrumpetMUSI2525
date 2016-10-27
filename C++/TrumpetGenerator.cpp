#include "TrumpetGenerator.h"
#include <math.h>
#include <iostream>

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
    if (vibrationFrequency < 261.63)
    {
        return 0;
    }
    else if (vibrationFrequency < 392)
    {
        switch(bitwiseValve)
        {
        case 0: return 261.63; // C4, 000
        case 7: return 277.18; // C#4, 111
        case 5: return 293.66; // D4, 101
        case 3: return 311.13; // Eb4, 011
        case 6: return 329.63; // E4, 110
        case 4: return 349.23; // F4, 100
        case 2: return 369.99; // F#4, 010
        default: return 0;
        }
    }
    else if (vibrationFrequency < 523.25)
    {
        switch(bitwiseValve)
        {
        case 0: return 392; // G4, 000
        case 3: return 415.3; // G#4, 011
        case 6: return 440; // A4, 110
        case 4: return 466.16; // Bb4, 100
        case 2: return 493.88; // B4, 010
        default: return 0;
        }
    }
    else {
        switch(bitwiseValve)
        {
        case 0: return 523.25; // C5, 000
        default: return 0;
        }
    }
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

