/**
 * @file: main.c
 * @author: Rodney Price
 * @brief Driver for the cheap beacon with PN sequence
 */

#include <msp430.h>
#include <stdint.h>

#include "morse.h"
#include "config.h"

/* create variable __sr on the stack */
#define SR_ALLOC() uint16_t __sr
/* save the status register and disable interrupts */
#define ENTER_CRITICAL() __sr = _get_interrupt_state(); __disable_interrupt()
/* restore the saved value of the status register */
#define EXIT_CRITICAL() __set_interrupt_state(__sr)


static uint16_t ticks;          /* morse code counter */
static volatile uint32_t clock; /* real-time clock */


int timer_start(void) {
  TACCR0 = 0;               /* stop the timer */
  TACTL |= TACLR;           /* set timer count to 0 */
  /* Set up channel 0 interrupts (m-sequences) */
  TACCR0 = MSEQ_TICKS;      /* m-sequence clock */
  TACCTL0 = CCIE;           /* compare mode, interrupt enabled */
  /* Set up channel 1 interrupts (Morse code) */
  TACCR1 = MORSE_TICKS;     /* Morse code clock */
  TACCTL1 = CCIE;
  TACTL = TASSEL0 | ID0 | MC_2 | TACLR; /* clock source ACLK, continuous mode */
  return 0;
}

/* int timer_start(void) { */
/*   P1OUT = 0;                /\* DEBUG *\/ */
/*   TACTL = TASSEL0 | ID0 | MC0 | TACLR; /\* clock source ACLK, divide by 1, timer off *\/ */
/*   TACCR0 = MSEQ_TICKS;      /\* interrupt frequency in units of ACLK *\/ */
/*   TACCTL0 = CCIE;           /\* compare mode, interrupt enabled *\/ */
/*   TACTL |= MC1;             /\* timer in up mode and we're off... *\/ */
/*   return 0; */
/* } */

int timer_stop(void) {
  TACCR0 = 0;                   /* stop the timer */
  TACTL |= TACLR;               /* set TAR to 0 */
  return 0;
}

__attribute__((interrupt(TIMER0_A0_VECTOR))) void mseq_isr(void) {
  TACCR0 += MSEQ_TICKS;         /* set the next timer period */
  if (P1OUT & MSEQ_PIN)         /* DEBUG */
    P1OUT &= ~MSEQ_PIN;         /* DEBUG */
  else                          /* DEBUG */
    P1OUT |= MSEQ_PIN;          /* DEBUG */
}

__attribute__((interrupt(TIMER0_A1_VECTOR))) void morse_isr(void) {
  switch(__even_in_range(TAIV,TAIV_TAIFG))
    {
    case 0:                       /* no interrupt pending */
      return;
    case TA0IV_TACCR1:
      TACCR1 += MORSE_TICKS;      /* set the next timer period */
      tock();                     /* send out the next Morse code bit */
      return;
    case TA0IV_TAIFG:
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
  P1SEL = BIT1 | BIT5;
  
  /* Initialize data structures */
  clock = -1;
  inittock();
  
  __nop();                     /* assembler says we might need this */
  __enable_interrupt();
  timer_start();

  /* Send the message repeatedly */
  while (1)
    {
      __delay_cycles(1000000);
      ticks = MORSE_TICKS;
      inittock();
    }
}

