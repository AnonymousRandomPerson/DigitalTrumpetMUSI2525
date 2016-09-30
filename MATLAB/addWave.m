% Adds a wave into the sound at a certain start time.
function [newSound, newTime] = addWave(startTime, newWave, originalSound)
    startSamples = secondsToSamples(startTime) + 1;
    numSamples = length(newWave);
    originalSound(1, startSamples + 1 : startSamples + numSamples) = originalSound(1, startSamples + 1 : startSamples + numSamples) + newWave(1, 1 : numSamples);
    newSound = originalSound;
    newTime = startTime + duration;
end