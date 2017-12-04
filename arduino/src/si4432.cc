
#include "SPI.h"
#include "si4432.h"

#define Si4432_SPI_SETTINGS (SPISettings(1000000, MSBFIRST, SPI_MODE0))


// Set up the radio's GPIO pins
void si4432_init_gpio(radiocfg *cfg) {
  si4432_write_register( Si4432_GPIO_CONFIGURATION0, cfg->gpio_configuration0 );
  si4432_write_register( Si4432_GPIO_CONFIGURATION1, cfg->gpio_configuration1 );
  si4432_write_register( Si4432_GPIO_CONFIGURATION2, cfg->gpio_configuration2 );
}

// Set the center frequency
void si4432_init_frequency(radiocfg *cfg) {
  si4432_write_register( Si4432_FREQUENCY_OFFSET1,           cfg->frequency_offset1 );
  si4432_write_register( Si4432_FREQUENCY_OFFSET2,           cfg->frequency_offset2 );
  si4432_write_register( Si4432_FREQUENCY_BAND,              cfg->frequency_band );
  si4432_write_register( Si4432_NOMINAL_CARRIER_FREQUENCY1,  cfg->nominal_carrier_frequency1 );
  si4432_write_register( Si4432_NOMINAL_CARRIER_FREQUENCY0,  cfg->nominal_carrier_frequency0 );
  si4432_write_register( Si4432_FREQUENCY_HOPPING_STEP_SIZE, cfg->frequency_hopping_step_size );
  si4432_write_register( Si4432_FREQUENCY_HOPPING_CHANNEL,   cfg->frequency_hopping_channel );
}

// Set transmit data rate, modulation
void si4432_init_transmit(radiocfg *cfg) {
  si4432_write_register( Si4432_TX_DATA_RATE1,       cfg->tx_data_rate1 );
  si4432_write_register( Si4432_TX_DATA_RATE0,       cfg->tx_data_rate1 );
  si4432_write_register( Si4432_MODULATION_CONTROL1, cfg->modulation_control1 );
  si4432_write_register( Si4432_MODULATION_CONTROL2, cfg->modulation_control2 );
  si4432_write_register( Si4432_FREQUENCY_DEVIATION, cfg->tx_frequency_deviation ); 
}

// Set receiver for anticipated data rate, modulation
void si4432_init_receive(radiocfg *cfg) {
  si4432_write_register( Si4432_IF_FILTER_BANDWIDTH,               cfg->if_filter_bandwidth );
  si4432_write_register( Si4432_AFC_LOOP_GEARSHIFT_OVERRIDE,       cfg->afc_loop_gearshift_override );
  si4432_write_register( Si4432_CLOCK_RECOVERY_GEARSHIFT_OVERRIDE, cfg->clock_recovery_gearshift_override );
  si4432_write_register( Si4432_CLOCK_RECOVERY_OVERSAMPLING_RATE,  cfg->clock_recovery_oversampling_rate );
  si4432_write_register( Si4432_CLOCK_RECOVERY_OFFSET2,            cfg->clock_recovery_offset2 );
  si4432_write_register( Si4432_CLOCK_RECOVERY_OFFSET1,            cfg->clock_recovery_offset1 );
  si4432_write_register( Si4432_CLOCK_RECOVERY_OFFSET0,            cfg->clock_recovery_offset0 );
  si4432_write_register( Si4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN1,  cfg->clock_recovery_timing_loop_gain1 );
  si4432_write_register( Si4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN0,  cfg->clock_recovery_timing_loop_gain0 );
  si4432_write_register( Si4432_OOK_COUNTER_VALUE_1,               cfg->ook_counter_value_1 );
  si4432_write_register( Si4432_OOK_COUNTER_VALUE_2,               cfg->ook_counter_value_2 );
  si4432_write_register( Si4432_SLICER_PEAK_HOLD,                  cfg->slicer_peak_hold );
  si4432_write_register( Si4432_AGC_OVERRIDE1,                     cfg->agc_override1 );
  si4432_write_register( Si4432_AFC_LOOP_GEARSHIFT_OVERRIDE,       cfg->afc_loop_gearshift_override );
  si4432_write_register( Si4432_AFC_TIMING_CONTROL,                cfg->afc_timing_control );
  si4432_write_register( Si4432_FREQUENCY_DEVIATION,               cfg->rx_frequency_deviation ); 
}

