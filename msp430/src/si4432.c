
#include "beacon.h"
#include "si4432.h"
#include "util.h"


void set_radio_state(radiostate state) {
  switch (state) {
    case READY:
      /* tear down, set up stuff */
      spi_write_register(Si4432_OPERATING_MODE1, xton);
      spi_write_register(Si4432_OPERATING_MODE2, 0x00);
      break;
    case XMIT:
      /* tear down, set up stuff */
      spi_write_register(Si4432_OPERATING_MODE1, txon);
      spi_write_register(Si4432_OPERATING_MODE2, 0x00);
      break;
    case RECV:
      /* tear down, set up stuff */
      spi_write_register(Si4432_OPERATING_MODE1, rxon);
      spi_write_register(Si4432_OPERATING_MODE2, 0x00);
      break;
    }
}


/* If the microcontroller handles the power-on reset for the entire
 * application, [we] recommend that a software reset be provided for
 * the radio every time the microcontroller performs a power-on reset
 * sequence.  The following code section shows how to perform a soft-
 * ware reset for the radio and determine when the reset procedure is
 * finished, allowing the radio to receive SPI commands from the
 * microcontroller.
 */
void si4432_reset() {
  /* Enable POR interrupt, then do software reset */
  spi_write_register(Si4432_INTERRUPT_ENABLE2, enport);
  spi_write_register(Si4432_OPERATING_MODE1, swres);
  LPM3;                         /* wait for nIRQ interrupt */
  P1OUT |= READY_LED_PIN;       /* turn on power light */
}

/* Check that communication over the SPI bus is taking place */
void si4432_check_device() {
  uint8_t type, version;
  /* Read device type register, check result */
  type = spi_read_register(Si4432_DEVICE_TYPE);
  while (type != device_type_code) {
    P1OUT ^= READY_LED_PIN;     /* if wrong, blink ready light */
    __delay_cycles(120000);
  }
  /* Read device version register, check result */
  version = spi_read_register(Si4432_VERSION_CODE);
  while (version != revision_B1) {
    P1OUT ^= XMIT_LED_PIN;      /* if wrong, blink xmit light */
    __delay_cycles(120000);
  }
}

/* Set up Si4432 GPIO pins for direct mode transmission */
void si4432_configure_gpio() {
  spi_write_register(           /* GPIO0 is xmit data clock */
    Si4432_GPIO_CONFIGURATION0,
    output_txrx_data_clock );
  spi_write_register(           /* GPIO1 is xmit data input */
    Si4432_GPIO_CONFIGURATION1,
    input_tx_direct_modulation_data );
  enable_clock_irq();           /* set up MSP430 pins to match */
}

/* Frequency control stuff */
void si4432_set_frequency() {
  /** Crystal oscillator load capacitance: 12.500 pF */
  spi_write_register( Si4432_OSCILLATOR_LOAD_CAPACITANCE, xlc_mask);

  /** Carrier frequency: 434.75 MHz */
  spi_write_register( Si4432_FREQUENCY_OFFSET1,           0);
  spi_write_register( Si4432_FREQUENCY_OFFSET2,           0);
  spi_write_register( Si4432_FREQUENCY_BAND,              sbsel | 0x13);
  spi_write_register( Si4432_NOMINAL_CARRIER_FREQUENCY1,  0x76);
  spi_write_register( Si4432_NOMINAL_CARRIER_FREQUENCY0,  0xC0);

  /** Frequency hopping: none */
  spi_write_register( Si4432_FREQUENCY_HOPPING_STEP_SIZE, 0);
  spi_write_register( Si4432_FREQUENCY_HOPPING_CHANNEL,   0);
}


