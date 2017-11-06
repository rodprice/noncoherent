/* Interrupt routines gathered here for easy reference. */

#include "interrupts.h"

#include <msp430.h>

#include "util.h"
#include "beacon.h"
#include "msequence.h"
#include "morse.h"
#include "si4432.h"


volatile uint32_t clock;  /* real-time clock */
volatile uint16_t galois; /* m-sequence shift register */
volatile uint16_t ticker; /* counts periods of m-sequences */


/* M-sequence generation */
__attribute__((interrupt(TIMER0_A0_VECTOR))) void mseq_isr(void) {
  uint8_t bit;
  TACCR0 += MSEQ_TICKS;          /* set the next timer period */
  bit = galois & REGLOAD;        /* find bit to send */
  sendbit_port2( bit, GPIO1_PIN ); /* send mseq bit */
  galois = galshift(galois);     /* generate the next bit */
  if ((galois & SEQLEN) == REGLOAD) { /* at beginning of mseq? */
    ticker++;                    /* increment period count */
    if (ticker >= PERIODS) {     /* sent all periods of mseq? */
      mseq_stop();               /* stop sending */
      LPM3_EXIT;                 /* wake up processor */
    }
  }
}

/* Morse code generation and clock */
__attribute__((interrupt(TIMER0_A1_VECTOR))) void morse_isr(void) {
  switch(__even_in_range(TAIV,TAIV_TAIFG))
    {
    case 0:                     /* no interrupt pending */
      return;
    case TA0IV_TACCR1:          /* Morse interrupt pending */
      TACCR1 += MORSE_TICKS;    /* set the next timer period */
      sendbit_port1( tock(), GPIO1_PIN );
      return;
    case TA0IV_TAIFG:           /* timer overflow interrupt */
      clock++;                  /* add 2 seconds to clock */
      return;
    }
}

/* nIRQ interrupts */
__attribute__((interrupt(PORT2_VECTOR))) void si4432_isr(void) {
  volatile uint8_t iflags1, iflags2;
  if (P2IFG & NIRQ_PIN) {       /* is this the nIRQ interrupt? */
    P2IFG &= ~NIRQ_PIN;         /* clear the interrupt flag */
    /* reading both status registers releases nIRQ */
    iflags1 = spi_read_register(Si4432_INTERRUPT_STATUS1);
    iflags2 = spi_read_register(Si4432_INTERRUPT_STATUS2);
    if (iflags2 & ipor) {       /* Si4432 power-on reset complete */
      LPM3_EXIT;                /* wake up processor on exit */
      return;
    }
  }
}
