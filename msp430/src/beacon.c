
#include <msp430.h>
#include <stdint.h>
#include <stddef.h>
#include "beacon.h"
#include "spi.h"
#include "si4432.h"
#include "util.h"


extern volatile uint32_t clock;  /* increments every two seconds */
extern volatile uint8_t aticker; /* counts audio tone half-periods */


/* Read calibration data from TLV, verify checksum */
/* http://www.simplyembedded.org/tutorials/msp430-configuration/ */
static int _verify_cal_data(void)
{
    uint8_t len = 31;
    uint16_t *data = (uint16_t *) 0x10c2;
    uint16_t crc = 0;
 
    while (len-- > 0) {
        crc ^= *(data++);
    }
 
    return (TLV_CHECKSUM + crc);
}


/* Set up MSP430 clocks, watchdog */
void configure_clocks() {
  WDTCTL = WDTPW + WDTHOLD;    /* hold the watchdog */
  DCOCTL = 0;                  /* set DCO clock to lowest setting */
  /* BCSCTL1 = CALBC1_1MHZ;       /\* set MCLK to 1 MHz from cal data *\/ */
  /* DCOCTL = CALDCO_1MHZ;        /\* set DCOCLK to 1 MHz from cal data *\/ */
  BCSCTL1 = CALBC1_8MHZ;       /* set MCLK to 8 MHz from cal data */
  DCOCTL = CALDCO_8MHZ;        /* set DCOCLK to 8 MHz from cal data */
  BCSCTL2 = DIVS_3;            /* set SMCLK to 1 MHZ (divide by 8) */
  BCSCTL3 |= LFXT1S_0 | XCAP_3; /* ACLK from 32768 Hz crystal, 12.5 pF */
  __delay_cycles(500000);      /* wait for 32768 Hz oscillator to start */
}

/* Set up MSP430 I/O pins */
void configure_pins() {
  /* Port 1 pins */
  P1REN = BUTTON_PIN;          /* set pullup resistor on button */
  P1OUT =         (
    BUTTON_PIN    |            /* pull up, not down */
    TXON_PIN      |            /* high turns transmitter off */
    RXON_PIN      );           /* high turns receiver off */
  P1DIR =         (            /* BUTTON_PIN and SDO_PIN are inputs */
    XMIT_LED_PIN  |            /* transmit light off */
    READY_LED_PIN |            /* power/ready light off */
    TXON_PIN      |            /* transmit command off */
    RXON_PIN      |            /* receive command off */
    SCK_PIN       |            /* SPI SCLK pin set low */
    SDI_PIN       );           /* SPI SIMO pin set low */
  /* Port 2 pins */
  P2OUT = NSEL_PIN;            /* nSEL high, SDN low */
  P2DIR = NSEL_PIN | SDN_PIN;  /* all other pins are inputs */
}

/* Enable interrupts on the nIRQ pin */
void enable_nirq() {
  P2DIR &= ~NIRQ_PIN;           /* set direction to input */
  P2IES |= NIRQ_PIN;            /* high-to-low requests interrupt */
  P2IFG &= ~NIRQ_PIN;           /* clear flag, just in case */
  P2IE  |= NIRQ_PIN;            /* enable interrupt on pin */
}

void disable_nirq() {
  P2IFG &= ~NIRQ_PIN;           /* clear flag, just in case */
  P2IE  &= ~NIRQ_PIN;           /* disable interrupt on pin */
  P2IFG &= ~NIRQ_PIN;           /* clear flag, just in case */
}

/* Enable interrupts from the radio for direct mode transmission */
void enable_clock_irq() {
  P2DIR |= XMIT_DATA_PIN;       /* pin drives Si4432 tx data */
  P2DIR &= ~RECV_DATA_PIN;      /* pin accepts Si4432 rx data */
  P2DIR &= ~XMIT_CLOCK_PIN;     /* pin accepts Si4423 tx clock */
  P2IES |= XMIT_CLOCK_PIN;      /* high-to-low requests interrupt */
  P2IFG &= ~XMIT_CLOCK_PIN;     /* clear flag, just in case */
  P2IE  |= XMIT_CLOCK_PIN;      /* enable interrupt on pin */  
}

void xmit_tone_start() {
  aticker = AUDIO_TICKS;        /* start the audio clock */
  P2IFG &= ~XMIT_CLOCK_PIN;     /* clear flag, just in case */
  P2IE  |= XMIT_CLOCK_PIN;      /* enable interrupt on pin */
  si4432_set_state(XMIT_TONE);  /* start transmitting */
}

void xmit_tone_stop() {
  si4432_set_state(READY);      /* stop transmitting */
  P2IE  &= ~XMIT_CLOCK_PIN;     /* disable interrupt on pin */  
  P2IFG &= ~XMIT_CLOCK_PIN;     /* clear flag, just in case */
}

void xmit_packet_start() {
  P2IFG &= ~NIRQ_PIN;           /* clear flag, just in case */
  P2IE  |= NIRQ_PIN;            /* enable interrupt on pin */
  si4432_set_state(XMIT_PACKET); /* start transmitting */
}

void xmit_packet_stop() {
  si4432_set_state(READY);      /* stop transmitting */
  P2IE  &= ~NIRQ_PIN;           /* disable interrupt on pin */  
  P2IFG &= ~NIRQ_PIN;           /* clear flag, just in case */
}

/* Start the timer, reset and start the clock */
void timer_start() {
  TACCR0 = 0;                   /* stop the timer */
  TACTL |= TACLR;               /* set timer count to 0 */
  clock = 0;                    /* reset the clock */
  /* do something with TAIE to enable interrupts */
  TACTL = TASSEL0 | ID0 | MC_2 | TACLR; /* timer source ACLK, continuous mode */
}

/* Stop the timer and pause the clock */
void timer_stop() {
  TACCR0 = 0;                    /* stop the timer */
  TACTL |= TACLR;                /* set TAR to 0 */
}



int main(int argc, char *argv[])
{
  configure_clocks();
  configure_pins();
  enable_spi();

  si4432_reset();
  si4432_check_device();
  si4432_configure_gpio();
  si4432_set_frequency();
  si4432_packet_config();

  enable_nirq();
  enable_clock_irq();

  __nop();
  __enable_interrupt();

  si4432_load_packet("hi there again", 14);
  xmit_packet_start();
  __delay_cycles(800000);
  xmit_packet_stop();

  xmit_tone_start();
  __delay_cycles(4000000);      /* half-second */
  xmit_tone_stop();
  
  while (1) {
    LPM3;                       /* sleep */
  }
}
