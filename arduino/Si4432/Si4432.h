#ifndef SI4432_H
#define SI4432_H


/* References:
 *
 * FM Tutorial, SiLabs: https://www.silabs.com/Marcom%20Documents/Resources/FMTutorial.pdf
 *
 * The Silabs site for Si4432 EZRadioPRO
 * https://www.silabs.com/products/wireless/proprietary/ezradiopro-ism-band-transmitters-recievers-transceivers/device.si4432
 *
 * SiLabs AN440, Si4430/31/32 Register Descriptions,
 * https://www.silabs.com/documents/public/application-notes/AN440.pdf
 *
 * SiLabs AN415, EZRadioPRO Programming Guide,
 * https://www.silabs.com/documents/public/application-notes/AN415.pdf
 * 
 */

#include <stdint.h>

// Arduino connections to Si4432 radio
#define NSEL_PIN 7
#define NIRQ_PIN 3
#define SDI_PIN  11             // arduino MOSI
#define SDO_PIN  12             // arduino MISO
#define SCLK_PIN 13             // arduino SCK

// Standard bits
#define BIT0 (0x0001)
#define BIT1 (0x0002)
#define BIT2 (0x0004)
#define BIT3 (0x0008)
#define BIT4 (0x0010)
#define BIT5 (0x0020)
#define BIT6 (0x0040)
#define BIT7 (0x0080)
#define BIT8 (0x0100)
#define BIT9 (0x0200)
#define BITA (0x0400)
#define BITB (0x0800)
#define BITC (0x1000)
#define BITD (0x2000)
#define BITE (0x4000)
#define BITF (0x8000)


/* Values for frequency deviation 3.75 kHz, 4 kbps data rate, which
   gives modulation index \beta = 0.9, so by Carson's rule, BW =
   2(\beta+1) f_m = 15 kHz, where f_m = 4 kHz.
 */

/* Register names */

/****************** Status *******************/

#define Si4432_DEVICE_TYPE                         0x00
#define device_type_code 0x08   /* EZRadioPRO code */

#define Si4432_VERSION_CODE                        0x01
#define revision_B1 0x06        /* are there other codes? */

#define Si4432_DEVICE_STATUS                       0x02
#define ffovfl  BIT7            /* rx/tx FIFO overflow */
#define ffunfl  BIT6            /* rx/tx FIFO underflow  */
#define rxffem  BIT5            /* rx FIFO empty */
#define headerr BIT4            /* header error */
#define freqerr BIT3            /* frequency error */
#define cpsidle 0x00            /* idle power state */
#define cpsrx   0x01            /* rx power state */
#define cpstx   0x02            /* tx power state */


/****************** Interrupts *******************/

/*
 * The Si4430/31/32 is capable of generating an interrupt signal when
 * certain events occur. The chip notifies the microcontroller that an
 * interrupt event has occurred by setting the nIRQ output pin LOW =
 * 0.  This interrupt signal will be generated when any one (or more)
 * of the interrupt events (corresponding to the Interrupt Status
 * bits) shown below occur.  The nIRQ pin will remain low until the
 * microcontroller reads the Interrupt Status Register(s) (Registers
 * 03h-04h) containing the active Interrupt Status bit.  The nIRQ
 * output signal will then be reset until the next change in status is
 * detected.  The interrupts must be enabled by the corresponding
 * enable bit in the Interrupt Enable Registers (Registers 05h-06h).
 * All enabled interrupt bits will be cleared when the microcontroller
 * reads the interrupt status register.  If the interrupt is not
 * enabled when the event occurs it will not trigger the nIRQ pin, but
 * the status may still be read at anytime in the Interrupt Status
 * registers.
*/

#define Si4432_INTERRUPT_STATUS1                   0x03
#define ifferr     BIT7         /* FIFO under/overflow error */
#define itxffsfull BIT6         /* tx FIFO almost full */
#define itxffaem   BIT5         /* tx FIFO almost empty */
#define irxffafull BIT4         /* rx FIFO almost full */
#define iext       BIT3         /* external interrupt */
#define ipksent    BIT2         /* packet sent interrupt */
#define ipkvalid   BIT1         /* valid packet received */
#define icrcerror  BIT0         /* CRC error */

