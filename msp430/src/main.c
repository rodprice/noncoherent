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


/* Set or clear P1OUT bits in mask */
#define SENDBIT_P1OUT_DEFAULT(bit,mask) \
  if (bit) {                            \
    (P1OUT) |= (mask);                  \
  } else {                              \
    (P1OUT) &= ~(mask);                 \
  }

/* Swap in debugging tools as desired */
#ifdef DEBUG_MORSE
#define SENDBIT_P1OUT(bit,mask) do { P1OUT = debug_morse_sendbit((bit),(mask)); } while(0)
#elif defined DEBUG_MSEQUENCE
#define SENDBIT_P1OUT(bit,mask) SENDBIT_P1OUT_DEFAULT((bit),(mask))
#elif defined DEBUG_RINGBUFFER
#define SENDBIT_P1OUT(bit,mask) SENDBIT_P1OUT_DEFAULT((bit),(mask))
#else
#define SENDBIT_P1OUT(bit,mask) SENDBIT_P1OUT_DEFAULT((bit),(mask))
#endif


static volatile uint32_t clock;  /* real-time clock */
static volatile uint16_t galois; /* m-sequence shift register */
static volatile uint16_t ticker; /* counts periods of m-sequences */


/* Start the timer, reset and start the clock */
inline void timer_start() {
  TACCR0 = 0;                    /* stop the timer */
  TACTL |= TACLR;                /* set timer count to 0 */
  clock = 0;                     /* reset the clock */
  /* do something with TAIE to enable interrupts */
  TACTL = TASSEL0 | ID0 | MC_2 | TACLR; /* timer source ACLK, continuous mode */
}

/* Stop the timer and pause the clock */
inline void timer_stop() {
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
  P1OUT &= ~(MSEQ_PINS | TICKER_PIN); /* turn out the lights */
}

/* Start sending Morse code */
inline void morse_start() {
  TACCR1 = MORSE_TICKS;          /* Morse code clock rate */
  TACCTL1 = CCIE;                /* compare mode, interrupt enabled */
}

/* Stop sending Morse code */
inline void morse_stop() {
  TACCTL1 = 0;                   /* stop morse_isr() interrupts */
  P1OUT &= ~(MORSE_PINS);        /* turn out the lights */
#ifdef DEBUG_MORSE
  P1OUT &= ~(DEBUG_MORSE_MASK);  /* don't forget any */
#endif
}

/* M-sequence generation */
__attribute__((interrupt(TIMER0_A0_VECTOR))) void mseq_isr(void) {
  TACCR0 += MSEQ_TICKS;          /* set the next timer period */
  SENDBIT_P1OUT_DEFAULT( galois & REGLOAD, MSEQ_PINS );
  if ((galois & SEQLEN) == REGLOAD) {
    ticker++;
    SENDBIT_P1OUT_DEFAULT( 1, TICKER_PIN );
  } else {
    SENDBIT_P1OUT_DEFAULT( 0, TICKER_PIN );
  }
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
      SENDBIT_P1OUT( tock(), MORSE_PINS );
      return;
    case TA0IV_TAIFG:           /* timer overflow interrupt */
      clock++;                  /* add 2 seconds to clock */
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
  /* P1SEL = BIT1 | BIT5;          /\* DEBUG *\/ */
  
  /* Initialize data structures */
  clock = 0;
  inittock();                  /* set up Morse code generator */
  galois = REGLOAD;            /* set up m-sequence generator */
  
  __nop();
  __enable_interrupt();
  __nop();
  
  timer_start();
  
  /* Send the message repeatedly */
  while (1) {
    /* Send my call sign and a short message */
    inittock();
    morse_start();
    while (!donemsg()) { __delay_cycles(1000); }
    morse_stop();
    
    /* Take a breath */
    __delay_cycles(50000);
    
    /* Send m-sequences repeatedly */
    ticker = 0;
    galois = REGLOAD;
    mseq_start();
    while (ticker < PERIODS) { __delay_cycles(20); }
    mseq_stop();
    
    /* Take a breath */
    __delay_cycles(500000);
  }
}

