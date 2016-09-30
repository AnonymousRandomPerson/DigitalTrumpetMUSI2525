% Converts seconds into the amount of samples in that time.
function samples = secondsToSamples(seconds)
    samples = int64(sampleRate * seconds);
end