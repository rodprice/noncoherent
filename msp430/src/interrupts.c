/* Interrupt routines gathered here for easy reference. */

#include "interrupts.h"

#include <msp430.h>

#include "util.h"
#include "beacon.h"
#include "msequence.h"
#include "morse.h"
#include "si4432.h"


volatile uint32_t clock;        /* real-time clock */
volatile uint16_t galois;       /* m-sequence shift register */
volatile uint16_t mticker;      /* counts periods of m-sequences */
volatile uint8_t  aticker;      /* counts audio tone half-periods */
volatile key thiskey;           /* audio/transmitter state */
volatile key lastkey;           /* lagging audio/transmitter state */

/* M-sequence generation */
__attribute__((interrupt(TIMER0_A0_VECTOR))) void mseq_isr(void) {
  uint8_t bit;
  TACCR0 += MSEQ_TICKS;         /* set the next timer period */
  bit = galois & REGLOAD;       /* find bit to send */
  sendbit_port2( bit, GPIO1_PIN ); /* send mseq bit */
  galois = galshift(galois);    /* generate the next bit */
  if ((galois & SEQLEN) == REGLOAD) { /* at beginning of mseq? */
    mticker++;                  /* increment period count */
    if (mticker >= PERIODS) {   /* sent all periods of mseq? */
      mseq_stop();              /* stop sending */
      LPM3_EXIT;                /* wake up processor */
    }
  }
}

/* The Si4432 transmitter seems to need a cooling off period after being told
   to go to READY mode.  That is, you tell it to go to READY mode while in
   direct transmission mode, and maybe it will and maybe it won't.  The Si4432
   documentation says that the power amp "ramps down" over a period of 5-20
   usecs.  The following code tells the radio to go to READY mode when tock()
   returns DOWN, but it leaves the Morse interrupt on.  When the next Morse
   interrupt occurs, it tells the radio to go to READY mode again, but this
   time it turns off the Morse interrupts.  This presumably gives the radio its
   cooling off period without requiring a busy-wait.  In any event, it seems to
   work.  Kind of like working with a toddler... */

/* Morse code generation and clock */
__attribute__((interrupt(TIMER0_A1_VECTOR))) void morse_isr(void) {
  switch(__even_in_range(TAIV,TAIV_TAIFG))
    {
    case 0:                     /* no interrupt pending */
      return;

    case TA0IV_TACCR1:          /* Morse interrupt pending */
      TACCR1 += MORSE_TICKS;    /* set up next interrupt */
      thiskey = tock();         /* get next key of Morse code */
      
      switch (thiskey) {        /* debug */
      case ON:
        P1OUT |= RXD_PIN;       /* ON */
        P1OUT |= TXD_PIN;       /* UP */
        break;
      case OFF:
        P1OUT &= ~RXD_PIN;      /* OFF */
        P1OUT |= TXD_PIN;       /* UP */
        break;
      case DOWN:
        P1OUT ^= RXD_PIN;       /* toggle to see interrupts */
        P1OUT &= ~TXD_PIN;      /* DOWN */
        break;
      }                         /* end debug */
      
      if (thiskey == DOWN && lastkey == DOWN) {
        xmit_morse_stop();      /* stop transmitting, stop interrupts */
        return;
      }
      if (thiskey == DOWN) {
        si4432_set_state(READY); /* stop transmitting */
      }
      lastkey = thiskey;
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
    /* first interrupt flag found wins */
    if (iflags2 & ipor) {       /* Si4432 power-on reset complete */
      return;
    }
    if (iflags2 & ichiprdy) {   /* Si4432 chip ready (xtal on) */
      return;
    }
    if (iflags1 & ipksent) {    /* packet transmission complete */
      xmit_packet_stop();       /* turn off transmitter */
      return;
    }
  }

  /* Transmit clock interrupt handler */
  if (P2IFG & XMIT_CLOCK_PIN) { /* beeps, or not -- single FM tone */
    P2IFG &= ~XMIT_CLOCK_PIN;   /* clear the interrupt flag */
    if (thiskey == ON) {        /* determined elsewhere */
      if (--aticker == 0) {     /* time to shift GFSK frequency? */
        aticker = AUDIO_TICKS;  /* reset counter */
        P2OUT ^= XMIT_DATA_PIN; /* toggle transmit data output */
      }
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
