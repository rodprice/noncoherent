
#define bool uint8_t
#include "si4432.h"

  
/* Register values after power-on reset per SiLabs AN440 */
const radiocfg defaults = {
  .operating_mode1                    = 0x01,
  .operating_mode2                    = 0x00,
  .oscillator_load_capacitance        = 0x7f,
  .uc_output_clock                    = 0x06,
  .gpio_configuration0                = 0x00,
  .gpio_configuration1                = 0x00,
  .gpio_configuration2                = 0x00,
  .io_port_configuration              = 0x00,
  .adc_configuration                  = 0x00,
  .adc_sensor_amp_offset              = 0x00,
  .temperature_sensor_calibration     = 0x20,
  .temperature_value_offset           = 0x00,
  .wakeup_timer_period1               = 0x03,
  .wakeup_timer_period2               = 0x00,
  .wakeup_timer_period3               = 0x00,
  .ldc_mode_duration                  = 0x01,
  .low_battery_detector_threshold     = 0x14,
  .if_filter_bandwidth                = 0x01,
  .afc_loop_gearshift_override        = 0x44,
  .afc_timing_control                 = 0x0a,
  .clock_recovery_gearshift_override  = 0x03,
  .clock_recovery_oversampling_rate   = 0x64,
  .clock_recovery_offset2             = 0x01,
  .clock_recovery_offset1             = 0x47,
  .clock_recovery_offset0             = 0xae,
  .clock_recovery_timing_loop_gain1   = 0x02,
  .clock_recovery_timing_loop_gain0   = 0x8f,
  .rssi_threshold                     = 0x1e,
  .afc_limiter                        = 0x00,
  .ook_counter_value_1                = 0x18,
  .ook_counter_value_2                = 0xbc,
  .slicer_peak_hold                   = 0x2c,
  .data_access_control                = 0x8d,
  .header_control1                    = 0x0c,
  .header_control2                    = 0x22,
  .preamble_length                    = 0x08,
  .preamble_detection_control1        = 0x2a,
  .sync_word3                         = 0x2d,
  .sync_word2                         = 0xd4,
  .sync_word1                         = 0x00,
  .sync_word0                         = 0x00,
  .transmit_header3                   = 0x00,
  .transmit_header2                   = 0x00,
  .transmit_header1                   = 0x00,
  .transmit_header0                   = 0x00,
  .check_header3                      = 0x00,
  .check_header2                      = 0x00,
  .check_header1                      = 0x00,
  .check_header0                      = 0x00,
  .packet_length                      = 0x00,
  .header_enable3                     = 0xff,
  .header_enable2                     = 0xff,
  .header_enable1                     = 0xff,
  .header_enable0                     = 0xff,
  .adc8_control                       = 0x10,
  .digital_test_bus_select            = 0x36,
  .modem_test                         = 0x40,
  .channel_filter_coefficient_address = 0x00,
  .crystal_oscillator_por_control     = 0x04,
  .agc_override1                      = 0x20,
  .tx_power                           = 0x18,
  .tx_data_rate0                      = 0x3d,
  .tx_data_rate1                      = 0x0a,
  .modulation_control1                = 0x0c,
  .modulation_control2                = 0x00,
  .rx_frequency_deviation             = 0x20,
  .tx_frequency_deviation             = 0x20,
  .frequency_offset1                  = 0x00,
  .frequency_offset2                  = 0x00,
  .frequency_band                     = 0x75,
  .nominal_carrier_frequency1         = 0xbb,
  .nominal_carrier_frequency0         = 0x80,
  .frequency_hopping_channel          = 0x00,
  .frequency_hopping_step_size        = 0x00,
  .tx_fifo_control1                   = 0x37,
  .tx_fifo_control2                   = 0x04,
  .rx_fifo_control                    = 0x37
};