// Define format for packet transmit, receive
void si4432_init_packet(radiocfg *cfg) {
  si4432_write_register( Si4432_DATA_ACCESS_CONTROL,         cfg->data_access_control );
  si4432_write_register( Si4432_HEADER_CONTROL1,             cfg->header_control1 );
  si4432_write_register( Si4432_HEADER_CONTROL2,             cfg->header_control2 );
  si4432_write_register( Si4432_PREAMBLE_LENGTH,             cfg->preamble_length );
  si4432_write_register( Si4432_PREAMBLE_DETECTION_CONTROL1, cfg->preamble_detection_control1 );
  si4432_write_register( Si4432_SYNC_WORD3,                  cfg->sync_word3 );
  si4432_write_register( Si4432_SYNC_WORD2,                  cfg->sync_word2 );
  si4432_write_register( Si4432_SYNC_WORD1,                  cfg->sync_word1 );
  si4432_write_register( Si4432_SYNC_WORD0,                  cfg->sync_word0 );
  si4432_write_register( Si4432_TRANSMIT_HEADER3,            cfg->transmit_header3 );
  si4432_write_register( Si4432_TRANSMIT_HEADER2,            cfg->transmit_header2 );
  si4432_write_register( Si4432_TRANSMIT_HEADER1,            cfg->transmit_header1 );
  si4432_write_register( Si4432_TRANSMIT_HEADER0,            cfg->transmit_header0 );
  si4432_write_register( Si4432_PACKET_LENGTH,               cfg->packet_length );
  si4432_write_register( Si4432_CHECK_HEADER3,               cfg->check_header3 );
  si4432_write_register( Si4432_CHECK_HEADER2,               cfg->check_header2 );
  si4432_write_register( Si4432_CHECK_HEADER1,               cfg->check_header1 );
  si4432_write_register( Si4432_CHECK_HEADER0,               cfg->check_header0 );
  si4432_write_register( Si4432_HEADER_ENABLE3,              cfg->header_enable3 );
  si4432_write_register( Si4432_HEADER_ENABLE2,              cfg->header_enable2 );
  si4432_write_register( Si4432_HEADER_ENABLE1,              cfg->header_enable1 );
  si4432_write_register( Si4432_HEADER_ENABLE0,              cfg->header_enable0 );
  si4432_write_register( Si4432_TX_FIFO_CONTROL1,            cfg->tx_fifo_control1 );
  si4432_write_register( Si4432_TX_FIFO_CONTROL2,            cfg->tx_fifo_control2 );
  si4432_write_register( Si4432_RX_FIFO_CONTROL,             cfg->rx_fifo_control );
}

// Transmit the contents of the buffer in packet format
void si4432_xmit_packet (ringbuffer *rb) {
  if (!rbempty(rb)) {
    /* reset and clear FIFO */
    si4432_write_register( Si4432_OPERATING_MODE2, ffclrtx);
    si4432_write_register( Si4432_OPERATING_MODE2, 0x00);
    /* load up the xmit FIFO */
    si4432_write_register( Si4432_PACKET_LENGTH, rblen(rb));
    si4432_write_burst( Si4432_FIFO_ACCESS, rblen(rb), rb);
    /* enable packet sent interrupt, disable others */
    si4432_write_register( Si4432_INTERRUPT_ENABLE1, enpksent);
    si4432_write_register( Si4432_INTERRUPT_ENABLE2, 0x00);
    /* clear any pending interrupts */
    si4432_read_register( Si4432_INTERRUPT_STATUS1);
    si4432_read_register( Si4432_INTERRUPT_STATUS2);
    /* transmit the packet */
    si4432_set_state(XMIT);
    /* turn off transmitter in interrupt handler */
  }
}

// Transmit the contents of the buffer as Morse code
void si4432_xmit_morse  (ringbuffer *rb) {
}

// Transmit the contents of the buffer as distinct tones
void si4432_xmit_tones  (ringbuffer *rb) {
}

// Transmit the contents of the buffer as distinct m-sequences
void si4432_xmit_mseq   (ringbuffer *rb) {
}

// Receive a packet transmission and save in a buffer
bool si4432_recv_packet (ringbuffer *rb) {
  return rbempty(rb);
}

// Receive a Morse code transmission and save in a buffer
bool si4432_recv_morse  (ringbuffer *rb) {
  return rbempty(rb);
}

// Receive a tone transmission and save in a buffer
bool si4432_recv_tones  (ringbuffer *rb) {
  return rbempty(rb);
}

// Poll to see if a transmission has been received
bool si4432_msg_ready(ringbuffer *rb) {
  return rbempty(rb);
}

// Perform a software reset of the radio; wait until done
bool si4432_reset() {
  volatile uint8_t value;
  si4432_write_register( Si4432_OPERATING_MODE1, swres );
  _delay_ms(10);            /* 10 ms delay is 20x max in data sheet */
  value = si4432_read_register( Si4432_INTERRUPT_STATUS1 );
  value = si4432_read_register( Si4432_INTERRUPT_STATUS2 );
  si4432_set_state(READY);
  _delay_ms(10);
  return (value | ipor) ? true : false ;
}

// Check SPI link to the radio
bool si4432_check_device() {
  uint8_t type, version;
  type = si4432_read_register( Si4432_DEVICE_TYPE );
  version = si4432_read_register( Si4432_VERSION_CODE );
  return ((type == device_type_code) && (version == revision_B1));
}

