/*
* Teleregister clock driver, Used in building a clock.
* currently non async library.
* Will be using an Arduino MEGA due to the amount of IO lines
* 
*/
#if !defined(__AVR_ATmega2560__)
#error This code is designed to run on Arduino MEGA boards
#endif

/**Need a way store and retrieve from eeprom***/
#include <EEPROMex.h>
#include "Arduino.h"


/*Needed to use non blocking delays*/
#include "Timer.h"
Timer t;

//Software timeclock until we get an RTC
#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define OFFSET_HEADER  "O"   // Header tag for serial time sync message


/*
  Output with Registors
*/
//Include register library
#include "register.h"
Register hour_10(43,42); //10's place hour
Register hour_1(45,44);  //1's place hour
Register min_10(47,46); //10's place min
Register min_1(49,48); //1's place min
Register sec_10(51,50); //10's place sec
Register sec_1(53,52); //1's place sec

//offset
int offset = -8;  // Pacific Standard Time (USA)

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200);
  Serial1.begin(115200); //for time sync messages on WIFI
  pinMode(LED_BUILTIN, OUTPUT);
  //Reset
  Serial.println("Reseting Registers...");
  hour_1.reset();
  hour_10.reset();
  min_1.reset();
  min_10.reset();
  sec_1.reset();
  sec_10.reset();
  Serial.println("Finished Reset.");
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
    if(timeStatus()!= timeSet) 
     Serial.println("Unable to sync with the RTC.");
  else
     Serial.println("RTC has set the system time.");  

  /*Get offset from EEPROM*/
  //offset = EEPROM.readInt(10);
  //if (offset > 12 || offset <-12 )
  //  offset = -8; //PST

  /*Clock service*/
  t.every(1000, digitalClockDisplay);

}

// the loop function runs over and over again forever
void loop() {
  t.update(); //timer service



  if (Serial.available()) {
    /*Menu and Sync processing*/
    time_t t = processSyncMessage();
    /*Time sync Processing*/
    if (t != 0) {
      //adjustTime(offset * SECS_PER_HOUR);
      t = t + offset * SECS_PER_HOUR;
      RTC.set(t);   // set the RTC and the system time to the received value
      setTime(t);   //While on the system uses timer library       
    }
  }

  if (Serial1.available()) {
    /*No menu but look for UNIXTIME*/
    time_t t = processSyncMessage_1();

    /*Time sync Processing*/
    if (t != 0) {
      //adjustTime(offset * SECS_PER_HOUR);
      t = t + offset * SECS_PER_HOUR;
      RTC.set(t);   // set the RTC and the system time to the received value
      setTime(t);   //While on the system uses timer library       
    }
  }

/*
  if (timeStatus()!= timeNotSet) {
    digitalClockDisplay();
    //delay(1000);
  }
*/
  if (timeStatus() == timeSet) {
    digitalWrite(LED_BUILTIN, HIGH); // LED on if synced
  } else {
    digitalWrite(LED_BUILTIN, LOW);  // LED off if needs refresh
  }

}


void digitalClockDisplay(){
if (timeStatus()!= timeNotSet) {
  // digital clock display of the time
  Serial.print(F("Current TIME:"));
  printDigits(hourFormat12());
  printDigits(minute());
  printDigits(second());
  Serial.println(); 

  /*Send to Register*/
  Serial.println("H10:");
  hour_10.countto((int) hourFormat12() / 10);
  Serial.println("H01:");
  hour_1.countto((int) hourFormat12() % 10); 
  Serial.println("M10:");
  min_10.countto((int) minute() / 10);
  Serial.println("M01:");
  min_1.countto((int) minute() % 10);
  Serial.println("S10:");
  sec_10.countto((int) second() / 10);
  Serial.println("S01:");
  sec_1.countto(second() % 10);

  /*Date*/
  Serial.print("Current Date:");
  Serial.print(day());
  Serial.print("/");
  Serial.print(month());
  Serial.print("/");
  Serial.print(year()); 
  Serial.println(); 


}

}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


unsigned long processSyncMessage() {
  /*T<UNIXTIME>
    O<OFFSET>
    Offset should probably be sent first then time
    TODO: RECIEVE THE SAME INFORMATION ON COM2 WHERE WIFI is at
    */
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 

  int buff = Serial.read();
  if (buff == 'T' ){
     pctime = Serial.parseInt();
     //return pctime;
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = 0L; // return 0 to indicate that the time is not valid
     }
  } else if (buff == 'O' ) {
    /*Process offset*/
    offset = Serial.parseInt();
    Serial.print("Offset:");
    Serial.println(offset);

  } else if (buff == '?') {
    Serial.println("MENU:");
    Serial.println("T<UNIXTIME>");
    Serial.println("O<OFFSET>");
    

  }



/*
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 
  //Serial.println("Recieved Time Set Command!");
  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     return pctime;
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = 0L; // return 0 to indicate that the time is not valid
     }
  }
*/

  return pctime;
}


unsigned long processSyncMessage_1() {

  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 
  
  if(Serial1.find(TIME_HEADER)) {
    Serial.println("*********Recieved Time on WIFI!");
     pctime = Serial1.parseInt();
     //return pctime;
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = 0L; // return 0 to indicate that the time is not valid
     }
  }
  return pctime;
}

