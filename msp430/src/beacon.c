
#include <msp430.h>
#include <stdint.h>
#include <stddef.h>
#include "beacon.h"
#include "spi.h"
#include "si4432.h"
#include "util.h"
#include "morse.h"


extern volatile uint32_t clock;  /* increments every two seconds */
extern volatile uint8_t aticker; /* counts audio tone half-periods */
extern volatile key thiskey;     /* audio/transmitter state */


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
  WDTCTL = WDTPW + WDTHOLD;     /* hold the watchdog */
  DCOCTL = 0;                   /* set DCO clock to lowest setting */
  BCSCTL1 = CALBC1_8MHZ;        /* set MCLK to 8 MHz from cal data */
  DCOCTL = CALDCO_8MHZ;         /* set DCOCLK to 8 MHz from cal data */
  BCSCTL2 = DIVS_3;             /* set SMCLK to 1 MHZ (divide by 8) */
  BCSCTL3 |= LFXT1S_0 | XCAP_3; /* ACLK from 32768 Hz crystal, 12.5 pF */
  __delay_cycles(500000);       /* wait for 32768 Hz oscillator to start */
}

/* Set up MSP430 I/O pins */
void configure_pins() {
  /* Port 1 pins */
  P1OUT =         (
    TXON_PIN      |             /* high turns transmitter off */
    RXON_PIN      );            /* high turns receiver off */
  P1DIR =         (
    XMIT_LED_PIN  |             /* transmit light off */
    TXON_PIN      |             /* transmit command off */
    RXON_PIN      |             /* receive command off */
    SCK_PIN       |             /* SPI SCLK pin set low */
    SDI_PIN       );            /* SPI SIMO pin set low */
  /* Port 2 pins */
  P2OUT =         (
    NSEL_PIN      );            /* nSEL is active low */
  P2DIR =         (
    XMIT_DATA_PIN |
    NSEL_PIN      |
    READY_LED_PIN );
  /* Set up SPI pins */
  P1SEL  |= ( SCK_PIN | SDO_PIN | SDI_PIN );
  P1SEL2 |= ( SCK_PIN | SDO_PIN | SDI_PIN );
  /* Set up UART pins */
  /* P1SEL  |= ( RXD_PIN | TXD_PIN ); */
  /* P1SEL2 |= ( RXD_PIN | TXD_PIN ); */
  P1OUT &= ~RXD_PIN;            /* debug */
  P1OUT &= ~TXD_PIN;            /* debug */
  P1DIR |= RXD_PIN;             /* debug */
  P1DIR |= TXD_PIN;             /* debug */
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
  /* P2DIR |= XMIT_DATA_PIN;       /\* pin drives Si4432 tx data *\/ */
  /* P2DIR &= ~RECV_DATA_PIN;      /\* pin accepts Si4432 rx data *\/ */
  /* P2DIR &= ~XMIT_CLOCK_PIN;     /\* pin accepts Si4423 tx clock *\/ */
  P2IES |= XMIT_CLOCK_PIN;      /* high-to-low requests interrupt */
  P2IFG &= ~XMIT_CLOCK_PIN;     /* clear flag, just in case */
  P2IE  |= XMIT_CLOCK_PIN;      /* enable interrupt on pin */  
}

void xmit_tone_start() {
  aticker = AUDIO_TICKS;        /* start the audio clock */
  P2IFG &= ~XMIT_CLOCK_PIN;     /* clear flag, just in case */
  P2IE  |= XMIT_CLOCK_PIN;      /* enable audio interrupt */
  si4432_init_tx_direct();      /* start sending xmit clock */
  si4432_set_state(XMIT_DIRECT); /* start transmitting */
  thiskey = ON;
}

void xmit_tone_stop() {
  si4432_set_state(READY);      /* stop transmitting */
  P2IE  &= ~XMIT_CLOCK_PIN;     /* disable interrupt on pin */  
  P2IFG &= ~XMIT_CLOCK_PIN;     /* clear flag, just in case */
}

/* Starts sending Morse code, assuming that the timer is running */
void xmit_morse_start() {
  aticker = AUDIO_TICKS;        /* start the audio clock */
  P2IFG &= ~XMIT_CLOCK_PIN;     /* clear flag, just in case */
  si4432_init_tx_direct();      /* start sending xmit clock */
  thiskey = init_tock();        /* point tock() at first letter */
  si4432_set_state(XMIT_DIRECT); /* start transmitting */
  TACCR1 = MORSE_TICKS;         /* set time until next interrupt */
  TACCTL1 = CCIE;               /* enable Morse interrupts */
  P2IE |= XMIT_CLOCK_PIN;       /* enable audio interrupt */
}

/* Stop sending Morse code, but don't stop the timer */
void xmit_morse_stop() {
  si4432_set_state(READY);      /* stop transmitting */
  P2IE  &= ~XMIT_CLOCK_PIN;     /* disable interrupt on pin */  
  P2IFG &= ~XMIT_CLOCK_PIN;     /* clear flag, just in case */
  TACCTL1 = 0;                  /* stop morse_isr() interrupts */
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
  /* timer source ACLK, continuous mode */
  TACTL = TASSEL0 | ID0 | MC_2; 
}

/* Stop the timer and pause the clock */
void timer_stop() {
  TACCR0 = 0;                    /* stop the timer */
  TACTL |= TACLR;                /* set TAR to 0 */
  TACTL &= ~MC_0;                /* set halted mode */
}



int main(int argc, char *argv[])
{
  int i;
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

  /* si4432_load_packet("hi there again", 14); */
  /* xmit_packet_start(); */
  /* __delay_cycles(800000); */
  /* xmit_packet_stop(); */

  /* xmit_tone_start(); */
  /* __delay_cycles(4000000);      /\* half-second *\/ */
  /* xmit_tone_stop(); */

  timer_start();
  xmit_morse_start();
  while (thiskey != DOWN);
  __delay_cycles(16000000);
  P2OUT |= NSEL_PIN;
  xmit_morse_stop();
  timer_stop();
  si4432_set_state(READY);

  P2OUT |= READY_LED_PIN;
  while (1);
  return 0;
}
