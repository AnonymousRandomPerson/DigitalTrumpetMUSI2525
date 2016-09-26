% Makes a line from a starting y to an ending y, lasting an amount of seconds
function [y, numElements] = makeLine(startY, endY, seconds)
    x = 0 : 1 / sampleRate : seconds;
    y = startY + (endY - startY) * x / seconds;
    numElements = length(x);
end