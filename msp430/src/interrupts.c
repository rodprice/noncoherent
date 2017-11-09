/* Interrupt routines gathered here for easy reference. */

#include "interrupts.h"

#include <msp430.h>

#include "util.h"
#include "beacon.h"
#include "msequence.h"
#include "morse.h"
#include "si4432.h"


volatile uint32_t clock;   /* real-time clock */
volatile uint16_t galois;  /* m-sequence shift register */
volatile uint16_t mticker; /* counts periods of m-sequences */
volatile uint8_t  aticker; /* counts audio tone half-periods */


/* M-sequence generation */
__attribute__((interrupt(TIMER0_A0_VECTOR))) void mseq_isr(void) {
  uint8_t bit;
  TACCR0 += MSEQ_TICKS;          /* set the next timer period */
  bit = galois & REGLOAD;        /* find bit to send */
  sendbit_port2( bit, GPIO1_PIN ); /* send mseq bit */
  galois = galshift(galois);     /* generate the next bit */
  if ((galois & SEQLEN) == REGLOAD) { /* at beginning of mseq? */
    mticker++;                   /* increment period count */
    if (mticker >= PERIODS) {    /* sent all periods of mseq? */
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

/* Interrupts from the radio */
__attribute__((interrupt(PORT2_VECTOR))) void si4432_isr(void) {
  volatile uint8_t iflags1, iflags2;

  /* nIRQ interrupt handler */
  if (P2IFG & NIRQ_PIN) {
    P2IFG &= ~NIRQ_PIN;         /* clear the interrupt flag */
    /* reading both status registers releases nIRQ */
    iflags1 = spi_read_register(Si4432_INTERRUPT_STATUS1);
    iflags2 = spi_read_register(Si4432_INTERRUPT_STATUS2);
    if (iflags2 & ipor) {       /* Si4432 power-on reset complete */
      LPM3_EXIT;                /* wake up processor on exit */
      return;
    }
  }

  /* Transmit clock interrupt handler */
  if (P2IFG & XMIT_CLOCK_PIN) { /* just sends a single FM tone */
    P2IFG &= ~XMIT_CLOCK_PIN;   /* clear the interrupt flag */
    if (--aticker == 0) {       /* time to shift frequency? */
      aticker = AUDIO_TICKS;    /* reset counter */
      P2OUT ^= XMIT_DATA_PIN;   /* toggle transmit data output */
    }
  }
}

/* Define empty ISRs */
__attribute__((interrupt(TRAPINT_VECTOR)))     void trapint_isr (void) {;}
__attribute__((interrupt(PORT1_VECTOR)))       void port1_isr   (void) {;}
__attribute__((interrupt(ADC10_VECTOR)))       void adc10_isr   (void) {;}
__attribute__((interrupt(USCIAB0TX_VECTOR)))   void usci_tx_isr (void) {;}
__attribute__((interrupt(USCIAB0RX_VECTOR)))   void usci_rx_isr (void) {;}
__attribute__((interrupt(WDT_VECTOR)))         void wdt_isr     (void) {;}
__attribute__((interrupt(COMPARATORA_VECTOR))) void compa_isr   (void) {;}
