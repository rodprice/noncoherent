#ifndef CONFIG_H
#define CONFIG_H

#include <msp430.h>

#define DEBUG 1

#define MESSAGE "AD0YX sending m-sequence    "


/**********************************************************************/
/* main.c */

/* Digital output pins.  All but LED_PIN and XMIT_PIN are used for
   debugging purposes only. */

#define LED_PIN BIT0            /* LED on transmitter board */
#define MORSE_PIN BIT0          /* Morse code output */

#define XTAL_FAULT_PIN BIT1     /* Crystal oscillator fault */

#define XMIT_PIN BIT2           /* Keys transmitter on or off */

#define MSEQ_PIN BIT3           /* M-sequence output */

#define MESSAGE_START_PIN BIT4

#define MSEQ_START_PIN BIT5     /* M-sequence start tick */
#define CHARACTER_START_PIN BIT5

#define MORSE_START_PIN BIT6    /* Morse code message start tick */
#define LETTER_START_PIN BIT6

#define EPOCH_START_PIN BIT7    /* Epoch start tick */
#define WORD_START_PIN BIT7

#define GREEN_LED_PIN BIT6      /* Green LED on the TI Launchpad */


/**********************************************************************/
/* m sequence */

/* Defines the bit rate of the m-sequence generator.  This sets
   TIMER_A to request an interrupt after MSEQ_TICKS of ACLK.  If ACLK
   is 32768 Hz and MSEQ_TICKS is 8, then an interrupt occurs 4096
   times per second.  A new bit of the m-sequence is sent at every
   interrupt call.  Given these settings, sending a 4095-bit
   m-sequence will take just less than 1 sec. */
#define MSEQ_TICKS 4

/* Defines the number of bits in the shift register.  Choices
   currently are 3, 4, 5, 6, 7, 8, 10, and 12. */
#define REGISTER_LENGTH 3

/* Defines the size of the lookup table used in the Fibonacci shift
   registers.  Choices are 16 and 256. */
#define MSEQUENCE_LUT_SIZE 16


/**********************************************************************/
/* morse code */

/* Defines the time unit of the Morse code generator.  A Morse code
   rate of 5 wpm corresponds to a time unit of 240 ms.   */
#define MORSE_TICKS 24          /* 6 ms */


/**********************************************************************/
/* ring buffer */

/* Number of characters in the buffer. Must be a power of 2. */
#define RINGSIZE 16


#endif
