
#include <stdint.h>
#include <stdarg.h>
#include "Si4432.h"

#include "Arduino.h"
#include "SPI.h"


#define Si4432_SPI_SETTINGS (SPISettings(1000000, MSBFIRST, SPI_MODE0))


// Temporary storage for packet contents
#define DATALEN 24
static uint8_t _data[DATALEN];


Si4432::Si4432() {
  // configure mode: direct or packet
  _direct = false;
  // configure SPI
  pinMode(NSEL_PIN, OUTPUT);
  pinMode(NIRQ_PIN, INPUT_PULLUP);
  SPI.begin();
  digitalWrite(NSEL_PIN, HIGH);
  // initialize static data
  for (uint8_t i=0; i<DATALEN; i++)
    _data[i] = 0;
  // configure Si4432 radio
  // configure_gpio();
  // set_frequency();
  // packet_config();
  // set_state(READY);
  _delay_ms(10);
}


// Read a register value from the radio
uint8_t Si4432::spi_read_register(uint8_t addr) {
  uint8_t value;
  digitalWrite(NSEL_PIN, LOW);           // chip select active
  SPI.beginTransaction(Si4432_SPI_SETTINGS);
  value = SPI.transfer( addr & ~BIT7 );  // send address
  value = SPI.transfer( 0 );             // read value
  SPI.endTransaction();
  digitalWrite(NSEL_PIN, HIGH);          // chip select inactive
  return value;
}

// Read a series of register values from the radio
uint8_t* Si4432::spi_read_burst(uint8_t addr, uint8_t *data, uint8_t len) {
  uint8_t i, value;
  SPI.beginTransaction(Si4432_SPI_SETTINGS);
  digitalWrite(NSEL_PIN, LOW);           // chip select active
  value = SPI.transfer( 0 );             // send address
  for (i=0; i<len; i++)                  // read continuously
    data[i] = SPI.transfer( addr & ~BIT7 );
  digitalWrite(NSEL_PIN, HIGH);          // chip select inactive
  SPI.endTransaction();
  return data;
}

// Write a value to a radio register
void Si4432::spi_write_register(uint8_t addr, uint8_t data) {
  uint8_t  value;
  SPI.beginTransaction(Si4432_SPI_SETTINGS);
  digitalWrite(NSEL_PIN, LOW);           // chip select active
  value = SPI.transfer( addr | BIT7 );   // send address
  value = SPI.transfer( 0 );             // send value
  digitalWrite(NSEL_PIN, HIGH);          // chip select inactive
  SPI.endTransaction();
}

// Write a series of register values to the radio
void Si4432::spi_write_burst(uint8_t addr, uint8_t *data, uint8_t len) {
  uint8_t i, value;
  SPI.beginTransaction(Si4432_SPI_SETTINGS);
  digitalWrite(NSEL_PIN, LOW);           // chip select active
  value = SPI.transfer( addr | BIT7 );   // send address
  for (i=0; i<len; i++)                  // write continuously
    data[i] = SPI.transfer( 0 );
  digitalWrite(NSEL_PIN, HIGH);          // chip select inactive
  SPI.endTransaction();
}

// Query the radio for state, returning a string
char* Si4432::state() {
  return show_state(get_power_state());
}

// Query the radio for state information
radiostate Si4432::get_state() {
  uint8_t status = spi_read_register(Si4432_DEVICE_STATUS);
  switch (status & 0x03) {
  case cpsidle:
    return READY;
  case cpsrx:
    if (_direct)
      return RECV_DIRECT;
    else
      return RECV_PACKET;
  case cpstx:
    if (_direct)     
      return XMIT_DIRECT;
    else
      return XMIT_PACKET;
  }
  return NULL;
}

radiostate Si4432::get_power_state() {
  uint8_t power_state = spi_read_register(Si4432_CRYSTAL_OSCILLATOR_POR_CONTROL);
  switch(power_state & internal_power_state_mask) {
  case internal_power_state_lp:
    return IDLE;
  case internal_power_state_ready:
    return READY;
  case internal_power_state_tune:
    return TUNE;
  case internal_power_state_rx:
   if (_direct)
      return RECV_DIRECT;
    else
      return RECV_PACKET;
  case internal_power_state_tx:
     if (_direct)
      return XMIT_DIRECT;
    else
      return XMIT_PACKET;
  }
  return SHUTDOWN;
}