#define Si4432_INTERRUPT_STATUS2                   0x04
#define iswdet     BIT7         /* sync word detected */
#define ipreaval   BIT6         /* valid preamble detected */
#define ipreminval BIT5         /* invalid preamble detected */
#define irssi      BIT4         /* RSSI exceeds threshold*/
#define iwut       BIT3         /* wake-up timer expried */
#define ilbd       BIT2         /* low battery detect */
#define ichiprdy   BIT1         /* chip ready (xtal) */
#define ipor       BIT0         /* power-on reset */

#define Si4432_INTERRUPT_ENABLE1                   0x05
#define enfferr     BIT7        /* enable FIFO under/overflow */
#define entxffafull BIT6        /* enable tx FIFO almost full */
#define entxffaem   BIT5        /* enable tx FIFO almost empty */
#define enrxffafull BIT4        /* enable rx FIFO almost full */
#define enext       BIT3        /* enable external interrupt */
#define enpksent    BIT2        /* enable packet sent */
#define enpkvalid   BIT1        /* enable valid packet received */
#define encrcerror  BIT0        /* enable CRC error */

#define Si4432_INTERRUPT_ENABLE2                   0x06
#define enswdet     BIT7        /* enable sync word detected */
#define enpreaval   BIT6        /* enable valid preamble detected */
#define enpreainval BIT5        /* enable invalid preamble detected */
#define enrssi      BIT4        /* enable RSSI */
#define enwut       BIT3        /* enable wake-up timer */
#define enlbd_ie2   BIT2        /* enable low battery detect */
#define enchiprdy   BIT1        /* enable chip ready (xtal) */
#define enport      BIT0        /* enable power-on reset */


/************* Operating mode and function control ************/

#define Si4432_OPERATING_MODE1                     0x07
#define swres       BIT7        /* software register reset bit */
#define enlbd_mode1 BIT6        /* enable low battery detect */
#define enwt        BIT5        /* enable wake-up timer */
#define x32ksel     BIT4        /* 32768 Hz crystal select */
#define txon        BIT3        /* tx on in manual transmit mode */
#define rxon        BIT2        /* rx on in manual receiver mode */
#define pllon       BIT1        /* tune mode (PLL is on) */
#define xton        BIT0        /* ready mode (xtal is on) */

#define Si4432_OPERATING_MODE2                     0x08
#define antdiv (BIT7|BIT6|BIT5) /* enable antenna diversity */
#define rxmpk   BIT4            /* rx multi-packet */
#define autotx  BIT3            /* automatic transmission */
#define enldm   BIT2            /* enable low duty cycle mode */
#define ffclrrx BIT1            /* rx FIFO reset/clear */
#define ffclrtx BIT0            /* tx FIFO reset/clear */

/* fiddle with the crystal load capacitance */
#define Si4432_OSCILLATOR_LOAD_CAPACITANCE         0x09
#define xtalshift BIT7          /* coarse shift */
#define xlc_mask  0x7F          /* tuning capacitance */

/* fiddle with the clock */
#define Si4432_UC_OUTPUT_CLOCK                     0x0a
#define clktl (BIT7|BIT6)       /* clock tail */
#define enlfc  BIT5             /* enable low frequency clock */
#define mclk30mhz    0          /* 30 MHz clock */
#define mclk15mhz    1          /* 15 MHz clock */
#define mclk10mhz    2          /* 10 MHz clock */
#define mclk4mhz     3          /* 4 MHz clock */
#define mclk3mhz     4          /* 3 MHz clock */
#define mclk2mhz     5          /* 2 MHz clock */
#define mclk1mhz     6          /* 1 MHz clock */
#define mclk32768khz 7          /* 32768 Hz clock */


/************* GPIO configuration ************/

#define Si4432_GPIO_CONFIGURATION0            0x0b
#define pup0 BUT5               /* pull-up resistor enable */
#define output_power_on_reset                 0x00

