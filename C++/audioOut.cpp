#include "mbed.h"
// Audio output demo for speaker
// generates a 500Hz sine wave on the analog output pin
// 128 data points on one sine wave cycle are precomputed,
// scaled, stored in an array and
// continuously output to the Digital to Analog convertor
 
AnalogOut DAC(p18);
//global variables used by interrupt routine
volatile int i=0;
float Analog_out_data[128];
 
// Interrupt routine
// used to output next analog sample whenever a timer interrupt occurs
void Sample_timer_interrupt(void)
{
    // send next analog sample out to D to A
    DAC = Analog_out_data[i];
    // increment pointer and wrap around back to 0 at 128
    i = (i+1) & 0x07F;
}
 
int main()
{
    // set up a timer to be used for sample rate interrupts
    Ticker Sample_Period;
    // precompute 128 sample points on one sine wave cycle 
    // used for continuous sine wave output later
    for(int k=0; k<128; k++) {
        Analog_out_data[k]=((1.0 + sin((float(k)/128*6.28318530717959)))/2.0);
        // scale the sine wave from 0.0 to 1.0 - as needed for AnalogOut arg 
    }
    // turn on timer interrupts to start sine wave output
    // sample rate is 500Hz with 128 samples per cycle on sine wave
    Sample_Period.attach(&Sample_timer_interrupt, 1.0/(500));
    // everything else needed is already being done by the interrupt routine
    while(1) {}
}