char* Si4432::show_state(radiostate state) {
  switch (state) {
  case SHUTDOWN:
    return "SHUTDOWN";
  case IDLE:
    return "IDLE";
  case STANDBY:
    return "STANDBY";
  case SLEEP:
    return "SLEEP";
  case SENSOR:
    return "SENSOR";
  case READY:
    return "READY";
  case TUNE:
    return "TUNE";
  case XMIT_DIRECT:
    return "XMIT_DIRECT";
  case XMIT_PACKET:
    return "XMIT_PACKET";
  case RECV_DIRECT:
    return "RECV_DIRECT";
  case RECV_PACKET:
    return "RECV_PACKET";
  }
  return "unknown";
}


// Query the radio for status, defined as the EZMAC status register
uint8_t Si4432::status() {
  return spi_read_register(Si4432_EZMAC_STATUS);
}

/* There's a chicken and egg thing going on here.  If you wait for the
   nIRQ pin to go high, as the documentation says, you will wait
   forever, because the nIRQ pin won't go high until you read the
   interrupt status registers.  If, however, you read the status
   registers right away, you'll never read anything, because the
   Si4432 is still in reset.  For that reason, I'll just wait. */
bool Si4432::reset() {
  volatile uint8_t value;
  spi_write_register(Si4432_OPERATING_MODE1, swres);
  _delay_ms(10);            /* 10 ms delay is 20x max in data sheet */
  value = spi_read_register(Si4432_INTERRUPT_STATUS1);
  value = spi_read_register(Si4432_INTERRUPT_STATUS2);
  set_state(READY);
  _delay_ms(10);
  return (value | ipor) ? true : false ;
}

/* Check that communication over the SPI bus is taking place */
bool Si4432::check_device() {
  uint8_t type, version;
  type = spi_read_register(Si4432_DEVICE_TYPE);
  version = spi_read_register(Si4432_VERSION_CODE);
  return ((type == device_type_code) && (version == revision_B1));
}

// interrupt serive routine for direct mode
#define AUDIO_TICKS 4
volatile uint16_t aticker = 0;
volatile uint8_t signal = 0;
void direct_isr() {
  aticker -= 1;
  if (aticker == 0) {
    aticker = AUDIO_TICKS;
    digitalWrite(SDI_PIN, signal);
    signal = signal ? false : true ;
  }
}

/* Transmit tone for `duration` milliseconds */
bool Si4432::xmit_tone(uint16_t duration) {
  digitalWrite(NSEL_PIN, HIGH); // just to be sure
  init_tx_direct();             // tell radio to go to direct mode
  digitalWrite(SDI_PIN, LOW);

  Serial.print("xmit off; state = ");
  Serial.println(spi_read_register(Si4432_CRYSTAL_OSCILLATOR_POR_CONTROL), HEX);
  set_state(XMIT_DIRECT);       // start transmission
  for (int i=0; i<50; i++) {
    Serial.print("xmit on; state = ");
    Serial.println(spi_read_register(Si4432_CRYSTAL_OSCILLATOR_POR_CONTROL), HEX);
  }
  Serial.println("stopping SPI");
  
  SPI.end();                    // turn off so we can use SDI (MOSI)
  pinMode(SDO_PIN, INPUT_PULLUP);
  aticker = AUDIO_TICKS;
  signal = 0;
  attachInterrupt(digitalPinToInterrupt(SDO_PIN), direct_isr, FALLING);
  _delay_ms(duration);
  detachInterrupt(digitalPinToInterrupt(SDO_PIN));
  SPI.begin();
  Serial.print("SPI back on, state = ");
  Serial.println(show_state(get_power_state()));
  set_state(READY);             // stop transmission
  Serial.print("xmit off; state = ");
  Serial.println(show_state(get_power_state()));
  return true;
}

/* Set up Si4432 GPIO pins for use with Tinyduino */
void Si4432::configure_gpio() {
  spi_write_register(           /* controls transmit antenna */
    Si4432_GPIO_CONFIGURATION0,
    output_rx_state);
  spi_write_register(           /* controls receive antenna */
    Si4432_GPIO_CONFIGURATION1,
    output_tx_state);
  // spi_write_register(           /* set to output gnd */
  //   Si4432_GPIO_CONFIGURATION2,
  //   0x1F);
}