#define Si4432_GPIO_CONFIGURATION1            0x0c
#define pup1 BIT5               /* pull-up resistor enable */
#define output_inverted_power_on_reset        0x00

#define Si4432_GPIO_CONFIGURATION2            0x0d
#define pup2 BIT5               /* pull-up resistor enable */
#define output_microcontroller_clock          0x00

/* the following apply to all GPIO_CONFIGURATIONx registers */
#define output_wakeup_timer                   0x01
#define output_low_battery_detect             0x02
#define input_direct_digital                  0x03
#define input_external_interrupt_falling_edge 0x04
#define input_external_interrupt_rising_edge  0x05
#define input_external_interrupt_state_change 0x06
#define input_adc_analog                      0x07
#define output_direct_digital                 0x0A
#define output_reference_voltage              0x0E
#define output_txrx_data_clock                0x0F
#define input_tx_direct_modulation_data       0x10
#define input_external_retransmission_request 0x11
#define output_tx_state                       0x12
#define output_tx_fifo_almost_full            0x13
#define output_rx_data                        0x14
#define output_rx_state                       0x15
#define output_rx_fifo_almost_full            0x16
#define output_antenna_diversity_switch_1     0x17
#define output_antenna_diversity_switch_2     0x18
#define output_valid_preamble_detected        0x19
#define output_invalid_preamble_detected      0x1A
#define output_sync_word_detected             0x1B
#define output_clear_channel                  0x1C
#define output_vdd                            0x1D

#define Si4432_IO_PORT_CONFIGURATION               0x0e
#define extitst2 BIT6           /* GPIO2 external interrupt status */
#define extitst1 BIT5           /* GPIO1 external interrupt status */
#define extitst0 BIT4           /* GPIO0 external interrupt status */
#define itsdo    BIT3           /* interrupt req output on SDO pin */
#define dio2     BIT2           /* direct i/o for GPIO2 */
#define dio1     BIT1           /* direct i/o for GPIO1 */
#define dio0     BIT0           /* direct i/o for GPIO0 */

/* analog to digital converter */
#define Si4432_ADC_CONFIGURATION                   0x0f
#define Si4432_ADC_SENSOR_AMP_OFFSET               0x10
#define Si4432_ADC_VALUE                           0x11

/* temperature sensor */
#define Si4432_TEMPERATURE_SENSOR_CALIBRATION      0x12
#define Si4432_TEMPERATURE_VALUE_OFFSET            0x13

/* wake-up timer */
#define Si4432_WAKEUP_TIMER_PERIOD1                0x14
#define Si4432_WAKEUP_TIMER_PERIOD2                0x15
#define Si4432_WAKEUP_TIMER_PERIOD3                0x16
#define Si4432_WAKEUP_TIMER_VALUE1                 0x17
#define Si4432_WAKEUP_TIMER_VALUE2                 0x18

/* low duty cycle  */
#define Si4432_LDC_MODE_DURATION                   0x19

/* battery */
#define Si4432_LOW_BATTERY_DETECTOR_THRESHOLD      0x1a
#define Si4432_BATTERY_VOLTAGE_LEVEL               0x1b

/* IF filter bandwidth -- see table in AN440 */
#define Si4432_IF_FILTER_BANDWIDTH                 0x1c

/* automatic frequency control */
#define Si4432_AFC_LOOP_GEARSHIFT_OVERRIDE         0x1d
#define Si4432_AFC_TIMING_CONTROL                  0x1e

/* clock recovery */
#define Si4432_CLOCK_RECOVERY_GEARSHIFT_OVERRIDE   0x1f
#define Si4432_CLOCK_RECOVERY_OVERSAMPLING_RATIO   0x20
#define Si4432_CLOCK_RECOVERY_OFFSET2              0x21
#define Si4432_CLOCK_RECOVERY_OFFSET1              0x22
#define Si4432_CLOCK_RECOVERY_OFFSET0              0x23
#define Si4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN1    0x24
#define Si4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN0    0x25

/* Received signal strength indicator */
#define Si4432_RSSI                                0x26
#define Si4432_RSSI_THRESHOLD                      0x27

