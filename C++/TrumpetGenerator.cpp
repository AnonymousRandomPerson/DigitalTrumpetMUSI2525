#include "TrumpetGenerator.h"
#include <math.h>
#include <iostream>

//int main()
//{
//    TrumpetGenerator *trumpet = new TrumpetGenerator();
//}

TrumpetGenerator::TrumpetGenerator()
{
    attackCurve = makeLine(0.2, 1, 0.075);
    decayCurve = makeLine(1, 0.8, 0.3);
    releaseCurve = makeLine(0.8, 0, 0.2);
}

std::vector<double> TrumpetGenerator::generateTrumpet(double frequency, double seconds)
{
    int numInitialSamples = seconds * SAMPLE_RATE + 1;
    int numSamples = numInitialSamples * 2;
    std::vector<double> trumpetSound(numSamples);

    for (unsigned long i = 0; i < envelope.size(); i++)
    {
        addSinusoidal(0, seconds, frequency * i, envelope[i], trumpetSound);
    }

    // FM.
    fmmod(trumpetSound, 0.01, frequency);

    // Initial attack.
    for (unsigned long i = 0; i < attackCurve.size(); i++)
    {
        trumpetSound[i] *= attackCurve[i];
    }

    // Decay.
    for (unsigned long i = 0; i < decayCurve.size(); i++)
    {
        trumpetSound[attackCurve.size() + i] *= decayCurve[i];
    }

    // Sustain.
    int sustainLength = 0;
    for (unsigned long i = attackCurve.size() + decayCurve.size(); i < numInitialSamples - releaseCurve.size(); i++)
    {
        trumpetSound[i] *= 0.8;
        sustainLength++;
    }

    // Release.
    for (unsigned long i = 0; i < releaseCurve.size(); i++)
    {
        trumpetSound[attackCurve.size() + decayCurve.size() + sustainLength + i] *= releaseCurve[i];
    }

    // Reverb.
    std::vector<double> partTrumpet(numInitialSamples);
    for (int i = 0; i < 40; i++) {
        std::vector<double> partTrumpetScaled(numInitialSamples);
        for (int j = 0; j < numInitialSamples; j++) {
            partTrumpetScaled[j] = partTrumpet[j] / (i * 1);
        }
        addWave(0 + i * 0.05, partTrumpetScaled, trumpetSound);
    }

    for (int i = numInitialSamples; i < numSamples; i++)
    {
        trumpetSound[i] = 0;
    }

    // LPF.
    double lowPassValue = 0;
    for (int i = 0; i < numSamples; i++)
    {
        lowPassValue += (trumpetSound[i] - lowPassValue) / 2;
        trumpetSound[i] = lowPassValue;
    }

    double maxAmplitude = 0;
    for (int i = 0; i < numSamples; i++)
    {
        maxAmplitude = fmax(maxAmplitude, fabs(trumpetSound[i]));
    }

    for (int i = 0; i < numSamples; i++)
    {
        trumpetSound[i] /= maxAmplitude;
    }

    return trumpetSound;
}

void TrumpetGenerator::addSinusoidal(double startTime, double seconds, double frequency, double amplitude, std::vector<double> originalSound)
{
    std::vector<double> newWave = generateSinusoidal(amplitude, frequency, seconds);
    addWave(startTime, newWave, originalSound);
}

void TrumpetGenerator::addWave(double startTime, std::vector<double> newWave, std::vector<double> originalSound)
{
    int startSamples = secondsToSamples(startTime);
    int numSamples = newWave.size();

    for (int i = 0; i < numSamples; i++)
    {
        originalSound[i + startSamples] += newWave[i];
    }
}

// Must delete return array after use.
std::vector<double> TrumpetGenerator::generateSinusoidal(double amplitude, double frequency, double seconds)
{
    int length = secondsToSamples(seconds);
    std::vector<double> x(length);

    for (int i = 0; i < length; i++)
    {
        x[i] = amplitude * sin(i * 2 * M_PI * frequency);
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

// Must delete return array after use.
std::vector<double> TrumpetGenerator::makeLine(double startY, double endY, double seconds)
{
    int length = secondsToSamples(seconds);
    std::vector<double> y(length);

    for (int i = 0; i < length; i++)
    {
        y[i] = startY + (endY - startY) * (i / (double) SAMPLE_RATE) / seconds;
    }

    return y;
}

void TrumpetGenerator::fmmod(std::vector<double> sound, double frequency, double freqDev)
{
    //std::vector<double> sinusoidal = generateSinusoidal(1, frequency, samplesToSeconds(sound.length));
}

