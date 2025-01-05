/*
* Teleregister clock driver, Used in building a clock.
* currently non async library.
*/

//#include "Timer.h"
//Timer t;

//Used arduino UNO->MEGA


//Software timeclock until we get an RTC
#include <TimeLib.h>
#define TIME_HEADER  "T"   // Header tag for serial time sync message

/*
  Output with Registors
*/
//Include register library
#include "register.h"
Register hour_1(7,6);  //1's place hour
Register hour_10(9,8); //10's place hour
Register min_1(11,10); //1's place min
Register min_10(13,12); //10's place min
Register sec_1(15,14); //1's place min
Register sec_10(17,16); //10's place min

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(COUNTPIN, OUTPUT);
  //digitalWrite(COUNTPIN, LOW);
  //pinMode(STARTPIN, OUTPUT);
  //digitalWrite(STARTPIN, LOW);

  //Reset
  //resetregister();
  hour_1.reset();
  hour_10.reset();
  min_1.reset();
  min_10.reset();
  sec_1.reset();
  sec_10.reset();

  Serial.println("Finished Reset.");

}

// the loop function runs over and over again forever
void loop() {

/*
  if (Serial.available()) {        // If anything comes in Serial (USB),


    int buff = Serial.parseInt();
    Serial.print("I received: ");
    Serial.println(buff);

    //resetregister();
    countto(buff);
    //t.after(10000, resetregister);
  }
*/
  //timer event
  //t.update();

  if (Serial.available()) {
    processSyncMessage();
  }


  if (timeStatus()!= timeNotSet) {
    digitalClockDisplay();
    delay(1000);
  }

  if (timeStatus() == timeSet) {
    digitalWrite(13, HIGH); // LED on if synced
  } else {
    digitalWrite(13, LOW);  // LED off if needs refresh
  }

}


void digitalClockDisplay(){
  // digital clock display of the time
  hour_10.countto((int) hourFormat12() / 10);
  hour_1.countto(hourFormat12() % 10);

  Serial.print(F("Current TIME:"));
  Serial.print(hourFormat12());
  //Serial.print(hour());
  //Serial.print(hourFormat12());
  
  min_10.countto((int) minute() / 10);
  min_1.countto(minute() % 10);
  printDigits(minute());

  sec_10.countto((int) second() / 10);
  sec_1.countto(second() % 10);
  
  printDigits(second());
  Serial.println(); 

  /*
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
  */
  
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime + SECS_PER_HOUR * -5); // Sync Arduino clock to the time received on the serial port
     }
  }
}


