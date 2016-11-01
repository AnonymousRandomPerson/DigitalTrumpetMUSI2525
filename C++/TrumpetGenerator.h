#ifndef TRUMPETGENERATOR_H
#define TRUMPETGENERATOR_H

#include <array>
#include <vector>

class TrumpetGenerator
{
public:
    TrumpetGenerator();
    ~TrumpetGenerator();
    // Generates a synthesized trumpet sound.
    std::vector<double> generateTrumpet(double frequency, double seconds);
    // Turns a vibration frequency and a valve configuration into a note frequency.
    // Valve input is bitwise in the order 321.
    double mapToFrequency(double vibrationFrequency, int bitwiseValve);
    static const int SAMPLE_RATE = 44100;

private:
    std::array<double, 19> envelope = {3.6, 2.825, 3, 2.688, 1.464, 1.520, 1.122, 0.940, 0.738, 0.495, 0.362, 0.237, 0.154, 0.154, 0.101, 0.082, 0.054, 0.038, 0.036};
    std::vector<double> attackCurve;
    std::vector<double> decayCurve;
    std::vector<double> releaseCurve;
    // Adds a sinusoidal into originalSound at startTime.
    void addSinusoidal(double startTime, double seconds, double frequency, double amplitude, std::vector<double> &originalSound);
    // Adds newWave to originalSound, starting at startTime.
    void addWave(double startTime, std::vector<double> newWave, std::vector<double> &originalSound);
    std::vector<double> generateSinusoidal(double amplitude, double frequency, double seconds);
    // Converts seconds into the amount of samples in that time.
    int secondsToSamples(double seconds);
    double samplesToSeconds(int samples);
    // Makes a line from startY to endY, lasting seconds.
    std::vector<double> makeLine(double startY, double endY, double seconds);
    void fmmod(std::vector<double> &sound, double frequency, double freqDev);
};

#endif
