#include "mbed.h"
// Audio output demo for speaker
// generates a 500Hz sine wave on the analog output pin
// 128 data points on one sine wave cycle are precomputed,
// scaled, stored in an array and
// continuously output to the Digital to Analog convertor
 
AnalogOut DAC(p18);
//global variables used by interrupt routine
volatile int i=0;
int counter=0;
const int BUFFER_SIZE = 128;
const int SAMPLE_RATE = 11025;
const float TWO_PI = 6.28318530717959;
const float PERIOD = 1046.0 / SAMPLE_RATE;
const float MULTIPLIER = TWO_PI * PERIOD;
float Analog_out_data[BUFFER_SIZE];
const int SIN_PRECISION = 1024;
float sinLookup[SIN_PRECISION];
bool needsUpdate = false;

float sinApprox(float input)
{
    bool negative = false;
    if (input < 0)
    {
        negative = true;
        input = -input;
    }
    int roundInput = input * SIN_PRECISION / TWO_PI;
    roundInput = roundInput & (SIN_PRECISION - 1);
    float answer = sinLookup[roundInput];
    if (negative)
    {
        answer = -answer;
    }
    return answer;
}

void recalcBuffer(int offset)
{
    float curve = 0.8;
    if (offset < 827)
    {
        curve = offset / 827.0 * 0.8 + 0.2;
    }
    else if (offset < 4134)
    {
        curve = 1.0 - (offset - 827.0) / 3307.0 * 0.2;
    }
    for(int k=0; k<BUFFER_SIZE; k++) {
        float baseMultiplier = (k + offset) * MULTIPLIER;
        Analog_out_data[k] = sinApprox(baseMultiplier);
        Analog_out_data[k] += sinApprox(baseMultiplier * 2) * 0.7847;
        Analog_out_data[k] += sinApprox(baseMultiplier * 3) * 0.8333;
        Analog_out_data[k] += sinApprox(baseMultiplier * 4) * 0.7467;
        Analog_out_data[k] += sinApprox(baseMultiplier * 5) * 0.4067;
        Analog_out_data[k] += sinApprox(baseMultiplier * 6) * 0.4222;
        Analog_out_data[k] += sinApprox(baseMultiplier * 7) * 0.3117;
        Analog_out_data[k] += sinApprox(baseMultiplier * 8) * 0.2611;
        Analog_out_data[k] += sinApprox(baseMultiplier * 9) * 0.205;
        Analog_out_data[k] += sinApprox(baseMultiplier * 10) * 0.1375;
        Analog_out_data[k] += sinApprox(baseMultiplier * 11) * 0.1006;
        Analog_out_data[k] += sinApprox(baseMultiplier * 12) * 0.0658;
        Analog_out_data[k] += sinApprox(baseMultiplier * 13) * 0.0428;
        Analog_out_data[k] = (5.3181 + Analog_out_data[k] * curve) / 10.6362;
    }
}
 
// Interrupt routine
// used to output next analog sample whenever a timer interrupt occurs
void Sample_timer_interrupt(void)
{
    // send next analog sample out to D to A
    DAC = Analog_out_data[i];
    // increment pointer and wrap around back to 0 at 128
    i = (i+1) & (BUFFER_SIZE - 1);
    if (i == 0)
    {
        needsUpdate = true;
    }
}
 
int main()
{
    // set up a timer to be used for sample rate interrupts
    Ticker Sample_Period;
    // precompute 128 sample points on one sine wave cycle 
    // used for continuous sine wave output later
    for(int k=0; k < SIN_PRECISION; k++) {
        sinLookup[k] = sin(TWO_PI * k / float(SIN_PRECISION));
    }
    recalcBuffer(0);
    // turn on timer interrupts to start sine wave output
    // sample rate is 500Hz with 128 samples per cycle on sine wave
    Sample_Period.attach(&Sample_timer_interrupt, 1.0/(SAMPLE_RATE));
    // everything else needed is already being done by the interrupt routine
    trumpet = new TrumpetGenerator();
    while(1)
    {
        if (needsUpdate)
        {
            needsUpdate = false;
            counter += BUFFER_SIZE;
            recalcBuffer(counter);
        }
    }
}

