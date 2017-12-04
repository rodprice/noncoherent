
#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* typedef struct { */
/*   uint8_t device_type;                         /\* 00 *\/ */
/*   uint8_t version_code;                        /\* 01 *\/ */
/*   uint8_t device_status;                       /\* 02 *\/ */
/*   uint8_t interrupt_status1;                   /\* 03 *\/ */
/*   uint8_t interrupt_status2;                   /\* 04 *\/ */
/*   uint8_t interrupt_enable1;                   /\* 05 *\/ */
/*   uint8_t interrupt_enable2;                   /\* 06 *\/ */
/*   uint8_t operating_mode1;                     /\* 07 *\/ */
/*   uint8_t operating_mode2;                     /\* 08 *\/ */
/*   uint8_t oscillator_load_capacitance;         /\* 09 *\/ */
/*   uint8_t uc_output_clock;                     /\* 0a *\/ */
/*   uint8_t gpio_configuration0;                 /\* 0b *\/ */
/*   uint8_t gpio_configuration1;                 /\* 0c *\/ */
/*   uint8_t gpio_configuration2;                 /\* 0d *\/ */
/*   uint8_t io_port_configuration;               /\* 0e *\/ */
/*   uint8_t adc_configuration;                   /\* 0f *\/ */
/*   uint8_t adc_sensor_amp_offset;               /\* 10 *\/ */
/*   uint8_t adc_value;                           /\* 11 *\/ */
/*   uint8_t temperature_sensor_calibration;      /\* 12 *\/ */
/*   uint8_t temperature_value_offset;            /\* 13 *\/ */
/*   uint8_t wakeup_timer_period1;                /\* 14 *\/ */
/*   uint8_t wakeup_timer_period2;                /\* 15 *\/ */
/*   uint8_t wakeup_timer_period3;                /\* 16 *\/ */
/*   uint8_t wakeup_timer_value1;                 /\* 17 *\/ */
/*   uint8_t wakeup_timer_value2;                 /\* 18 *\/ */
/*   uint8_t ldc_mode_duration;                   /\* 19 *\/ */
/*   uint8_t low_battery_detector_threshold;      /\* 1a *\/ */
/*   uint8_t battery_voltage_level;               /\* 1b *\/ */
/*   uint8_t if_filter_bandwidth;                 /\* 1c, 2d *\/ */
/*   uint8_t afc_loop_gearshift_override;         /\* 1d, 40, default *\/ */
/*   uint8_t afc_timing_control;                  /\* 1e *\/ */
/*   uint8_t clock_recovery_gearshift_override;   /\* 1f *\/ */
/*   uint8_t clock_recovery_oversampling_ratio;   /\* 20, 40 *\/ */
/*   uint8_t clock_recovery_offset2;              /\* 21, C0 *\/ */
/*   uint8_t clock_recovery_offset1;              /\* 22, 14 *\/ */
/*   uint8_t clock_recovery_offset0;              /\* 23, 7b *\/ */
/*   uint8_t clock_recovery_timing_loop_gain1;    /\* 24, 00 *\/ */
/*   uint8_t clock_recovery_timing_loop_gain0;    /\* 25, 05 *\/ */
/*   uint8_t rssi;                                /\* 26 *\/ */
/*   uint8_t rssi_threshold;                      /\* 27 *\/ */
/*   uint8_t antenna_diversity1;                  /\* 28 *\/ */
/*   uint8_t antenna_diversity2;                  /\* 29 *\/ */
/*   uint8_t afc_limiter;                         /\* 2a *\/ */
/*   uint8_t afc_correction_read;                 /\* 2b *\/ */
/*   uint8_t ook_counter_value_1;                 /\* 2c *\/ */
/*   uint8_t ook_counter_value_2;                 /\* 2d *\/ */
/*   uint8_t slicer_peak_hold;                    /\* 2e *\/ */
/*   uint8_t data_access_control;                 /\* 30, ac *\/ */
/*   uint8_t ezmac_status;                        /\* 31 *\/ */
/*   uint8_t header_control1;                     /\* 32, 8c *\/ */
/*   uint8_t header_control2;                     /\* 33, 02 *\/ */
/*   uint8_t preamble_length;                     /\* 34, 7f *\/ */
/*   uint8_t preamble_detection_control1;         /\* 35, 22 *\/ */
/*   uint8_t sync_word3;                          /\* 36, 2d *\/ */
/*   uint8_t sync_word2;                          /\* 37, d4 *\/ */
/*   uint8_t sync_word1;                          /\* 38, 00 *\/ */
/*   uint8_t sync_word0;                          /\* 39, 00 *\/ */
/*   uint8_t transmit_header3;                    /\* 3a, 00 *\/ */
/*   uint8_t transmit_header2;                    /\* 3b, 00 *\/ */
/*   uint8_t transmit_header1;                    /\* 3c, 00 *\/ */
/*   uint8_t transmit_header0;                    /\* 3d, 00 *\/ */
/*   uint8_t packet_length;                       /\* 3e, ff *\/ */
/*   uint8_t check_header3;                       /\* 3f, 00 *\/ */
/*   uint8_t check_header2;                       /\* 40, 00 *\/ */
/*   uint8_t check_header1;                       /\* 41, 00 *\/ */
/*   uint8_t check_header0;                       /\* 42, 00 *\/ */
/*   uint8_t header_enable3;                      /\* 43, ff *\/ */
/*   uint8_t header_enable2;                      /\* 44, ff *\/ */
/*   uint8_t header_enable1;                      /\* 45, ff *\/ */
/*   uint8_t header_enable0;                      /\* 46, ff *\/ */
/*   uint8_t received_header3;                    /\* 47 *\/ */
/*   uint8_t received_header2;                    /\* 48 *\/ */
/*   uint8_t received_header1;                    /\* 49 *\/ */
/*   uint8_t received_header0;                    /\* 4a *\/ */
/*   uint8_t received_packet_length;              /\* 4b *\/ */
/*   uint8_t analog_test_bus_select;              /\* 50 *\/ */
/*   uint8_t digital_test_bus_select;             /\* 51 *\/ */
/*   uint8_t tx_ramp_control;                     /\* 52 *\/ */
/*   uint8_t pll_tune_time;                       /\* 53 *\/ */
/*   uint8_t calibration_control;                 /\* 55 *\/ */
/*   uint8_t modem_test;                          /\* 56, 00 *\/ */
/*   uint8_t charge_pump_test;                    /\* 57 *\/ */
/*   uint8_t charge_pump_current_trimming;        /\* 58 *\/ */
/*   uint8_t divider_current_trimming;            /\* 59 *\/ */
/*   uint8_t vco_current_trimming;                /\* 5a *\/ */
/*   uint8_t vco_calibration;                     /\* 5b *\/ */
/*   uint8_t synthesizer_test;                    /\* 5c *\/ */
/*   uint8_t block_enable_override1;              /\* 5d *\/ */
/*   uint8_t block_enable_override2;              /\* 5e *\/ */
/*   uint8_t block_enable_override3;              /\* 5f *\/ */
/*   uint8_t channel_filter_coefficient_address;  /\* 60 *\/ */
/*   uint8_t channel_filter_coefficient_value;    /\* 61 *\/ */
/*   uint8_t crystal_oscillator_por_control;      /\* 62 *\/ */
/*   uint8_t rc_oscillator_coarse_calibration;    /\* 63 *\/ */
/*   uint8_t rc_oscillator_fine_calibration;      /\* 64 *\/ */
/*   uint8_t ldo_control_override;                /\* 65 *\/ */
/*   uint8_t ldo_level_settings;                  /\* 66 *\/ */
/*   uint8_t delta_sigma_adc_tuning1;             /\* 67 *\/ */
/*   uint8_t delta_sigma_adc_tuning2;             /\* 68 *\/ */
/*   uint8_t agc_override1;                       /\* 69 *\/ */
/*   uint8_t agc_override2;                       /\* 6a *\/ */
/*   uint8_t gfsk_fir_filter_coefficient_address; /\*;6b *\/ */
/*   uint8_t gfsk_fir_filter_coefficient_value;   /\* 6c *\/ */
/*   uint8_t tx_data_rate1;                       /\* 6e, 05 *\/ */
/*   uint8_t tx_data_rate0;                       /\* 6f, 1f *\/ */
/*   uint8_t modulation_control1;                 /\* 70, 2c *\/ */
/*   uint8_t modulation_control2;                 /\* 71, 63 *\/ */
/*   uint8_t rx_frequency_deviation;              /\* 72, 08 *\/ */
/*   uint8_t tx_frequency_deviation;              /\* 72, 1a *\/ */
/*   uint8_t frequency_offset1;                   /\* 73 *\/ */
/*   uint8_t frequency_offset2;                   /\* 74 *\/ */
/*   uint8_t frequency_band;                      /\* 75, 53 *\/ */
/*   uint8_t nominal_carrier_frequency1;          /\* 76, 76 *\/ */
/*   uint8_t nominal_carrier_frequency0;          /\* 77, c0 *\/ */
/*   uint8_t frequency_hopping_channel;           /\* 79 *\/ */
/*   uint8_t frequency_hopping_step_size;         /\* 7a *\/ */
/*   uint8_t tx_fifo_control1;                    /\* 7c *\/ */
/*   uint8_t tx_fifo_control2;                    /\* 7d *\/ */
/*   uint8_t rx_fifo_control;                     /\* 7e *\/ */
/*   uint8_t fifo_access;                         /\* 7f *\/ */
/* } si4432; */

