#include "mbed.h"
#include "PinDetect.h"
#include "Speaker.h"

// setup builtin LEDs
DigitalOut myLed1(LED1);
DigitalOut myLed2(LED2);
DigitalOut myLed3(LED3);
DigitalOut myLed4(LED4);

// setup pushbuttons for debounced interrupt use
PinDetect pb1(p23);
PinDetect pb2(p24);

// setup instance of new Speaker class, mySpeaker using pin 21
// the pin must be a PWM output pin
Speaker mySpeaker(p21);

// pushbutton status - need volatile for safe interrupt R/W
volatile int pbStatus = 0;

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

// main program
int main()
{
//setup four SPST push buttons
    pb1.mode(PullUp); //add internal pullup resistor
    pb2.mode(PullUp);
    // need a small delay for initial pullup to take effect due to capacitance
    wait(.01);
    // Setup Interrupt callback functions for each pb hit
    pb1.attach_deasserted(&pb1_hit_callback);
    pb2.attach_deasserted(&pb2_hit_callback);
    // Start sampling the pb inputs using interrupts
    pb1.setSampleFrequency();
    pb2.setSampleFrequency();
// pushbuttons are all now setup with pullups and running with a software decounce filter

//main loop 
    while(1) {
        //check pushbutton status for new input
        switch (pbStatus) {
                //move forward
            case 0x01 :
                myLed2 = 1;
                pbStatus = 0;
                mySpeaker.PlayNote(200.0,0.1,0.8);
                break;
                //move backwards
            case 0x02 :
                myLed3 = 1;
                pbStatus = 0;
                mySpeaker.PlayNote(400.0,0.1,0.8);
                break;
        //no pb or multiple pbs hit
            default :
                myLed2 = 0;
                myLed3 = 0;
                myLed4 = 0;
                pbStatus = 0;
        } //end switch
        myLed1 = !myLed1; //blink to show loop is still running
        wait(.5);
    } //end while
} //end main

