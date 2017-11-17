/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
  This example code is in the public domain.
 */

#include <Arduino.h>
#include "pinger.h"
#include "RH_RF22.h"


RH_RF22 radio(7,3);

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  Serial.begin(9600);
  while(!Serial);
  radio.init();
  radio.spiRead(0x03);
}

void loop() {
  Serial.println("hi there");
  
  digitalWrite(13, HIGH);   // set the LED on
  delay(500);              // wait for a second
  digitalWrite(13, LOW);    // set the LED off
  delay(500);              // wait for a second
}
