/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
  This example code is in the public domain.
 */

#include <Arduino.h>
#include "SPI.h"
#include "pinger.h"
#include "Si4432.h"
#include "RH_RF22.h"


void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  Serial.begin(9600);
  while(!Serial);

#if 0
  RH_RF22 radio(7,3);
  radio.myinit();
  _delay_ms(100);
  radio.spiWrite(Si4432_FREQUENCY_OFFSET1, 0);  // REVISIT
  radio.spiWrite(Si4432_FREQUENCY_OFFSET2, 0);
  radio.spiWrite(Si4432_FREQUENCY_BAND, 0x53);
  radio.spiWrite(Si4432_NOMINAL_CARRIER_FREQUENCY1, 0x76);
  radio.spiWrite(Si4432_NOMINAL_CARRIER_FREQUENCY0, 0xC0);
  _delay_ms(1);
  radio.spiWrite(Si4432_OPERATING_MODE1, xton);
  _delay_ms(1);
  Serial.println(radio.spiRead(Si4432_CRYSTAL_OSCILLATOR_POR_CONTROL), HEX);
  radio.spiWrite(Si4432_OPERATING_MODE1, txon );
  _delay_ms(1);
  for (int i=0; i<8; i++) {
    Serial.println(radio.spiRead(Si4432_CRYSTAL_OSCILLATOR_POR_CONTROL), HEX);
    _delay_ms(1);
  }
  radio.spiWrite(Si4432_OPERATING_MODE1, xton);
  Serial.println(radio.spiRead(Si4432_CRYSTAL_OSCILLATOR_POR_CONTROL), HEX);
  _delay_ms(1);
  Serial.println(radio.spiRead(Si4432_CRYSTAL_OSCILLATOR_POR_CONTROL), HEX);
  // radio.init();
  // radio.setTxPower(RH_RF22_TXPOW_20DBM);
  // radio.setModemConfig(RH_RF22::GFSK_Rb125Fd125);
  // Serial.println(radio.spiRead(Si4432_CRYSTAL_OSCILLATOR_POR_CONTROL), HEX);
  // Serial.println(radio.statusRead(), HEX);
  // uint8_t data[4] = {0,1,2,3};
  // for (int i=0; i<50; i++) {
  //   radio.send(data, 3);
  // }

#else
  Si4432 radio = Si4432();
  _delay_ms(100);
  /** Carrier frequency: 434.75 MHz */
  radio.spi_write_register( Si4432_FREQUENCY_OFFSET1,           0);
  radio.spi_write_register( Si4432_FREQUENCY_OFFSET2,           0);
  radio.spi_write_register( Si4432_FREQUENCY_BAND,              0x53);
  radio.spi_write_register( Si4432_NOMINAL_CARRIER_FREQUENCY1,  0x76);
  radio.spi_write_register( Si4432_NOMINAL_CARRIER_FREQUENCY0,  0xC0);
  _delay_ms(1);
  radio.spi_write_register(Si4432_OPERATING_MODE1, xton);
  _delay_ms(1);
  //Serial.print("xmit off; state = ");
  Serial.println(radio.spi_read_register(Si4432_CRYSTAL_OSCILLATOR_POR_CONTROL), HEX);
  radio.spi_write_register(Si4432_OPERATING_MODE1, txon);
  _delay_ms(1);
  for (int i=0; i<8; i++) {
    //  Serial.print("xmit on; state = ");
    Serial.println(radio.spi_read_register(Si4432_CRYSTAL_OSCILLATOR_POR_CONTROL), HEX);
    _delay_ms(1);
  }
  radio.spi_write_register(Si4432_OPERATING_MODE1, xton);
  Serial.println(radio.spi_read_register(Si4432_CRYSTAL_OSCILLATOR_POR_CONTROL), HEX);
  _delay_ms(1);
  Serial.println(radio.spi_read_register(Si4432_CRYSTAL_OSCILLATOR_POR_CONTROL), HEX);
  return;
  Serial.print("Resetting radio...");
  radio.reset();
  Serial.print("done...");
  if (radio.check_device())
    Serial.println("success!");
  else
    Serial.println("failed.");

  Serial.print("state = ");
  Serial.println(radio.state());

  radio.xmit_tone(500);
  Serial.println("done");
#endif
}

void loop() {
  SPI.end();
  digitalWrite(13, HIGH);   // set the LED on
  delay(500);              // wait for a second
  digitalWrite(13, LOW);    // set the LED off
  delay(500);              // wait for a second
}
