% Generates a synthesized trumpet sound.
function generateTrumpet()
    fundamental = 440;
    seconds = 2;
    
    numInitialSamples = seconds * sampleRate + 1;
    numSamples = numInitialSamples * 2;
    trumpetSound = zeros(1, numSamples);
    % Initial amplitude envelope derived from http://iie.fing.edu.uy/~rocamora/wind_synthesis/doc/wind_synthesis/instrument_table.html
    amplitudes = [3.6 2.825 3 2.688 1.464 1.520 1.122 0.940 0.738 0.495 0.362 0.237 0.154 0.154 0.101 0.082 0.054 0.038 0.036];
    for i = 1 : 1 : length(amplitudes)
        [trumpetSound, ~] = addSinusoidal(0, seconds, fundamental * i, amplitudes(i), trumpetSound);
    end
    
    % FM.
    trumpetSound = fmmod(trumpetSound, 0.01, sampleRate, fundamental);
    
    % Initial attack.
    [attackCurve, attackLength] = makeLine(0.2, 1, 0.075);
    trumpetSound(1:attackLength) = trumpetSound(1:attackLength) .* attackCurve;
    
    % Decay.
    [decayCurve, decayLength] = makeLine(1, 0.8, 0.3);
    trumpetSound(attackLength + 1:attackLength + decayLength) = trumpetSound(attackLength + 1:attackLength + decayLength) .* decayCurve;
    
    [releaseCurve, releaseLength] = makeLine(0.8, 0, 0.2);
    
    % Sustain.
    trumpetSound(attackLength + decayLength:numInitialSamples - releaseLength) = trumpetSound(attackLength + decayLength:numInitialSamples - releaseLength) * 0.8;
    
    % Release.
    trumpetSound(numInitialSamples - releaseLength + 1:numInitialSamples) = trumpetSound(numInitialSamples - releaseLength + 1:numInitialSamples) .* releaseCurve;
    
    % Reverb.
    partTrumpet = trumpetSound(1:numInitialSamples);
    for i = 1 : 1 : 40
        [trumpetSound, ~] = addWave(0 + i * 0.05, partTrumpet / (i * i), trumpetSound);
    end
    
    trumpetSound(numInitialSamples + 1:numSamples) = 0;
    
    % LPF.
    lowPassValue = 0;
    for i = 1 : 1 : length(trumpetSound)
        lowPassValue = lowPassValue + (trumpetSound(i) - lowPassValue) / 2;
        trumpetSound(i) = lowPassValue;
    end
    
    trumpetSound = trumpetSound / max(trumpetSound);
    x = linspace(0, 2.6, length(trumpetSound));
    plot(x, trumpetSound);
    sound(trumpetSound, sampleRate);
    audiowrite('FakeTrumpet.wav', trumpetSound, sampleRate);
end