/* antenna diversity */
#define Si4432_ANTENNA_DIVERSITY1                  0x28
#define Si4432_ANTENNA_DIVERSITY2                  0x29

/* more automatic frequency control */
#define Si4432_AFC_LIMIT                           0x2a
#define Si4432_AFC_CORRECTION_READ                 0x2b

/* on-off keyed */
#define Si4432_OOK_COUNTER_VALUE_1                 0x2c
#define Si4432_OOK_COUNTER_VALUE_2                 0x2d
#define Si4432_SLICER_PEAK_HOLD                    0x2e

/* packet stuff */
#define Si4432_DATA_ACCESS_CONTROL                 0x30
#define enpacrx  BIT7           /* enable packet rx handling */
#define lsbfrst  BIT6           /* LSB first enable */
#define crcdonly BIT5           /* CRC data only enable */
#define skip2ph  BIT4           /* skip second phase of preamble detection */
#define enpactx  BIT3           /* enable packet tx handling */
#define encrc    BIT2           /* CRC enable */
#define crc_ccitt    0          /* CRC polynomial */
#define crc_ibm_16   1          /* CRC polynomial */
#define crc_iec_16   2          /* CRC polynomial */
#define crc_biacheva 3          /* CRC polynomial */

#define Si4432_EZMAC_STATUS                        0x31
#define rxcrc1   BIT6           /* last CRC received was all ones */
#define pksrch   BIT5           /* packet searching */
#define pkrx     BIT4           /* packet receiving */
#define pkvalid  BIT3           /* valid packet received */
#define crcerror BIT2           /* CRC error */
#define pktx     BIT1           /* packet transmitting */
#define pksent   BIT0           /* packet sent */

#define Si4432_HEADER_CONTROL1                     0x32
#define bcen_mask 0xF0          /* broadcast address check enable */
#define hdch_mask 0x0F          /* received header bytes to be checked */

#define Si4432_HEADER_CONTROL2                     0x33
#define skipsyn        BIT7     /* skip sync word search timeout */
#define hdlen_mask   ( BIT6 | BIT5 | BIT4 ) /* header length */
#define fixpklen       BIT3     /* fix transmit/receive packet length */
#define synclen_mask ( BIT2 | BIT1 ) /* sync word length */
#define prealen        BIT0     /* MSB of preamble length */

#define Si4432_PREAMBLE_LENGTH                     0x34
#define Si4432_PREAMBLE_DETECTION_CONTROL1         0x35
#define Si4432_SYNC_WORD3                          0x36
#define Si4432_SYNC_WORD2                          0x37
#define Si4432_SYNC_WORD1                          0x38
#define Si4432_SYNC_WORD0                          0x39
#define Si4432_TRANSMIT_HEADER3                    0x3a
#define Si4432_TRANSMIT_HEADER2                    0x3b
#define Si4432_TRANSMIT_HEADER1                    0x3c
#define Si4432_TRANSMIT_HEADER0                    0x3d
#define Si4432_PACKET_LENGTH                       0x3e
#define Si4432_CHECK_HEADER3                       0x3f
#define Si4432_CHECK_HEADER2                       0x40
#define Si4432_CHECK_HEADER1                       0x41
#define Si4432_CHECK_HEADER0                       0x42
#define Si4432_HEADER_ENABLE3                      0x43
#define Si4432_HEADER_ENABLE2                      0x44
#define Si4432_HEADER_ENABLE1                      0x45
#define Si4432_HEADER_ENABLE0                      0x46
#define Si4432_RECEIVED_HEADER3                    0x47
#define Si4432_RECEIVED_HEADER2                    0x48
#define Si4432_RECEIVED_HEADER1                    0x49
#define Si4432_RECEIVED_HEADER0                    0x4a
#define Si4432_RECEIVED_PACKET_LENGTH              0x4b
#define Si4432_ANALOG_TEST_BUS_SELECT              0x50
#define Si4432_DIGITAL_TEST_BUS_SELECT             0x51
#define Si4432_TX_RAMP_CONTROL                     0x52
#define Si4432_PLL_TUNE_TIME                       0x53
#define Si4432_CALIBRATION_CONTROL                 0x55
#define Si4432_MODEM_TEST                          0x56
#define Si4432_CHARGE_PUMP_TEST                    0x57
#define Si4432_CHARGE_PUMP_CURRENT_TRIMMING        0x58
#define Si4432_DIVIDER_CURRENT_TRIMMING            0x59
#define Si4432_VCO_CURRENT_TRIMMING                0x5a
#define Si4432_VCO_CALIBRATION                     0x5b
#define Si4432_SYNTHESIZER_TEST                    0x5c
#define Si4432_BLOCK_ENABLE_OVERRIDE1              0x5d
#define Si4432_BLOCK_ENABLE_OVERRIDE2              0x5e
#define Si4432_BLOCK_ENABLE_OVERRIDE3              0x5f
#define Si4432_CHANNEL_FILTER_COEFFICIENT_ADDRESS  0x60
#define Si4432_CHANNEL_FILTER_COEFFICIENT_VALUE    0x61