// Set the radio state: ready, transmit, receive, etc
void si4432_set_state(radiostate state) {
  switch (state) {
  case TUNE:
  case SHUTDOWN:
    break;
  case IDLE:
  case STANDBY:
  case SLEEP:
  case SENSOR:
  case READY:
    si4432_write_register( Si4432_OPERATING_MODE1, xton);
    si4432_write_register( Si4432_OPERATING_MODE2, 0x00);
    break;
  case XMIT:
    si4432_write_register( Si4432_TX_POWER, txpow_max | lna_sw );
    si4432_write_register( Si4432_OPERATING_MODE1, txon | xton);
    break;
  case RECV:
    /* not yet tested */
    si4432_write_register( Si4432_OPERATING_MODE1, rxon | xton);
    break;
  }
}

// Retrieve the current state of the radio
radiostate si4432_get_state() {
  uint8_t power = si4432_read_register( Si4432_CRYSTAL_OSCILLATOR_POR_CONTROL );
  switch (power & internal_power_state_mask) {
  case internal_power_state_lp:
    return IDLE;
  case internal_power_state_ready:
    return READY;
  case internal_power_state_tune:
    return TUNE;
  case internal_power_state_tx:
    return XMIT;
  case internal_power_state_rx:
    return RECV;
  }
  return SHUTDOWN;
}

// Return the current carrier frequency of the radio
uint16_t si4432_get_frequency() {
  uint8_t fo1 = si4432_read_register( Si4432_FREQUENCY_OFFSET1 );
  uint8_t fo2 = si4432_read_register( Si4432_FREQUENCY_OFFSET2 );
  uint32_t fo = ((fo2 & 0x03) << 8) + fo1;
  uint8_t fhb = si4432_read_register( Si4432_FREQUENCY_BAND );
  uint8_t fc1 = si4432_read_register( Si4432_NOMINAL_CARRIER_FREQUENCY1 );
  uint8_t fc0 = si4432_read_register( Si4432_NOMINAL_CARRIER_FREQUENCY0 );
  uint32_t fc = (fc1 << 8) + fc0;
  uint32_t fcarr = ((fhb & fb_mask) + 24);
  fcarr *= 10000;
  fcarr += (10*(fc+fo)) >> 6;
  fcarr = (fhb & hbsel) ? fcarr << 1 : fcarr ;
  uint16_t fhch = si4432_read_register( Si4432_FREQUENCY_HOPPING_CHANNEL );
  uint16_t fhs = si4432_read_register( Si4432_FREQUENCY_HOPPING_STEP_SIZE );
  fcarr += 10*fhs*fhch;
  return (uint16_t)fcarr;
}

// Return the current data rate of the tx/rx modem
uint16_t si4432_get_data_rate() {
  uint8_t data1 = si4432_read_register( Si4432_TX_DATA_RATE1 );
  uint8_t data0 = si4432_read_register( Si4432_TX_DATA_RATE0 );
  uint8_t scale = si4432_read_register( Si4432_MODULATION_CONTROL1 );
  uint16_t txdr = (data1 << 8) + data0;
  uint32_t result = 1000000 * txdr;
  return (scale & txdtrtscale) ? result >> 21 : result >> 16 ;
}


// Read a register value from the radio
uint8_t si4432_read_register(uint8_t addr) {
  uint8_t value;
  SPI.beginTransaction( Si4432_SPI_SETTINGS );
  digitalWrite(NSEL_PIN, LOW);           // chip select active
  SPI.transfer( addr & ~BIT7 );          // send address
  value = SPI.transfer(0);               // read value
  SPI.endTransaction();
  digitalWrite(NSEL_PIN, HIGH);          // chip select inactive
  return value;
}

// Write a value to a radio register
void si4432_write_register(uint8_t addr, uint8_t data) {
  SPI.beginTransaction( Si4432_SPI_SETTINGS );
  digitalWrite(NSEL_PIN, LOW);           // chip select active
  SPI.transfer( addr | BIT7 );           // send address
  SPI.transfer( data );                  // send value
  digitalWrite(NSEL_PIN, HIGH);          // chip select inactive
  SPI.endTransaction();
}

// Read a series of register values from the radio
bool si4432_read_burst(uint8_t addr, uint8_t len, ringbuffer *rb) {
  uint8_t i;
  SPI.beginTransaction( Si4432_SPI_SETTINGS );
  digitalWrite(NSEL_PIN, LOW);           // chip select active
  SPI.transfer( addr & ~BIT7 );          // send address
  for (i=0; i<len; i++) {                // read continuously
    if (rbfull(rb)) return false;
    rbput(rb, SPI.transfer(0));
  }
  digitalWrite(NSEL_PIN, HIGH);          // chip select inactive
  SPI.endTransaction();
  return true;
}

// Write a series of register values to the radio
bool si4432_write_burst(uint8_t addr, uint8_t len, ringbuffer *rb) {
  uint8_t i;
  SPI.beginTransaction( Si4432_SPI_SETTINGS );
  digitalWrite(NSEL_PIN, LOW);           // chip select active
  SPI.transfer( addr | BIT7 );           // send address
  for (i=0; i<len; i++) {                // write continuously
    if (rbempty(rb)) return false;
    SPI.transfer( rbget(rb) );
  }
  digitalWrite(NSEL_PIN, HIGH);          // chip select inactive
  SPI.endTransaction();
  return true;
}
