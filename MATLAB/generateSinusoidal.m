function [t, x] = generateSinusoidal(amplitude, frequencyInHz, lengthInS)
    t = 0 : 1 / sampleRate : lengthInS;
    x = amplitude * sin(t * 2 * pi * frequencyInHz);
end