/* Frequency control stuff */
void Si4432::set_frequency() {
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
void Si4432::init_tx_direct() {
  /** Transmit power: ?? dBm */
  spi_write_register( Si4432_TX_POWER, txpow_max | lna_sw );

  /** Transmit data rate: 4096 bps */
  spi_write_register( Si4432_TX_DATA_RATE1, 0x21); /* 0x218E = 8590 */
  spi_write_register( Si4432_TX_DATA_RATE0, 0x8E); /* gives 4096.03 bps */

  /** Transmit mode: direct mode, clock and data on GPIO */
  spi_write_register( Si4432_MODULATION_CONTROL1, \
                      txdtrtscale ); /* data rate < 30 kbps */
  spi_write_register( Si4432_MODULATION_CONTROL2, \
                      tx_data_clock_sdo     |     \
                      dtmod_direct_sdi      |     \
                      modtyp_gfsk );

  /* Frequency deviation: 5000 Hz, BW = 18192 Hz by Carson's rule */
  spi_write_register( Si4432_FREQUENCY_DEVIATION, 8); 
}

void Si4432::init_tx_packet() {
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
void Si4432::init_rx_packet() {
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

void Si4432::packet_config() {
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
  spi_write_register(Si4432_SYNC_WORD1, 0x00);
  spi_write_register(Si4432_SYNC_WORD0, 0x00);
  spi_write_register(Si4432_HEADER_CONTROL2, 0x22); /* por default value */
  spi_write_register(Si4432_HEADER_ENABLE3, 0xFF);
  spi_write_register(Si4432_HEADER_ENABLE2, 0xFF);
  spi_write_register(Si4432_HEADER_ENABLE1, 0xFF);
  spi_write_register(Si4432_HEADER_ENABLE0, 0xFF);
}

void Si4432::load_packet(uint8_t *data, uint8_t len) {
  /* reset and clear FIFO */
  spi_write_register(Si4432_OPERATING_MODE2, ffclrtx);
  spi_write_register(Si4432_OPERATING_MODE2, 0x00);
  /* load up the xmit FIFO */
  spi_write_register(Si4432_PACKET_LENGTH, len);
  spi_write_burst(Si4432_FIFO_ACCESS, data, len);
  spi_write_register(Si4432_PREAMBLE_LENGTH, 8); /* set preamble length */
  /* enable packet sent interrupt, disable others */
  spi_write_register(Si4432_INTERRUPT_ENABLE1, enpksent);
  spi_write_register(Si4432_INTERRUPT_ENABLE2, 0x00);
  /* clear any pending interrupts */
  spi_read_register(Si4432_INTERRUPT_STATUS1);
  spi_read_register(Si4432_INTERRUPT_STATUS2);
}

/* Set Si4432 chip state, turn on/off transmitter and receiver */
void Si4432::set_state(radiostate state) {
  switch (state) {
  case TUNE:
  case SHUTDOWN:
    break;
  case IDLE:
  case STANDBY:
  case SLEEP:
  case SENSOR:
  case READY:
    spi_write_register(Si4432_OPERATING_MODE1, xton);
    spi_write_register(Si4432_OPERATING_MODE2, 0x00);
    break;
  case XMIT_DIRECT:
    // init_tx_direct();
    spi_write_register(Si4432_TX_POWER, txpow_max | lna_sw );
    spi_write_register(Si4432_OPERATING_MODE1, txon | xton);
    // spi_write_register(Si4432_OPERATING_MODE2, 0x00);
    _delay_ms(10);
    break;
  case XMIT_PACKET:
    init_tx_packet();
    spi_write_register(Si4432_TX_POWER, txpow_max | lna_sw );
    spi_write_register(Si4432_OPERATING_MODE1, txon | xton);
    break;
  case RECV_DIRECT:
    /* unimplemented, probably won't */
    set_state(READY);
    break;
  case RECV_PACKET:
    /* not yet tested */
    init_rx_packet();
    spi_write_register(Si4432_OPERATING_MODE1, rxon | xton);
    break;
  }
}

