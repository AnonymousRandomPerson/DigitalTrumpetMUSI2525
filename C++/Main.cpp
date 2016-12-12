#include "mbed.h"
#include "PinDetect.h"


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
  AnalogIn pressure(p16);
  float preading;
  
  
//Patterns for choosing which valve switch statement to use.
  bool pattern_a = 1;
  bool pattern_b = 0;
// setup pc serial monitor
  Serial pc(USBTX, USBRX);
 

// setup pushbuttons for debounced interrupt use
  PinDetect pb1(p23);
  PinDetect pb2(p24);
  PinDetect pb3(p25);
  PinDetect pb4(p22);

// pushbutton status - need volatile for safe interrupt R/W
  volatile int pbStatus = 0;
  volatile int range = 0;

  AnalogOut DAC(p18);
  
//global variables used by interrupt routine
  volatile int i=0;
  int counter=0;
  const int BUFFER_SIZE = 256;
  const int SAMPLE_RATE = 11025;
  float Analog_out_data[BUFFER_SIZE];
  bool needsUpdate = false;
  const float PERIOD = 1.0 / SAMPLE_RATE;
  const float TWO_PI = 6.28318530717959;
  const float MULTIPLIER = TWO_PI * PERIOD;
  const int SIN_PRECISION = 1024;
  float sinLookup[SIN_PRECISION];

// Callback routine for button on
  void pb1_hit_callback_on (void)
  {
      // CODE HERE WILL RUN WHEN INTERUPT IS GENERATED
      pbStatus = pbStatus | 0x01;
  }
// Callback routine is interrupt activated by a debounced pb2 hit
  void pb2_hit_callback_on (void)
  {
      // CODE HERE WILL RUN WHEN INTERUPT IS GENERATED
      pbStatus = pbStatus | 0x02;
  }
  void pb3_hit_callback_on (void)
  {
      // CODE HERE WILL RUN WHEN INTERUPT IS GENERATED
      pbStatus = pbStatus | 0x04;
  }
  
  void pb4_hit_callback_on (void)
  {
      // CODE HERE WILL RUN WHEN INTERUPT IS GENERATED
     pbStatus = pbStatus | 0x08;
  }

// Callback routine for button off
  void pb1_hit_callback_off (void)
  {
      // CODE HERE WILL RUN WHEN INTERUPT IS GENERATED
      //0x06
      pbStatus = pbStatus & 0x14;

  }
  //pb2
  void pb2_hit_callback_off (void)
  {
      // CODE HERE WILL RUN WHEN INTERUPT IS GENERATED
      //0x05
      pbStatus = pbStatus & 0x13;
  }
  //pb3 
  void pb3_hit_callback_off (void)
  {
      // CODE HERE WILL RUN WHEN INTERUPT IS GENERATED 0x03
      pbStatus = pbStatus & 0x11;
  }
  //pb4
  void pb4_hit_callback_off (void)
  {
      // CODE HERE WILL RUN WHEN INTERUPT IS GENERATED
      pbStatus = pbStatus & 0x07;
  }


//member functions
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

void recalcBuffer(int offset, float frequency, bool release)
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
    else if (release)
    {
        if (offset < 2205)
        {
            curve = 0.8 - (offset / 2205.0);
        }
        else
        {
            curve = 0;
        }
    }
    float noteMultiplier = frequency * MULTIPLIER;
    for(int k=0; k<BUFFER_SIZE; k++) {
        float baseMultiplier = (k + offset) * noteMultiplier;
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
    pb4.mode(PullUp);
    // need a small delay for initial pullup to take effect due to capacitance
    wait(.01);
    pb1.attach_deasserted(&pb1_hit_callback_on);
    pb2.attach_deasserted(&pb2_hit_callback_on);
    pb3.attach_deasserted(&pb3_hit_callback_on);
    pb4.attach_deasserted(&pb4_hit_callback_on);
    
    
    // Setup Interrupt callback functions for each pb hit
    pb1.attach_asserted(&pb1_hit_callback_off);
    pb2.attach_asserted(&pb2_hit_callback_off);
    pb3.attach_asserted(&pb3_hit_callback_off);
    pb4.attach_asserted(&pb4_hit_callback_off);
    // Start sampling the pb inputs using interrupts
    pb1.setSampleFrequency();
    pb2.setSampleFrequency();
    pb3.setSampleFrequency();
    pb4.setSampleFrequency();
// pushbuttons are setup with pullups and running with a software debounce filter

    
    // set up a timer to be used for sample rate interrupts
    for(int k=0; k < SIN_PRECISION; k++) {
        sinLookup[k] = sin(TWO_PI * k / float(SIN_PRECISION));
    }
    float frequency = 440;
    recalcBuffer(counter, frequency, false);
    Ticker Sample_Period;
    Sample_Period.attach(&Sample_timer_interrupt, 1.0/(SAMPLE_RATE));

//main loop 
    
    while(1) {
        //code here will run when buffer is out of samples 
        if (needsUpdate)
        {
            needsUpdate = false;
            counter += BUFFER_SIZE;
            switch(pbStatus)
            {
            case 0x00: frequency = 261; break; // C
            case 0x07: frequency = 277.2; break; // C#
            case 0x05: frequency = 293.7; break; // D
            case 0x06: frequency = 311.1; break; // Eb
            case 0x03: frequency = 329.6; break; // E
            case 0x01: frequency = 349.2; break; // F
            case 0x02: frequency = 370.0; break; // F#
            case 0x08: frequency = 392; break; // G
            case 0x14: frequency = 415.3; break; // G#
            case 0x11: frequency = 440; break; // A
            case 0x09: frequency = 466.2; break; // A#
            case 0x10: frequency = 493.9; break; // B
            default: frequency = 261;
            }
        
            recalcBuffer(counter, frequency, false);
            
        }  
    
    } //end while
    
} //end main