/* Configuration for very low-rate packet rx/tx from SiLabs spreadsheet */
const radiocfg slow_packet = {
  /* carrier frequency 434.75 MHz */
  .frequency_band                    = 0x53, /* 75 */
  .nominal_carrier_frequency1        = 0x76, /* 76 */
  .nominal_carrier_frequency0        = 0xc0, /* 77 */
  /* tx data rate 625 bps */
  .tx_data_rate1                     = 0x05, /* 6e (1311 decimal) */
  .tx_data_rate0                     = 0x1f, /* 6f */
  .modulation_control1               = 0x2c, /* 70 (set txdtrtscale bit) */
  /* tx frequency deviation */
  .modulation_control2               = 0x63, /* 71 (fifo mode, gfsk modulation) */
  .tx_frequency_deviation            = 0x08, /* 72 (8 x 625 = 5 kHz) */
  /* receive gfsk/fsk modem configuration */
  .if_filter_bandwidth               = 0x2d, /* 1c */
  .clock_recovery_oversampling_rate  = 0x40, /* 20 */
  .clock_recovery_offset2            = 0xC0, /* 21 */
  .clock_recovery_offset1            = 0x14, /* 22 */
  .clock_recovery_offset0            = 0x7b, /* 23 */
  .clock_recovery_timing_loop_gain1  = 0x00, /* 24 */
  .clock_recovery_timing_loop_gain0  = 0x05, /* 25 */
  .afc_loop_gearshift_override       = 0x40, /* 1d */
  .rx_frequency_deviation            = 0x1A, /* 72 (16.25 kHz) */
  .clock_recovery_gearshift_override = 0x03, /* 1f */
  /* packet handler */
  .data_access_control               = 0xac, /* 30 (rx/tx packet handling, crc enabled) */
  .header_control1                   = 0xf0, /* 32 */
  .header_control2                   = 0x02, /* 33 */
  .preamble_length                   = 0x10, /* 34 (64 bits) */
  .preamble_detection_control1       = 0x22, /* 35 (16 bits detected correctly) */
  .sync_word3                        = 0x2d, /* 36 */
  .sync_word2                        = 0xd4, /* 37 */
  .sync_word1                        = 0x00, /* 38 */
  .sync_word0                        = 0x00, /* 39 */
  .transmit_header3                  = 0x00, /* 3a */
  .transmit_header2                  = 0x00, /* 3b */
  .transmit_header1                  = 0x00, /* 3c */
  .transmit_header0                  = 0x00, /* 3d */
  .packet_length                     = 0x40, /* 3e (64 bytes) */
  .check_header3                     = 0x00, /* 3f */
  .check_header2                     = 0x00, /* 40 */
  .check_header1                     = 0x00, /* 41 */
  .check_header0                     = 0x00, /* 42 */
  .header_enable3                    = 0xff, /* 43 */
  .header_enable2                    = 0xff, /* 44 */
  .header_enable1                    = 0xff, /* 45 */
  .header_enable0                    = 0xff  /* 46 */
};

/* Direct mode configuration to send Morse code */
const radiocfg morse_code = {
  /* carrier frequency 434.75 MHz */
  .frequency_band                    = 0x53, /* 75 */
  .nominal_carrier_frequency1        = 0x76, /* 76 */
  .nominal_carrier_frequency0        = 0xc0, /* 77 */
  /* tx data rate 4096 bps */
  .tx_data_rate1                     = 0x21, /* 6e */
  .tx_data_rate0                     = 0x8e, /* 6f */
  .modulation_control1               = txdtrtscale, /* 70 */
  /* tx frequency deviation */
  .modulation_control2               = tx_data_clock_sdo | dtmod_direct_sdi | modtyp_gfsk, /* 71 */
  .tx_frequency_deviation            = 0x08, /* 72 (8 x 625 = 5 kHz) */
  /* receive gfsk/fsk modem configuration */
  .if_filter_bandwidth               = 0x2d, /* 1c */
  .clock_recovery_oversampling_rate  = 0x40, /* 20 */
  .clock_recovery_offset2            = 0xC0, /* 21 */
  .clock_recovery_offset1            = 0x14, /* 22 */
  .clock_recovery_offset0            = 0x7b, /* 23 */
  .clock_recovery_timing_loop_gain1  = 0x00, /* 24 */
  .clock_recovery_timing_loop_gain0  = 0x05, /* 25 */
  .afc_loop_gearshift_override       = 0x40, /* 1d */
  .rx_frequency_deviation            = 0x1A, /* 72 (16.25 kHz) */
  .clock_recovery_gearshift_override = 0x03, /* 1f */
  /* packet handler */
  .data_access_control               = 0xac, /* 30 (rx/tx packet handling, crc enabled) */
  .header_control1                   = 0xf0, /* 32 */
  .header_control2                   = 0x02, /* 33 */
  .preamble_length                   = 0x10, /* 34 (64 bits) */
  .preamble_detection_control1       = 0x22, /* 35 (16 bits detected correctly) */
  .sync_word3                        = 0x2d, /* 36 */
  .sync_word2                        = 0xd4, /* 37 */
  .sync_word1                        = 0x00, /* 38 */
  .sync_word0                        = 0x00, /* 39 */
  .transmit_header3                  = 0x00, /* 3a */
  .transmit_header2                  = 0x00, /* 3b */
  .transmit_header1                  = 0x00, /* 3c */
  .transmit_header0                  = 0x00, /* 3d */
  .packet_length                     = 0x40, /* 3e (64 bytes) */
  .check_header3                     = 0x00, /* 3f */
  .check_header2                     = 0x00, /* 40 */
  .check_header1                     = 0x00, /* 41 */
  .check_header0                     = 0x00, /* 42 */
  .header_enable3                    = 0xff, /* 43 */
  .header_enable2                    = 0xff, /* 44 */
  .header_enable1                    = 0xff, /* 45 */
  .header_enable0                    = 0xff  /* 46 */
};