/* power-on reset  */
#define Si4432_CRYSTAL_OSCILLATOR_POR_CONTROL      0x62
#define internal_power_state_mask (BIT7|BIT6|BIT5)
#define internal_power_state_lp    0x00
#define internal_power_state_ready 0x20
#define internal_power_state_tune  0x60
#define internal_power_state_tx    0x40
#define internal_power_state_rx    0xE0
#define clkhyst  BIT4           /* clock hysteresis setting */
#define enbias2x BIT3           /* double bias current enable */
#define enamp2x  BIT2           /* double amplification enable */
#define bufovr   BIT1           /* output buffer enable override */
#define enbuf    BIT0           /* output buffer enable */

#define Si4432_RC_OSCILLATOR_COARSE_CALIBRATION    0x63
#define Si4432_RC_OSCILLATOR_FINE_CALIBRATION      0x64
#define Si4432_LDO_CONTROL_OVERRIDE                0x65
#define Si4432_LDO_LEVEL_SETTINGS                  0x66
#define Si4432_DELTA_SIGMA_ADC_TUNING1             0x67
#define Si4432_DELTA_SIGMA_ADC_TUNING2             0x68
#define Si4432_AGC_OVERRIDE1                       0x69
#define Si4432_AGC_OVERRIDE2                       0x6a
#define Si4432_GFSK_FIR_FILTER_COEFFICIENT_ADDRESS 0x6b
#define Si4432_GFSK_FIR_FILTER_COEFFICIENT_VALUE   0x6c

/* transmit power */
#define Si4432_TX_POWER                            0x6d
#define lna_sw      BIT3        /* lna switch controller */
#define txpow_mask (BIT2|BIT1|BIT0)
#define txpow_max   0x07        /* maximum transmit power */
#define txpow_17dbm 0x06
#define txpow_14dbm 0x05
#define txpow_11dbm 0x04
#define txpow_08dbm 0x03
#define txpow_05dbm 0x02
#define txpow_02dbm 0x01
#define txpow_min   0x00        /* minimum transmit power */

/* transmission data rate */
#define Si4432_TX_DATA_RATE1                       0x6e
#define Si4432_TX_DATA_RATE0                       0x6f
/* 8590 decimal, 0x218E in these registers gives 4096.03 kbps */

/* modulation */
#define Si4432_MODULATION_CONTROL1                 0x70
#define txdtrtscale BIT5        /* set for data rate < 30kbps */
#define enphpwdn    BIT4        /* power down packet handler */
#define manppol     BIT3        /* manchester preamble polarity */
#define enmaninv    BIT2        /* manchester data inversion */
#define enmanch     BIT1        /* manchester coding enable */
#define enwhite     BIT0        /* data whitening enabled */

