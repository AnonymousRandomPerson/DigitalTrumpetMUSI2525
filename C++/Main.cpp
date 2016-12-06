#include "mbed.h"
#include "PinDetect.h"
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
// main program
void main()
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

//main loop 
    while(1) {
    //read and set vibration range
            vreading = vibration.read();
            pc.printf(" vibration = [%f]\n\r", vreading); 
    //read and set pressure range
            preading = pressure.read();
            pc.printf(" pressure = [%f]\n\r\n", preading);
       //check pushbutton status for new input
       

       
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
        myLed1 = !myLed1; //blink to show loop is still running
        wait(1);
        
    } //end while
} //end main



