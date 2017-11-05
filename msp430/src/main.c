/**
 * @file: main.c
 * @author: Rodney Price
 * @brief Driver for the beacon with PN sequence and Morse code
 */

#include <msp430.h>
#include <stdint.h>

#include "config.h"
#include "interrupts.h"
#include "morse.h"
#include "msequence.h"
#include "si4432.h"


extern volatile uint32_t clock;

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


int main(int argc, char *argv[])
{
  configure_clocks();
  configure_pins();
  enable_nirq();
  enable_spi();

  si4432_reset();
  si4432_check_device();
  si4432_configure_gpio();
  si4432_set_frequency();
  si4432_init_rx_modem();
  si4432_init_tx_modem();
  
  /* Initialize data structures */
  inittock();                  /* set up Morse code generator */

  
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
    mseq_start();
    LPM3;                       /* sleep until finished */
    
    /* Take a breath */
    __delay_cycles(500000);
  }
}

