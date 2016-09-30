% Generates a synthesized trumpet sound.
function generateTrumpet()
    fundamental = 440;
    seconds = 2;
    
    numSamples = seconds * sampleRate + 1;
    trumpetSound = zeros(1, numSamples * 2);
    amplitudes = [3.6 2.825 2.654 2.688 1.464 1.520 1.122 0.940 0.738 0.495 0.362 0.237 0.154 0.154 0.101 0.082 0.054 0.038 0.036];
    for i = 1 : 1 : length(amplitudes)
        [trumpetSound, ~] = addSinusoidal(0, seconds, fundamental * i, amplitudes(i), trumpetSound);
    end
    
    % Initial attack.
    [attackCurve, attackLength] = makeLine(0.2, 1, 0.3);
    trumpetSound(1:attackLength) = trumpetSound(1:attackLength) .* attackCurve;
    
    % Decay.
    [decayCurve, decayLength] = makeLine(1, 0.8, 0.3);
    trumpetSound(attackLength + 1:attackLength + decayLength) = trumpetSound(attackLength + 1:attackLength + decayLength) .* decayCurve;
    
    trumpetSound(attackLength + decayLength:numSamples) = trumpetSound(attackLength + decayLength:numSamples) * 0.8;
    
    trumpetSound = trumpetSound / max(trumpetSound);
    x = linspace(0, 2.6, length(trumpetSound));
    plot(x, trumpetSound);
    sound(trumpetSound, sampleRate);
    audiowrite('FakeTrumpet.wav', trumpetSound, sampleRate);
end