/* Transmitter setup */
void si4432_init_tx_modem() {
  /** Transmit power: 20 dBm */
  spi_write_register( Si4432_TX_POWER, txpow_max);

  /** Transmit data rate: 4096 bps */
  spi_write_register( Si4432_TX_DATA_RATE1, 0x21); /* 0x218E = 8590 */
  spi_write_register( Si4432_TX_DATA_RATE1, 0x8E); /* gives 4096.03 bps */

  /** Transmit mode: direct mode, clock and data on GPIO */
  spi_write_register( Si4432_MODULATION_CONTROL1, \
                      txdtrtscale ); /* data rate < 30 kbps */
  spi_write_register( Si4432_MODULATION_CONTROL2, \
                      tx_data_clock_gpio    |     \
                      dtmod_direct_gpio     |     \
                      modtype_gfsk );

  /** Frequency deviation: 5000 Hz, BW = 18192 Hz by Carson's rule */
  spi_write_register( Si4432_FREQUENCY_DEVIATION, 8); 
  /* Value of 8 with data rate 4096 bps gives modulation index beta =
     1.22, which by Carson's law gives BW = 2*(beta+1)*rate = 18,192
     Hz.  Remember to set freq_deviation_msb = 0 */

  /* TODO: setup GPIO pins for xmit data and clock */
}


/* Receiver setup */
void si4432_init_rx_modem() {
  /* trusting SiLab's program here */
  spi_write_register(Si4432_IF_FILTER_BANDWIDTH,                   0x2B);
  spi_write_register(Si4432_AFC_LOOP_GEARSHIFT_OVERRIDE,           0x40);
  spi_write_register(Si4432_AFC_TIMING_CONTROL,                    0x0A);
  spi_write_register(Si4432_CLOCK_RECOVERY_GEARSHIFT_OVERRIDE,     0x03);
  spi_write_register(Si4432_CLOCK_RECOVERY_OVERSAMPLING_RATIO,     0xFA);
  spi_write_register(Si4432_CLOCK_RECOVERY_OFFSET2,                0x00);
  spi_write_register(Si4432_CLOCK_RECOVERY_OFFSET1,                0x83);
  spi_write_register(Si4432_CLOCK_RECOVERY_OFFSET0,                0x12);
  spi_write_register(Si4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN1,      0x01);
  spi_write_register(Si4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN0,      0x1A);
  spi_write_register(Si4432_AFC_LIMIT,                             0x1D);
  spi_write_register(Si4432_AGC_OVERRIDE1,                         0x60);
}
  
/*   // wait at least 15 ms for POR to complete, or wait for an interrupt */
/*   // from the radio from nIRQ.  Check the "ipor" interrupt status bit, */
/*   // which will be set if POR finished correctly. */
  
/*   // read interrupt status registers to clear interrupt flags and */
/*   // release nIRQ pin */
/*   it_status1 = spi_read_register(0x03); // interrupt status 1 reg */
/*   it_status2 = spi_read_register(0x04); // interrupt status 1 reg */
/* } */

/* /\* The following section sets the basic RF parameters needed to do */
/*  * OOK, FSK, or GFSK modulated packet transmission. These parameters */
/*  * include center frequency, transmit data rate, and transmit */
/*  * deviation. *\/ */

/* void si4432_set_rf_parameters() { */
/*   // set center frequency to 915 MHz */
/*   spi_write_register(0x75, 0x75); // frequency band select register */
/*   spi_write_register(0x76, 0xBB); // nominal carrier frequency 1 */
/*   spi_write_register(0x77, 0x80); // nominal carrier frequency 0 */
/*   // set the desired tx data rate (9.6 kbps) */
/*   spi_write_register(0x6E, 0x4E); // tx data rate 1 register */
/*   spi_write_register(0x6F, 0xA5); // tx data rate 0 register */
/*   spi_write_register(0x70, 0x2C); // modulation mode control 1 */
/*   // set desired tx deviation (+/- 45 kHz) */
/*   spi_write_register(0x72, 0x48); // frequency deviation register */
  
/*   /\* Besides the TX Data Rate 0 and TX Data Rate 1 registers, an */
/*    * additional bit is used to define the data rate: 'txdtrtscale' */
/*    * (bit5 in the Modulation Mode Control1 register).  The */
/*    * 'txdtrtscale' bit must be set to 1 if the desired data rate is */
/*    * below 30 kbps.  The MSB of the deviation setting can be found in */
/*    * the Modulation Mode Control register 2 (fd[8] -- bit2). *\/ */
/* } */

