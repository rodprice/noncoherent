/**
 * @file: main.c
 * @author: Rodney Price
 * @brief Driver for the cheap beacon with PN sequence
 */

#include <msp430.h>
#include <stdint.h>


/* create variable __sr on the stack */
#define SR_ALLOC() uint16_t __sr
/* save the status register and disable interrupts */
#define ENTER_CRITICAL() __sr = _get_interrupt_state(); __disable_interrupt()
/* restore the saved value of the status register */
#define EXIT_CRITICAL() __set_interrupt_state(__sr)

/* sample interval in milliseconds */
#define PERIOD 500
#define LED 0x01
#define XMIT 0x04

static uint16_t tick;       /* timer tick */


int timer_start(void) {
  /* clock source SMCLK, divide by 1, timer off */
  TACTL = TASSEL1 | ID0 | MC0 | TACLR;
  /* interrupt every 1 ms */
  TACCR0 = 100;
  /* compare mode, interrupt enabled */
  TACCTL0 = CCIE;
  /* timer in up mode and we're off... */
  TACTL |= MC1;
  return 0;
}

int timer_stop(void) {
  TACCR0 = 0;                   /* stop the timer */
  TACTL |= TACLR;               /* set TAR to 0 */
  return 0;
}

__attribute__((interrupt(TIMER0_A0_VECTOR))) void timer_isr(void) {
  /* clear the interrupt flag */
  TACCTL0 &= ~CCIFG;
  /* increment the timer tick */
  tick++;
  if (tick >= PERIOD) {
    tick = 0;
    P1OUT ^= LED | XMIT;        /* toggle outputs */
  }
}

int main(int argc, char *argv[])
{
    /* Hold the watchdog */
    WDTCTL = WDTPW + WDTHOLD;

    /* configure the clocks */
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    BCSCTL3 |= LFXT1S_2;        /* ACLK from internal VLO */

    /* set all digital i/o pins to output */
    P1DIR = 0xFF;
    P2DIR = 0xFF;

    /* Set all digital i/o outputs high */
    P1OUT = 0xFF;
    P2OUT = 0xFF;

    __enable_interrupt();
    timer_start();

    while (1) {
        /* Wait for 2000000 cycles */
        /* __delay_cycles(2000000); */
        
        /* Toggle P1.0 and P1.3 output */
        /* P1OUT ^= LED | XMIT; */
  }
}

