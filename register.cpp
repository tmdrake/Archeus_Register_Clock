#include "register.h"

Register::Register(byte A_Pin, byte B_Pin) {
  pinMode(A_Pin, OUTPUT);
  pinMode(B_Pin, OUTPUT);
  digitalWrite(A_Pin, LOW);
  digitalWrite(B_Pin, LOW);
  _A_Pin = A_Pin;
  _B_Pin = B_Pin;
  lastnumber = -1;
}

void Register::reset() {
    //Reset, send 11 pulses to reset
    //TODO: make an async reset
  for(int i = 0; i < 12; i++ ){
  //digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  digitalWrite(_A_Pin, HIGH);
  delay(75);                      // wait for a second
  //digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(_A_Pin, LOW);
  delay(75);                      // wait for a second  
  }

}

void Register::countto(int counto) {
//figure out the last number, then count to it... otherwise reset and count
//static int lastnumber = -1;
Serial.print("Last number:");
Serial.println(lastnumber);

if (counto <= 0)
    counto = 10; //Zero is actually 10
//else if (counto > 9)
//    counto = 10; //also zero

if (lastnumber == counto){
  //Serial.println("Nothing to do.");s
  return;
} else if(lastnumber < counto && lastnumber != -1)
{
  
  // New count is just the difference..
  int difference = counto - lastnumber;
  
  Serial.print("Move foward:");
  Serial.println(difference);
  lastnumber = counto;
  counto = difference; //to be processed

} else if(lastnumber > counto)
{
  lastnumber = counto;
  //trigger reset
  //Serial.println("Reset and count from start");
  reset();
  //TODO: Compute number of cycle without reset
} else
{
  lastnumber = counto;
}
  
//Count to the number requested.
  for(int i = 0; i < counto; i++ ){
  //digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  digitalWrite(_A_Pin,HIGH);
    if (i == 0)//trigger a start
      digitalWrite(_B_Pin, HIGH);

  delay(75);                      // wait for a second
  //digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(_A_Pin,LOW);
      if (i == 0)//trigger a start
      digitalWrite(_B_Pin, LOW);
  delay(75);                      // wait for a second  
  }
}