/* more modulation */
#define Si4432_MODULATION_CONTROL2                 0x71
#define tx_data_clock_config_mask (BIT7|BIT6) /* clock source */
#define tx_data_clock_none 0x00 /* no tx data clock available */
#define tx_data_clock_gpio 0x40 /* tx data clock into GPIO pin */
#define tx_data_clock_sdo  0x80 /* tx data clock into SDO pin */
#define tx_data_clock_nirq 0xC0 /* tx data clock into nIRQ pin */
#define dtmod_mask (BIT5|BIT4)  /* modulation data source */
#define dtmod_direct_gpio  0x00 /* tx data into GPIO pin */
#define dtmod_direct_sdi   0x10 /* tx data into SDI pin */
#define dtmod_fifo         0x20 /* FIFO mode */
#define dtmod_pn9          0x30 /* PN9 m-sequence (x^9 + x^4 + 1) */
#define eninv BIT3              /* invert tx and rx data */
#define freq_deviation_msb BIT2 /* MSB of frequency deviation */
#define modtyp_mask (BIT1|BIT0) /* modulation type */
#define modtyp_unmodulated 0    /* unmodulated carrier */
#define modtyp_ook         1    /* on-off keyed */
#define modtyp_fsk         2    /* frequency-shift keyed */
#define modtyp_gfsk        3    /* gaussian fsk */

/* frequency deviation */
#define Si4432_FREQUENCY_DEVIATION                 0x72

/* frequency offset */
#define Si4432_FREQUENCY_OFFSET1                   0x73
#define Si4432_FREQUENCY_OFFSET2                   0x74

/* frequency band select */
#define Si4432_FREQUENCY_BAND                      0x75
#define sbsel BIT6              /* side band select */
#define hbsel BIT5              /* high band select */
#define fb_mask 0x1F            /* frequency band select */
/* Choose sbsel = 1, hbsel = 0, fb = 19, 0x13 for frequency band 430
   MHz.  Then value is (sbsel|0x13) */

/* carrier frequency */
#define Si4432_NOMINAL_CARRIER_FREQUENCY1          0x76
#define Si4432_NOMINAL_CARRIER_FREQUENCY0          0x77
/* Spreadsheet gives fc = 0x76C0 = 30400 => carrier = 434.75 MHz*/

/* frequency hopping */
#define Si4432_FREQUENCY_HOPPING_CHANNEL           0x79
#define Si4432_FREQUENCY_HOPPING_STEP_SIZE         0x7a

/* tx FIFO  */
#define Si4432_TX_FIFO_CONTROL1                    0x7c
#define txafthr_mask 0x1F       /* tx FIFO almost full threshold */
#define Si4432_TX_FIFO_CONTROL2                    0x7d
#define txaethr_mask 0x1F       /* tx FIFO almost empty threshold */

/* rx FIFO */
#define Si4432_RX_FIFO_CONTROL                     0x7e
#define txafthr_mask 0x1F       /* tx FIFO almost full threshold */
#define Si4432_FIFO_ACCESS                         0x7f


typedef enum {
  SHUTDOWN,
  IDLE,
  STANDBY,
  SLEEP,
  SENSOR,
  READY,
  TUNE,
  XMIT_DIRECT,
  XMIT_PACKET,
  RECV_DIRECT,
  RECV_PACKET
} radiostate;


class Si4432 {

public:
  Si4432();

  uint8_t spi_read_register(uint8_t addr);
  uint8_t *spi_read_burst(uint8_t addr, uint8_t *data, uint8_t len);
  void spi_write_register(uint8_t addr, uint8_t data);
  void spi_write_burst(uint8_t addr, uint8_t *data, uint8_t len);
  
  char* state();
  uint8_t status();
  bool reset();
  bool check_device();
  bool xmit_tone(uint16_t duration);
  bool xmit_packet(uint8_t *data, uint8_t len);
  uint8_t *recv_packet();

  //protected:
  void configure_gpio();
  void set_frequency();
  void init_tx_direct();
  void init_tx_packet();
  void init_rx_packet();
  void packet_config();
  void load_packet(uint8_t *data, uint8_t len);
  radiostate get_state();
  radiostate get_power_state();
  void set_state(radiostate state);
  char* show_state(radiostate state);

  bool _direct; /* flag, true -> direct mode, false -> packet mode */
};


#endif
