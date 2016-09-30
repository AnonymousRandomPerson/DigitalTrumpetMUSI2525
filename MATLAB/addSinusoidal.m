% Adds a sinusoidal into the sound at a certain start time.
function [newSound, newTime] = addSinusoidal(startTime, duration, frequency, amplitude, originalSound)
    [~, newWave] = generateSinusoidal(amplitude, frequency, duration);
    [newSound, newTime] = addWave(startTime, newWave, originalSound);
end