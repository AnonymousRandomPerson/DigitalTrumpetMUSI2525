#ifndef TRUMPETGENERATOR_H
#define TRUMPETGENERATOR_H

#include <array>
#include <vector>

class TrumpetGenerator
{
public:
    TrumpetGenerator();
    ~TrumpetGenerator();
    // Generates a synthesized trumpet sound with a frequency, starting at sampleOffset, as a full sound or a release.
    std::vector<double> generateTrumpet(double frequency, int sampleOffset, bool release);
    // Turns a vibration frequency and a valve configuration into a note frequency.
    // Valve input is bitwise in the order 321.
    double mapToFrequency(double vibrationFrequency, int bitwiseValve);
    // Inserts vector2 at the end of vector1.
    void insertVector(std::vector<double>& vector1, std::vector<double> vector2);
    void resetCumsum();
    static const int SAMPLE_RATE = 44100;
    static const int BLOCK_SIZE = 2048;

private:
    std::array<double, 19> envelope = {3.6, 2.825, 3, 2.688, 1.464, 1.520, 1.122, 0.940, 0.738, 0.495, 0.362, 0.237, 0.154, 0.154, 0.101, 0.082, 0.054, 0.038, 0.036};
    std::vector<double> attackDecayCurve;
    std::vector<double> releaseCurve;
    // Used to generate a trumpet sound before reverb.
    std::vector<double> generateBaseTrumpet(double frequency, int sampleOffset, bool release);
    // Adds a sinusoidal into originalSound at startTime.
    void addSinusoidal(double startTime, double seconds, double frequency, double amplitude, std::vector<double> &originalSound, int sampleOffset);
    // Adds newWave to originalSound, starting at startTime.
    void addWave(double startTime, std::vector<double> newWave, std::vector<double> &originalSound);
    std::vector<double> generateSinusoidal(double amplitude, double frequency, double seconds, int sampleOffset);
    // Converts seconds into the amount of samples in that time.
    int secondsToSamples(double seconds);
    double samplesToSeconds(int samples);
    // Makes a line from startY to endY, lasting seconds.
    std::vector<double> makeLine(double startY, double endY, double seconds);
    void fmmod(std::vector<double> &sound, double frequency, double freqDev, int sampleOffset);
    double cumsum;
};

#endif
