#include "mbed.h"
#include "pinDetect.h"
#include "Speaker.h"

// setup builtin LEDs
DigitalOut myLed1(LED1);
DigitalOut myLed2(LED2);
DigitalOut myLed3(LED3);
DigitalOut myLed4(LED4);

//Hardware Setup

// setup vibration sensor
  AnalogIn vibration(p17);
  float vreading;
//setup pressure sensor
  AnalogIn pressure(p18);
  float preading;
  
  
// Patterns for choosing which valve switch statement to use.
  enum Pattern { pattern_a, pattern_b };
// The current on/off status of the valves 123.
  enum ValveStatus { v000, v100, v010, v110, v001, v101, v011, v111 };
// setup pc serial monitor
  Serial pc(USBTX, USBRX);
 

// setup pushbuttons for debounced interrupt use
PinDetect pb1(p23);
PinDetect pb2(p24);
PinDetect pb3(p25);

// setup instance of new Speaker class, mySpeaker using pin 21
// the pin must be a PWM output pin
Speaker mySpeaker(p21);

// pushbutton status - need volatile for safe interrupt R/W
volatile int pbStatus = 0;
volatile int range = 0;

AnalogOut DAC(p18);
//global variables used by interrupt routine
volatile int i=0;
int counter=0;
const int BUFFER_SIZE = 128;
const int SAMPLE_RATE = 11025;
float Analog_out_data[BUFFER_SIZE];
bool needsUpdate = false;

// Callback routine is interrupt activated by a debounced pb1 hit
void pb1_hit_callback (void)
{
    // CODE HERE WILL RUN WHEN INTERUPT IS GENERATED
    pbStatus = pbStatus | 0x01;

}
// Callback routine is interrupt activated by a debounced pb2 hit
void pb2_hit_callback (void)
{
    // CODE HERE WILL RUN WHEN INTERUPT IS GENERATED
    pbStatus = pbStatus | 0x02;
}
void pb3_hit_callback (void)
{
    // CODE HERE WILL RUN WHEN INTERUPT IS GENERATED
    pbStatus = pbStatus | 0x04;

}

const float PERIOD = 440.0 / SAMPLE_RATE;
const float TWO_PI = 6.28318530717959;
const float MULTIPLIER = TWO_PI * PERIOD;
const int SIN_PRECISION = 1024;
float sinLookup[SIN_PRECISION];

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

// main program
int main()
{
//setup four SPST push buttons
    pb1.mode(PullUp); //add internal pullup resistor
    pb2.mode(PullUp);
    pb3.mode(PullUp);
    // need a small delay for initial pullup to take effect due to capacitance
    wait(.01);
    // Setup Interrupt callback functions for each pb hit
    pb1.attach_deasserted(&pb1_hit_callback);
    pb2.attach_deasserted(&pb2_hit_callback);
    pb3.attach_deasserted(&pb3_hit_callback);
    // Start sampling the pb inputs using interrupts
    pb1.setSampleFrequency();
    pb2.setSampleFrequency();
    pb3.setSampleFrequency();
// pushbuttons are setup with pullups and running with a software debounce filter

//boolean valves
    bool valve1 = 0;
    bool valve2 = 0;
    bool valve3 = 0;
    
    // set up a timer to be used for sample rate interrupts
    for(int k=0; k < SIN_PRECISION; k++) {
        sinLookup[k] = sin(TWO_PI * k / float(SIN_PRECISION));
    }
    recalcBuffer(0);
    Ticker Sample_Period;
    Sample_Period.attach(&Sample_timer_interrupt, 1.0/(SAMPLE_RATE));

//main loop 
    while(1) {
    //read and set vibration range
//            vreading = vibration.read();
//            pc.printf(" vibration = [%f]\n\r", vreading); 
    //read and set pressure range
//            preading = pressure.read();
//            pc.printf(" pressure = [%f]\n\r\n", preading);
       //check pushbutton status for new input
       
        if (needsUpdate)
        {
            needsUpdate = false;
            counter += BUFFER_SIZE;
            recalcBuffer(counter);
        }
       
        switch (pbStatus) {
            case 0x01 :
                myLed2 = 1;
                wait(.01);
                myLed2 = 0;
                mySpeaker.PlayNote(200.0,0.1,0.8);
                break;
            case 0x02 :
                //blink light for debugging
                myLed3 = 1;
                wait(.01);
                myLed3 = 0;
                pbStatus = 0;
                mySpeaker.PlayNote(400.0,0.1,0.8);
                break;
            case 0x04 :
                //blink light for debugging
                myLed4 = 1;
                wait(.01);
                myLed4 = 0;
                pbStatus = 0;
                mySpeaker.PlayNote(700.0,0.1,0.8);
                break;  
        //no pb or multiple pbs hit
            default :
                myLed2 = 0;
                myLed3 = 0;
                myLed4 = 0;
                pbStatus = 0;
        } //end switch
        //myLed1 = !myLed1; //blink to show loop is still running
        //wait(1);
        
    } //end while
    return 0;
} //end main