const radiocfg raw_mode = {
  /* carrier frequency 434.75 MHz */
  .frequency_band                    = 0x53, /* 75 */
  .nominal_carrier_frequency1        = 0x76, /* 76 */
  .nominal_carrier_frequency0        = 0xc0, /* 77 */
  /* tx data rate 625 bps */
  .tx_data_rate1                     = 0x05, /* 6e (1311 decimal) */
  .tx_data_rate0                     = 0x1f, /* 6f */
  .modulation_control1               = 0x2c, /* 70 (set txdtrtscale bit) */
  /* tx frequency deviation */
  .modulation_control2               = 0x63, /* 71 (fifo mode, gfsk modulation) */
  .tx_frequency_deviation            = 0x08, /* 72 (8 x 625 = 5 kHz) */
  /* receive gfsk/fsk modem configuration */
  .if_filter_bandwidth               = 0x2d, /* 1c */
  .clock_recovery_oversampling_rate  = 0x40, /* 20 */
  .clock_recovery_offset2            = 0xC0, /* 21 */
  .clock_recovery_offset1            = 0x14, /* 22 */
  .clock_recovery_offset0            = 0x7b, /* 23 */
  .clock_recovery_timing_loop_gain1  = 0x00, /* 24 */
  .clock_recovery_timing_loop_gain0  = 0x05, /* 25 */
  .afc_loop_gearshift_override       = 0x40, /* 1d */
  .rx_frequency_deviation            = 0x1A, /* 72 (16.25 kHz) */
  .clock_recovery_gearshift_override = 0x03, /* 1f */
  /* packet handler */
  .data_access_control               = 0xac, /* 30 (rx/tx packet handling, crc enabled) */
  .header_control1                   = 0xf0, /* 32 */
  .header_control2                   = 0x02, /* 33 */
  .preamble_length                   = 0x10, /* 34 (64 bits) */
  .preamble_detection_control1       = 0x22, /* 35 (16 bits detected correctly) */
  .sync_word3                        = 0x2d, /* 36 */
  .sync_word2                        = 0xd4, /* 37 */
  .sync_word1                        = 0x00, /* 38 */
  .sync_word0                        = 0x00, /* 39 */
  .transmit_header3                  = 0x00, /* 3a */
  .transmit_header2                  = 0x00, /* 3b */
  .transmit_header1                  = 0x00, /* 3c */
  .transmit_header0                  = 0x00, /* 3d */
  .packet_length                     = 0x40, /* 3e (64 bytes) */
  .check_header3                     = 0x00, /* 3f */
  .check_header2                     = 0x00, /* 40 */
  .check_header1                     = 0x00, /* 41 */
  .check_header0                     = 0x00, /* 42 */
  .header_enable3                    = 0xff, /* 43 */
  .header_enable2                    = 0xff, /* 44 */
  .header_enable1                    = 0xff, /* 45 */
  .header_enable0                    = 0xff  /* 46 */
};