/* /\* Packet structure: */
/*  * programmable preamble (up to 255 bytes) */
/*  * programmable sync word length and pattern (up to four bytes) */
/*  * automatic header generation and qualification (up to four bytes) */
/*  * fixed and variable length packets */
/*  * payload data up to 64 bytes (using the built-in FIFO) */
/*  * automatic CRC calculation and verification */
/*  * */
/*  * Demo code packet configuration. */
/*  * preamble       : 55 55 55 55 55 */
/*  * sync pattern   : 2D D4 */
/*  * payload length : 08 */
/*  * payload        : 42 75 74 74 6F 6E 31 0D  */
/*  * CRC            : 7A B1 */
/*  * */
/*  * The packet payload is set by the push buttons in the demo circuit. */
/*  * The payload above is "Button1 + CR", whatever that means.  The */
/*  * length of the payload will be included in the packet in this */
/*  * example. */
/*  *\/ */

/* void si4432_set_packet_config() { */
/*   // preamble length 5 bytes */
/*   spi_write_register(0x34, 0x09); // preamcle length register */

/*   // disable header bytes, variable packet length, two-byte sync word */
/*   spi_write_register(0x33, 0x02); // header control 2 register */
/*   /\* Note: The MSB bit of the preamble length setting is located in */
/*    * this register (0x33, 'prealen8' -- bit0 in the Header Control2 */
/*    * register). *\/ */
  
/*   // set sync word pattern to 0x2DD4 */
/*   spi_write_register(0x36, 0x2D); // sync word 3 register */
/*   spi_write_register(0x37, 0xD4); // sync word 2 register */

/*   // enable tx packet handler and CRC-16 (IBM) check */
/*   spi_write_register(0x30, 0x0D); // data access control register */

/*   // enable FIFO mode and GFSK modulation */
/*   spi_write_register(0x71, 0x63); // modulation mode control 2 */

/*   // GPIO config specific to SiLabs test circuit (antenna RF switch) */
/*   spi_write_register(0x08, 0x18); */
/* } */

/* void si4432_select_modulation() { */
/*   // modulation mode control 2 register selects modulation (??) */
/* #ifdef SI4432_REVISION_V2 */
/*   spi_write_register(0x59, 0x40); // divider current trimming register */
/*   spi_write_register(0x5A, 0x7F); // VCO current trimming register */
/* #endif */
/* #ifdef SI4432_REVISION_A0 */
/*   spi_write_register(0x57, 0x01); // charge pump test register */
/*   spi_write_register(0x59, 0x40); // divider current trimming register */
/*   spi_write_register(0x5A, 0x7F); // VCO current trimming register */
/* #endif */
/* } */

/* void si4432_set_crystal_capacitance() { */
/*   spi_write_register(0x09, 0xD7); // crystal osc load capacitance */
/*   /\* experiment with until proper center frequency found *\/ */
/* } */

/* void si4432_xmit_packet() { */
/*   // load up the FIFO through the FIFO access register */
/*   spi_write_register(0x7f, 0x42); // 'B' */
/*   spi_write_register(0x7f, 0x42); // 'U' */
/*   spi_write_register(0x7f, 0x42); // 'T' */
/*   spi_write_register(0x7f, 0x42); // 'T' */
/*   spi_write_register(0x7f, 0x42); // 'O' */
/*   spi_write_register(0x7f, 0x42); // 'N' */
/*   spi_write_register(0x7f, 0x42); // '1' */
/*   spi_write_register(0x7f, 0x42); // '\r' */

/*   // enable packet sent, disable all other interrupts */
/*   spi_write_register(0x05, 0x04); // interrupt enable 1 register */
/*   spi_write_register(0x06, 0x00); // interrupt enable 2 register */

/*   // read interrupt status registers to clear all pending interrupts */
/*   // and set nIRQ pin high */
/*   it_status1 = spi_write_register(0x03); // interrupt status 1 */
/*   it_status2 = spi_write_register(0x04); // interrupt status 2 */

/*   // enable transmitter and send packet */
/*   spi_write_register(0x07, 0x09); // operating function control 1 */

/*   // wait for packet sent interrupt 'ipksent' */
/*   while(nIRQ == 1); */

/*   // release the interrupt flags */
/*   it_status1 = spi_write_register(0x03); // interrupt status 1 */
/*   it_status2 = spi_write_register(0x04); // interrupt status 2 */
/* } */
