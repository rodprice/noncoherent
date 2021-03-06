
#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#else
typedef enum { false, true } bool;
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
  
#ifdef __cplusplus
}
#endif

#endif
