/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
  This example code is in the public domain.
 */

#include <Arduino.h>
#include "pinger.h"
#include "si4432.h"


void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Starting");
  
  // Si4432 radio = Si4432();
  // if (radio.check_device())
  //   Serial.println("device check passed");
  // else
  //   Serial.println("device check failed");
  // Serial.print("initial state = ");
  // Serial.println(radio.show_state(radio.get_state()));
  // radio.init_tx_direct();
  // radio.show_modulation_control(true);
  // radio.show_frequency(true);
  // radio.show_data_rate(true);
  // radio.show_ezmac(true);
  // radio.xmit_tone(500);
}

void loop() {
  digitalWrite(13, HIGH);   // set the LED on
  delay(500);              // wait for a second
  digitalWrite(13, LOW);    // set the LED off
  delay(500);              // wait for a second
}
