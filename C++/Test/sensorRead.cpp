#include "mbed.h"

    // Wheatstone bridge terminals
      AnalogIn Vc(p19);
      AnalogIn Vd(p20);
      float Vcreading;
      float Vdreading;
     
    // setup vibration sensor
      AnalogIn vibration(p17);
      float vreading;
    //setup pressure sensor
      AnalogIn pressure(p16);
      float preading;
      
      Serial pc(USBTX, USBRX);
    
int main() {
   //Choose while loop to execute by placin on top
   
   //vibration reading
   while(1)
    {
        // read and set vibration range
            vreading = vibration.read();
            pc.printf(" vibration = [%f]\n\r", vreading); 
            float voltage =(vreading*(5.0/ 4096.0)); 
            pc.printf("voltage %f \n\r",voltage);
            wait(.75);
    }
    //pressure reading
    while(1)
    {
        // read and set bridge node voltages
            Vcreading = Vc.read();
            Vdreading = Vd.read();
            float Voltc =(Vcreading*(5.0/ 4096.0));
            float Voltd =(Vdreading*(5.0/ 4096.0)); 
            float Vdiff = 1000 * (Voltd - Voltc);
            pc.printf(" Vc = [%f]\n\r", Voltc);
            pc.printf(" Vd = [%f]\n\r", Voltd);
            pc.printf("Vdiff = %f \n\r",Vdiff);
            wait(.75);
    }
}