typedef struct {
  uint8_t operating_mode1;                     /* 07, 0000 0001 */
  uint8_t operating_mode2;                     /* 08, 0000 0000 */
  uint8_t oscillator_load_capacitance;         /* 09, 0111 1111 */
  uint8_t uc_output_clock;                     /* 0a, xx00 0110 */
  uint8_t gpio_configuration0;                 /* 0b, 0000 0000 */
  uint8_t gpio_configuration1;                 /* 0c, 0000 0000 */
  uint8_t gpio_configuration2;                 /* 0d, 0000 0000 */
  uint8_t io_port_configuration;               /* 0e, xxxx x000 */
  uint8_t adc_configuration;                   /* 0f, 0000 0000 */
  uint8_t adc_sensor_amp_offset;               /* 10, xxxx 0000 */
  uint8_t temperature_sensor_calibration;      /* 12, 0010 0000 */
  uint8_t temperature_value_offset;            /* 13, 0000 0000 */
  uint8_t wakeup_timer_period1;                /* 14, xxx0 0011 */
  uint8_t wakeup_timer_period2;                /* 15, 0000 0000 */
  uint8_t wakeup_timer_period3;                /* 16, 0000 0000 */
  uint8_t ldc_mode_duration;                   /* 19, 0000 0001 */
  uint8_t low_battery_detector_threshold;      /* 1a, xxx1 0100 */
  uint8_t if_filter_bandwidth;                 /* 1c, 0000 0001 */
  uint8_t afc_loop_gearshift_override;         /* 1d, 0100 0100 */
  uint8_t afc_timing_control;                  /* 1e, xx00 1010 */
  uint8_t clock_recovery_gearshift_override;   /* 1f, 0000 0011 */
  uint8_t clock_recovery_oversampling_rate;    /* 20, 0110 0100 */
  uint8_t clock_recovery_offset2;              /* 21, 0000 0001 */
  uint8_t clock_recovery_offset1;              /* 22, 0100 0111 */
  uint8_t clock_recovery_offset0;              /* 23, 1010 1110 */
  uint8_t clock_recovery_timing_loop_gain1;    /* 24, 0000 0010 */
  uint8_t clock_recovery_timing_loop_gain0;    /* 25, 1000 1111 */
  uint8_t rssi_threshold;                      /* 27, 0001 1110 */
  uint8_t afc_limiter;                         /* 2a, 0000 0000 */
  uint8_t ook_counter_value_1;                 /* 2c, 0001 1000 */
  uint8_t ook_counter_value_2;                 /* 2d, 1011 1100 */
  uint8_t slicer_peak_hold;                    /* 2e, 0010 1100 */
  uint8_t data_access_control;                 /* 30, 1000 1101 */
  uint8_t header_control1;                     /* 32, 0000 1100 */
  uint8_t header_control2;                     /* 33, 0010 0010 */
  uint8_t preamble_length;                     /* 34, 0000 1000 */
  uint8_t preamble_detection_control1;         /* 35, 0010 1010 */
  uint8_t sync_word3;                          /* 36, 0010 1101 */
  uint8_t sync_word2;                          /* 37, 1101 0100 */
  uint8_t sync_word1;                          /* 38, 0000 0000 */
  uint8_t sync_word0;                          /* 39, 0000 0000 */
  uint8_t transmit_header3;                    /* 3a, 0000 0000 */
  uint8_t transmit_header2;                    /* 3b, 0000 0000 */
  uint8_t transmit_header1;                    /* 3c, 0000 0000 */
  uint8_t transmit_header0;                    /* 3d, 0000 0000 */
  uint8_t packet_length;                       /* 3e, 0000 0000 */
  uint8_t check_header3;                       /* 3f, 0000 0000 */
  uint8_t check_header2;                       /* 40, 0000 0000 */
  uint8_t check_header1;                       /* 41, 0000 0000 */
  uint8_t check_header0;                       /* 42, 0000 0000 */
  uint8_t header_enable3;                      /* 43, 1111 1111 */
  uint8_t header_enable2;                      /* 44, 1111 1111 */
  uint8_t header_enable1;                      /* 45, 1111 1111 */
  uint8_t header_enable0;                      /* 46, 1111 1111 */
  uint8_t adc8_control;                        /* 4f, 0001 0000 */
  uint8_t channel_filter_coefficient_address;  /* 60, 0000 0000 */
  uint8_t crystal_oscillator_por_control;      /* 62, xxx0 0100 */
  uint8_t agc_override1;                       /* 69, 0010 0000 */
  uint8_t tx_power;                            /* 6d, x001 1000 */
  uint8_t tx_data_rate1;                       /* 6e, 0000 1010 */
  uint8_t tx_data_rate0;                       /* 6f, 0011 1101 */
  uint8_t modulation_control1;                 /* 70, 0000 1100 */
  uint8_t modulation_control2;                 /* 71, 0000 0000 */
  uint8_t rx_frequency_deviation;              /* 72, 0010 0000 */
  uint8_t tx_frequency_deviation;              /* 72, 0010 0000 */
  uint8_t frequency_offset1;                   /* 73, 0000 0000 */
  uint8_t frequency_offset2;                   /* 74, 0000 0000 */
  uint8_t frequency_band;                      /* 75, 0111 0101 */
  uint8_t nominal_carrier_frequency1;          /* 76, 1011 1011 */
  uint8_t nominal_carrier_frequency0;          /* 77, 1000 0000 */
  uint8_t frequency_hopping_channel;           /* 79, 0000 0000 */
  uint8_t frequency_hopping_step_size;         /* 7a, 0000 0000 */
  uint8_t tx_fifo_control1;                    /* 7c, 0011 0111 */
  uint8_t tx_fifo_control2;                    /* 7d, 0000 0100 */
  uint8_t rx_fifo_control;                     /* 7e, 0011 0111 */
} si4432;

/* Register values after power-on reset per SiLabs AN440 */
const si4432 defaults = {
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
  .channel_filter_coefficient_address = 0x00,
  .crystal_oscillator_por_control     = 0x04,
  .agc_override1                      = 0x20,
  .tx_power                           = 0x18,
  .tx_data_rate1                      = 0x0a,
  .tx_data_rate0                      = 0x3d,
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
const si4432 slow_packet = {
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

#ifdef __cplusplus
}
#endif

#endif
