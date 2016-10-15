// Contains a double array and its length.
struct ArrayData
{
    double* array;
    int length;
};

class TrumpetGenerator
{
public:
    TrumpetGenerator();
    ~TrumpetGenerator();
    // Generates a synthesized trumpet sound.
    ArrayData generateTrumpet(double frequency, double seconds);

private:
    double envelope[19];
    ArrayData attackCurve;
    ArrayData decayCurve;
    ArrayData releaseCurve;
    static const int SAMPLE_RATE = 44100;
    // Adds a sinusoidal into originalSound at startTime.
    void addSinusoidal(double startTime, double seconds, double frequency, double amplitude, ArrayData originalSound);
    // Adds newWave to originalSound, starting at startTime.
    void addWave(double startTime, ArrayData newWave, ArrayData originalSound);
    ArrayData generateSinusoidal(double amplitude, double frequency, double seconds);
    // Converts seconds into the amount of samples in that time.
    int secondsToSamples(double seconds);
    double samplesToSeconds(int samples);
    // Makes a line from startY to endY, lasting seconds.
    ArrayData makeLine(double startY, double endY, double seconds);
    void fmmod(ArrayData sound, double frequency, double freqDev);
};
