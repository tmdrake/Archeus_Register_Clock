#ifndef REGISTER_H
#define REGISTER_H
#include "Arduino.h"

class Register {
  
  private:
    byte _A_Pin; // Pin for Trigger
    byte _B_Pin; // Pin for Reset
    int lastnumber; //for last number

    //For async checking
    bool _isBusy = false;

  public:
    // Setup pin LED and call init()
    //public construct
    Register(byte A_Pin, byte B_Pin);
    // Setup the pin led as OUTPUT
    // and power off the LED - default state
    //void init();
    
    // Reset the register (11 Clicks usually)
    void reset();
    // Send to distination number
    void countto(int counto);

    //For Async Updates
    //void update();
    //void A_reset();
    //bool isBusy();


};


#endif