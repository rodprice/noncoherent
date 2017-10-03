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


inline void xtal_pin_set() {
  /* Set output pin for crystal fault */
  if (BCSCTL3 & LFXT1OF)
    P1OUT |= XTAL_FAULT_PIN;
  else
    P1OUT &= ~XTAL_FAULT_PIN;
}

int timer_start(void) {
  P1OUT = 0;                /* DEBUG */
  xtal_pin_set();           /* set high until crystal osc is running */
  TACTL = TASSEL0 | ID0 | MC0 | TACLR; /* clock source ACLK, divide by 1, timer off */
  TACCR0 = MSEQ_TICKS;      /* interrupt frequency in units of ACLK */
  TACCTL0 = CCIE;           /* compare mode, interrupt enabled */
  TACTL |= MC1;             /* timer in up mode and we're off... */
  return 0;
}

int timer_stop(void) {
  TACCR0 = 0;                   /* stop the timer */
  TACTL |= TACLR;               /* set TAR to 0 */
  return 0;
}

__attribute__((interrupt(TIMER0_A0_VECTOR))) void timer_isr(void) {
  TACCTL0 &= ~CCIFG;      /* clear the interrupt flag */
  
  xtal_pin_set();         /* update the crystal osc status */
  P1OUT ^= MSEQ_PIN;      /* DEBUG */

  /* real-time clock */
  if (!(BCSCTL3 & LFXT1OF) && (clock == -1))
    clock = 0;
  clock++;
    
  /* morse code generator */
  ticks--;
  if (ticks == 0) {
    P1OUT ^= LED_PIN;
    newtock();           /* DEBUG */
    tock();              /* send a new morse bit */
    P1OUT = gettock();   /* DEBUG */
    ticks = MORSE_TICKS; /* start it all over again */
  }
}


int main(int argc, char *argv[])
{
  int i;
  
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
  
  /* Initialize data structures */
  clock = -1;
  inittock();
  
  __enable_interrupt();
  timer_start();

  /* Send the message repeatedly */
  while (1)
    {
      __delay_cycles(2000000);
      ticks = MORSE_TICKS;
      inittock();
    }
}