const radiocfg raw_mode_setting_1 = {
  .modem_test                        = 0x40, /* 0x56, undocumented */
  .if_filter_bandwidth               = 0x3e, /* 0x1c */
  .afc_loop_gearshift_override       = 0x58, /* 0x1d */
  .afc_timing_control                = 0x0a, /* 0x1e */
  .clock_recovery_oversampling_rate  = 0x0d, /* 0x20 */
  .clock_recovery_offset2            = 0x09, /* 0x21 */
  .clock_recovery_offset1            = 0xd4, /* 0x22 */
  .clock_recovery_offset0            = 0x95, /* 0x23 */
  .clock_recovery_timing_loop_gain1  = 0x27, /* 0x24 */
  .clock_recovery_timing_loop_gain0  = 0x54, /* 0x25 */
  .afc_limiter                       = 0x11, /* 0x2a */
  .modulation_control2               = ( tx_data_clock_gpio |
                                         dtmod_direct_sdi   |
                                         modtyp_gfsk ), /* 0x71 */
  // packet handler settings
  .preamble_detection_control1       = 0xF8, /* maximum threshold */
  // set GPIOs
  .gpio_configuration0               = gpiodrv_max | output_rx_data,
  .gpio_configuration1               = gpiodrv_max | output_txrx_data_clock,
  .digital_test_bus_select           = 0x36, /* undocumented */
  // Set Non default registers
  .agc_override1                     = sgin | agcen,
  // avoid false packet detection
  .data_access_control               = enpacrx | encrc,
  .header_control1                   = hdch_mask,
  .header_control2                   = hdlen_mask | synclen_mask | prealen,
  .operating_mode2                   = rxmpk,
};

/* #ifdef Setting2        */
/* // Modem Parameters    */
/* SpiWriteRegister(0x56,0x40); */
/* SpiWriteRegister(0x1C,0xAE);    /\* 153.3 kHz bandwidth *\/ */
/* SpiWriteRegister(0x1D,0x68); */
/* SpiWriteRegister(0x1E,0x0A); */
/* SpiWriteRegister(0x20,0x3E); */
/* SpiWriteRegister(0x21,0x02); */
/* SpiWriteRegister(0x22,0x0C); */
/* SpiWriteRegister(0x23,0x49); */
/* SpiWriteRegister(0x24,0x04); */
/* SpiWriteRegister(0x25,0x60); */
/* SpiWriteRegister(0x2A,0xB0); */
/* SpiWriteRegister(0x71,0x63); */
/* #endif */

/* #ifdef Setting3        */
/* // Modem Parameters    */
/* SpiWriteRegister(0x56,0x40); */
/* SpiWriteRegister(0x1C,0x1E); */
/* SpiWriteRegister(0x1D,0x58); */
/* SpiWriteRegister(0x1E,0x1A); */
/* SpiWriteRegister(0x1F,0x03); */
/* SpiWriteRegister(0x20,0x68); */
/* SpiWriteRegister(0x21,0x01); */
/* SpiWriteRegister(0x22,0x3A); */
/* SpiWriteRegister(0x23,0x92); */
/* SpiWriteRegister(0x24,0x00); */
/* SpiWriteRegister(0x25,0xCB); */
/* SpiWriteRegister(0x2A,0x61); */
/* SpiWriteRegister(0x71,0x63); */
/* #endif */
/* #ifdef Setting4 */
/* // Modem Parameters */
/* SpiWriteRegister(0x56,0x40); */
/* SpiWriteRegister(0x1C,0xAE); */
/* SpiWriteRegister(0x1D,0x58); */
/* SpiWriteRegister(0x1E,0x1A); */
/* SpiWriteRegister(0x20,0x3E); */
/* SpiWriteRegister(0x21,0x02); */
/* SpiWriteRegister(0x22,0x0C); */
/* SpiWriteRegister(0x23,0x49); */
/* SpiWriteRegister(0x24,0x04); */
/* SpiWriteRegister(0x25,0x60); */
/* SpiWriteRegister(0x2A,0x61); */
/* SpiWriteRegister(0x71,0x63); */
/* #endif */
/* #ifdef Setting5 */
/* // Modem Parameters */
/* SpiWriteRegister(0x56,0x40); */
/* SpiWriteRegister(0x1C,0x8E); */
/* SpiWriteRegister(0x1D,0x68); */
/* SpiWriteRegister(0x1E,0x0A); */
/* SpiWriteRegister(0x20,0x68); */
/* SpiWriteRegister(0x21,0x01); */
/* SpiWriteRegister(0x22,0x3A); */
/* SpiWriteRegister(0x23,0x92); */
/* SpiWriteRegister(0x24,0x04); */
/* SpiWriteRegister(0x25,0xB9); */
/* SpiWriteRegister(0x2A,0xB0); */
/* SpiWriteRegister(0x71,0x63); */
/* #endif */
