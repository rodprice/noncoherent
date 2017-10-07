/**
 * @file: main.c
 * @author: Rodney Price
 * @brief Driver for the cheap beacon with PN sequence and Morse code
 */

#include <msp430.h>
#include <stdint.h>

#include "config.h"
#include "morse.h"
#include "msequence.h"


static volatile uint32_t clock;  /* real-time clock */
static volatile uint16_t galois; /* m-sequence shift register */

static uint8_t buffer[RINGSIZE]; /* move data between main loop and */
static ringbuffer ring;          /* interrupt routines, lock-free */


/* Start the timer, reset and start the clock */
void timer_start() {
  TACCR0 = 0;                    /* stop the timer */
  TACTL |= TACLR;                /* set timer count to 0 */
  clock = 0;                     /* reset the clock */
  /* do something with TAIE to enable interrupts */
  TACTL = TASSEL0 | ID0 | MC_2 | TACLR; /* timer source ACLK, continuous mode */
}

/* Stop the timer and pause the clock */
void timer_stop() {
  TACCR0 = 0;                    /* stop the timer */
  TACTL |= TACLR;                /* set TAR to 0 */
}

/* Start sending m-sequences */
inline void mseq_start() {
  TACCR0 = MSEQ_TICKS;           /* m-sequence clock rate */
  TACCTL0 = CCIE;                /* compare mode, interrupt enabled */
}

/* Stop sending m-sequences */
inline void mseq_stop() {
  TACCTL0 = 0;                   /* stop mseq_isr() interrupts */
}

/* Start sending Morse code */
inline void morse_start() {
  TACCR1 = MORSE_TICKS;          /* Morse code clock rate */
  TACCTL1 = CCIE;                /* compare mode, interrupt enabled */
}

/* Stop sending Morse code */
inline void morse_stop() {
  TACCTL0 = 0;                   /* stop morse_isr() interrupts */
}

/* M-sequence generation */
__attribute__((interrupt(TIMER0_A0_VECTOR))) void mseq_isr(void) {
  TACCR0 += MSEQ_TICKS;          /* set the next timer period */
  if (galois & REGLOAD)          /* send the current bit */
    P1OUT |= (MSEQ_PIN | XMIT_PIN);
  else
    P1OUT &= ~(MSEQ_PIN | XMIT_PIN);
  galois = galshift(galois);     /* generate the next bit */
}

/* Morse code generation and clock */
__attribute__((interrupt(TIMER0_A1_VECTOR))) void morse_isr(void) {
  switch(__even_in_range(TAIV,TAIV_TAIFG))
    {
    case 0:                     /* no interrupt pending */
      return;
    case TA0IV_TACCR1:          /* Morse interrupt pending */
      TACCR1 += MORSE_TICKS;    /* set the next timer period */
      if (tock(&ring))          /* send out the next Morse code bit */
        P1OUT |= (MORSE_PIN | XMIT_PIN);
      else
        P1OUT &= ~(MORSE_PIN | XMIT_PIN);
      return;
    case TA0IV_TAIFG:           /* timer overflow interrupt */
      clock++;                  /* Add 2 seconds to clock */
      return;
    }
}

int main(int argc, char *argv[])
{
  /* Hold the watchdog */
  WDTCTL = WDTPW + WDTHOLD;
  
  /* Configure the clocks */
  DCOCTL = 0;
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;
  BCSCTL3 |= LFXT1S_0;        /* ACLK from 32768 Hz watch crystal */
  
  /* Set all digital i/o outputs low */
  P1OUT = 0x00;
  P2OUT = 0x00;
  
  /* Set all digital i/o pins to output */
  P1DIR = 0xFF;
  P2DIR = 0xFF;

  /* Connect P1.1 to timer, P1.5 to ACLK */
  P1SEL = BIT1 | BIT5;          /* DEBUG */
  
  /* Initialize data structures */
  clock = 0;
  ring = rbnew(buffer, sizeof(buffer));
  inittock(&ring);             /* set up Morse code generator */
  galois = REGLOAD;            /* set up m-sequence generator */
  
  __nop();                     /* assembler says we might need this */
  __enable_interrupt();
  timer_start();

  /* Send the message repeatedly */
  while (1)
    {
      __delay_cycles(1000000);
      inittock();
    }
}

