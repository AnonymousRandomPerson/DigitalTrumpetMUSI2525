#include "TrumpetGenerator.h"
#include <math.h>
#include <iostream>

int main()
{
    TrumpetGenerator *trumpet = new TrumpetGenerator();
}

TrumpetGenerator::TrumpetGenerator()
{
    double amplitudes[] = {3.6, 2.825, 3, 2.688, 1.464, 1.520, 1.122, 0.940, 0.738, 0.495, 0.362, 0.237, 0.154, 0.154, 0.101, 0.082, 0.054, 0.038, 0.036};
    for (int i = 0; i < (int)(sizeof(envelope) / sizeof(double)); i++)
    {
        envelope[0] = amplitudes[0];
    }
    attackCurve = makeLine(0.2, 1, 0.075);
    decayCurve = makeLine(1, 0.8, 0.3);
    releaseCurve = makeLine(0.8, 0, 0.2);
}

TrumpetGenerator::~TrumpetGenerator()
{
    delete [] attackCurve.array;
    delete [] decayCurve.array;
    delete [] releaseCurve.array;
}

ArrayData TrumpetGenerator::generateTrumpet(double frequency, double seconds)
{
    int numInitialSamples = seconds * SAMPLE_RATE + 1;
    int numSamples = numInitialSamples * 2;
    double* trumpetSound = new double[numSamples];
    ArrayData trumpetSoundData = {trumpetSound, numSamples};

    for (int i = 0; i < (int)(sizeof(envelope) / sizeof(double)); i++)
    {
        addSinusoidal(0, seconds, frequency * i, envelope[i], trumpetSoundData);
    }

    // FM.
    fmmod(trumpetSoundData, 0.01, frequency);

    // Initial attack.
    for (int i = 0; i < attackCurve.length; i++)
    {
        trumpetSound[i] *= attackCurve.array[i];
    }

    // Decay.
    for (int i = 0; i < decayCurve.length; i++)
    {
        trumpetSound[attackCurve.length + i] *= decayCurve.array[i];
    }

    // Sustain.
    int sustainLength = 0;
    for (int i = attackCurve.length + decayCurve.length; i < numInitialSamples - releaseCurve.length; i++)
    {
        trumpetSound[i] *= 0.8;
        sustainLength++;
    }

    // Release.
    for (int i = 0; i < releaseCurve.length; i++)
    {
        trumpetSound[attackCurve.length + decayCurve.length + sustainLength + i] *= releaseCurve.array[i];
    }

    // Reverb.
    double* partTrumpet = new double[numInitialSamples];
    for (int i = 0; i < 40; i++) {
        ArrayData partTrumpetScaled = {new double[numInitialSamples], numInitialSamples};
        for (int j = 0; j < numInitialSamples; j++) {
            partTrumpetScaled.array[j] = partTrumpet[j] / (i * 1);
        }
        addWave(0 + i * 0.05, partTrumpetScaled, trumpetSoundData);
        delete [] partTrumpetScaled.array;
    }
    delete [] partTrumpet;

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

    return trumpetSoundData;
}

void TrumpetGenerator::addSinusoidal(double startTime, double seconds, double frequency, double amplitude, ArrayData originalSound)
{
    ArrayData newWave = generateSinusoidal(amplitude, frequency, seconds);
    addWave(startTime, newWave, originalSound);
    delete newWave.array;
}

void TrumpetGenerator::addWave(double startTime, ArrayData newWave, ArrayData originalSound)
{
    int startSamples = secondsToSamples(startTime);
    int numSamples = newWave.length;

    for (int i = 0; i < numSamples; i++)
    {
        originalSound.array[i + startSamples] += newWave.array[i];
    }
}

// Must delete return array after use.
ArrayData TrumpetGenerator::generateSinusoidal(double amplitude, double frequency, double seconds)
{
    int length = secondsToSamples(seconds);
    double *x = new double[length];

    for (int i = 0; i < length; i++)
    {
        x[i] = amplitude * sin(i * 2 * M_PI * frequency);
    }

    ArrayData data;
    return data;
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
ArrayData TrumpetGenerator::makeLine(double startY, double endY, double seconds)
{
    int length = secondsToSamples(seconds);
    double *y = new double(length);

    for (int i = 0; i < length; i++)
    {
        std::cout << i << std::endl;
        y[i] = startY + (endY - startY) * (i / SAMPLE_RATE) / seconds;
    }

    ArrayData data = {y, length};
    return data;
}

void TrumpetGenerator::fmmod(ArrayData sound, double frequency, double freqDev)
{
    //ArrayData sinusoidal = generateSinusoidal(1, frequency, samplesToSeconds(sound.length));
}

