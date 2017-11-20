
#include "beacon.h"
#include "si4432.h"
#include "util.h"


/* There's a chicken and egg thing going on here.  If you wait for the
   nIRQ pin to go high, as the documentation says, you will wait
   forever, because the nIRQ pin won't go high until you read the
   interrupt status registers.  If, however, you read the status
   registers right away, you'll never read anything, because the
   Si4432 is still in reset.  For that reason, I'll just wait. */
void si4432_reset() {
  volatile uint8_t value;
  spi_write_register(Si4432_OPERATING_MODE1, swres);
  __delay_cycles(80000);        /* 10 ms delay is 20x max in data sheet */
  value = spi_read_register(Si4432_INTERRUPT_STATUS1);
  value = spi_read_register(Si4432_INTERRUPT_STATUS2);
  if (value | ipor)
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
  spi_write_register(           /* GPIO2 is recv data output */
    Si4432_GPIO_CONFIGURATION2,
    output_rx_data );
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
void si4432_init_tx_tone() {
  /** Transmit power: ?? dBm */
  spi_write_register( Si4432_TX_POWER, txpow_max | lna_sw );

  /** Transmit data rate: 4096 bps */
  spi_write_register( Si4432_TX_DATA_RATE1, 0x21); /* 0x218E = 8590 */
  spi_write_register( Si4432_TX_DATA_RATE0, 0x8E); /* gives 4096.03 bps */

  /** Transmit mode: direct mode, clock and data on GPIO */
  spi_write_register( Si4432_MODULATION_CONTROL1, \
                      txdtrtscale ); /* data rate < 30 kbps */
  spi_write_register( Si4432_MODULATION_CONTROL2, \
                      tx_data_clock_gpio    |     \
                      dtmod_direct_gpio     |     \
                      modtyp_gfsk );

  /** Frequency deviation: 5000 Hz, BW = 18192 Hz by Carson's rule */
  spi_write_register( Si4432_FREQUENCY_DEVIATION, 8); 
}

void si4432_init_tx_packet() {
  /** Transmit power: ?? dBm */
  spi_write_register( Si4432_TX_POWER, txpow_max | lna_sw );

  /** Transmit data rate: 2000 bps */
  spi_write_register( Si4432_TX_DATA_RATE1, 0x10); /* 0x1062 = 4194 */
  spi_write_register( Si4432_TX_DATA_RATE0, 0x62); /* gives 1999.8 bps */

  /** Transmit mode: FIFO mode, GFSK modulation */
  spi_write_register( Si4432_MODULATION_CONTROL1,
                      txdtrtscale           | /* data rate < 30 kbps */
                      manppol               | /* sets preamble polarity */
                      enmaninv );             /* preamble bits inverted */
  spi_write_register( Si4432_MODULATION_CONTROL2,
                      tx_data_clock_none    | /* data clock not available */
                      dtmod_fifo            | /* FIFO mode */
                      modtyp_gfsk );          /* GFSK modulation */

  /** Frequency deviation: 5000 Hz, BW = 14 kHz by Carson's rule */
  spi_write_register( Si4432_FREQUENCY_DEVIATION, 8); 
}

/* Set up to receive 2 kbps, 5 kHz deviation packets */
void si4432_init_rx_packet() {
  /* IF_FILTER_BANDWIDTH: */
  /* dwn3_bypass = 0, ndec_exp = 2, filset = 0xB = 11 */
  /* decimate by 2^2 = 4, filset = 1 gives 18.9 kHz bandwidth */
  /* decimate by 2^2 = 4, filset = 11 not in table, may give 120 kHz bandwidth! */
  spi_write_register(Si4432_IF_FILTER_BANDWIDTH,                   0x2B);
  spi_write_register(Si4432_CLOCK_RECOVERY_GEARSHIFT_OVERRIDE,     0x03);
  spi_write_register(Si4432_CLOCK_RECOVERY_OVERSAMPLING_RATIO,     0xF4);
  spi_write_register(Si4432_CLOCK_RECOVERY_OFFSET2,                0x20);
  spi_write_register(Si4432_CLOCK_RECOVERY_OFFSET1,                0x41);
  spi_write_register(Si4432_CLOCK_RECOVERY_OFFSET0,                0x89);
  spi_write_register(Si4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN1,      0x01);
  spi_write_register(Si4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN0,      0x1A);
  spi_write_register(Si4432_OOK_COUNTER_VALUE_1,                   0x40);
  spi_write_register(Si4432_OOK_COUNTER_VALUE_2,                   0x0A);
  spi_write_register(Si4432_SLICER_PEAK_HOLD,                      0x1D);
  spi_write_register(Si4432_CHARGE_PUMP_CURRENT_TRIMMING,          0x80);
  /* AGC_OVERRIDE1: read out AGC control from bits [4:0] */
  spi_write_register(Si4432_AGC_OVERRIDE1,                         0x60);
  spi_write_register(Si4432_AFC_LOOP_GEARSHIFT_OVERRIDE,           0x40);
  spi_write_register(Si4432_AFC_TIMING_CONTROL,                    0x0A);
  spi_write_register(Si4432_AFC_LIMIT,                             0x1D);
}

void si4432_packet_config() {
  /* uint8_t tx_fifo_almost_full = ; */
  /* spi_write_register(Si4432_TX_FIFO_CONTROL1, rxafthr_mask); */
  uint8_t tx_fifo_almost_empty = 4; /* threshold for txffaam interrupt */
  uint8_t rx_fifo_almost_full = 55; /* threshold for rxffafull interrupt */
  spi_write_register(Si4432_TX_FIFO_CONTROL2, tx_fifo_almost_empty);
  spi_write_register(Si4432_RX_FIFO_CONTROL, rx_fifo_almost_full);
  spi_write_register(Si4432_DATA_ACCESS_CONTROL,
                     enpacrx    | /* automatic rx packet handling */
                     enpactx    | /* automatic tx packet handling */
                     encrc      | /* check CRC against data fields only */
                     crc_ibm_16 ); /* use IBM 16 CRC polynomial */
  uint8_t bcen = 0x80;            /* broadcast address check enable */
  uint8_t hdch = 0x08;            /* received header bytes to be checked */
  spi_write_register(Si4432_HEADER_CONTROL1, bcen | hdch);
  spi_write_register(Si4432_PREAMBLE_LENGTH, 8); /* set preamble length */
  spi_write_register(Si4432_SYNC_WORD3, 0x2D);   /* first sync byte */
  spi_write_register(Si4432_SYNC_WORD2, 0xD4);   /* second sync byte */
  /* RadioHead doesn't set these!  Hope this is right */
  spi_write_register(Si4432_SYNC_WORD1, 0x00);
  spi_write_register(Si4432_SYNC_WORD0, 0x00);
  spi_write_register(Si4432_HEADER_CONTROL2, 0x22); /* por default value */
  uint8_t promiscuous = 0xFF;   /* this is por default */
  spi_write_register(Si4432_HEADER_ENABLE3, promiscuous);
  /* RadioHead doesn't set these!  Hope this is right */
  spi_write_register(Si4432_HEADER_ENABLE2, promiscuous);
  spi_write_register(Si4432_HEADER_ENABLE1, promiscuous);
  spi_write_register(Si4432_HEADER_ENABLE0, promiscuous);
}

void si4432_load_packet(uint8_t *data, uint8_t len) {
  /* reset and clear FIFO */
  spi_write_register(Si4432_OPERATING_MODE2, ffclrtx);
  spi_write_register(Si4432_OPERATING_MODE2, 0x00);
  /* load up the xmit FIFO */
  spi_write_register(Si4432_PACKET_LENGTH, len);
  spi_burst_write(Si4432_FIFO_ACCESS, data, len);
  spi_write_register(Si4432_PREAMBLE_LENGTH, 8); /* set preamble length */
  /* enable packet sent interrupt, disable others */
  spi_write_register(Si4432_INTERRUPT_ENABLE1, enpksent);
  spi_write_register(Si4432_INTERRUPT_ENABLE2, 0x00);
  /* clear any pending interrupts */
  spi_read_register(Si4432_INTERRUPT_STATUS1);
  spi_read_register(Si4432_INTERRUPT_STATUS2);
}

/* Set Si4432 chip state, turn on/off transmitter and receiver */
void si4432_set_state(radiostate state) {
  switch (state) {
  case READY:
    P1OUT |= RXON_PIN | TXON_PIN; /* turn off recv and xmit */
    P1OUT &= ~XMIT_LED_PIN;       /* turn off transmit light */
    spi_write_register(Si4432_OPERATING_MODE1, xton);
    spi_write_register(Si4432_OPERATING_MODE2, 0x00);
    break;
  case XMIT_DIRECT:
    si4432_init_tx_tone();
    spi_write_register( Si4432_TX_POWER, txpow_max | lna_sw );
    P1OUT |= RXON_PIN;          /* turn off receiver */
    P1OUT &= ~TXON_PIN;         /* turn on transmitter */
    P1OUT |= XMIT_LED_PIN;      /* turn on transmit light */
    spi_write_register(Si4432_OPERATING_MODE1, txon | xton);
    spi_write_register(Si4432_OPERATING_MODE2, 0x00);
    break;
  case XMIT_PACKET:
    si4432_init_tx_packet();
    spi_write_register( Si4432_TX_POWER, txpow_max | lna_sw );
    P1OUT |= RXON_PIN;          /* turn off receiver */
    P1OUT &= ~TXON_PIN;         /* turn on transmitter */
    P1OUT |= XMIT_LED_PIN;      /* turn on transmit light */
    spi_write_register(Si4432_OPERATING_MODE1, txon | xton);
    LPM3;                       /* sleep until done */
    break;
  case RECV_DIRECT:
    /* unimplemented, probably won't */
    si4432_set_state(READY);
    break;
  case RECV_PACKET:
    /* not yet tested */
    si4432_init_rx_packet();
    P1OUT &= ~RXON_PIN;         /* turn on receiver */
    P1OUT |= TXON_PIN;          /* turn off transmitter */
    P1OUT &= ~XMIT_LED_PIN;     /* turn off transmit light */
    spi_write_register(Si4432_OPERATING_MODE1, rxon | xton);
    LPM3;                       /* sleep until done */
    break;
  }
}


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

/* RadioHead packet format */
/// \par Packet Format
///
/// All messages sent and received by this Driver must conform to this packet format:
///
/// - 8 nibbles (4 octets) PREAMBLE
/// - 2 octets SYNC 0x2d, 0xd4
/// - 4 octets HEADER: (TO, FROM, ID, FLAGS)
/// - 1 octet LENGTH (0 to 255), number of octets in DATA
/// - 0 to 255 octets DATA
/// - 2 octets CRC computed with CRC16(IBM), computed on HEADER, LENGTH and DATA
///

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
