#ifndef CONFIG_H
#define CONFIG_H

#include <msp430.h>


#define MESSAGE "AD0YX sending m-sequence    "


/**********************************************************************/
/* m sequence */

/* Defines the bit rate of the m-sequence generator.  This sets
   TIMER_A to request an interrupt after MSEQ_TICKS of ACLK.  If ACLK
   is 32768 Hz and MSEQ_TICKS is 8, then an interrupt occurs 4096
   times per second.  A new bit of the m-sequence is sent at every
   interrupt call.  Given these settings, sending a 4095-bit
   m-sequence will take just less than 1 sec. */
#define MSEQ_TICKS 4            /* 0.25 ms */

/* Defines the number of bits in the shift register.  Choices
   currently are 3, 4, 5, 6, 7, 8, 10, and 12. */
#define REGISTER_LENGTH 3


/**********************************************************************/
/* morse code */

/* Defines the time unit of the Morse code generator.  A Morse code
   rate of 5 wpm corresponds to a time unit of 240 ms.   */
#define MORSE_TICKS 256          /* 6 ms */


/**********************************************************************/
/* ring buffer */

/* Number of characters in the buffer. Must be a power of 2. */
#define RINGSIZE 8


/**********************************************************************/
/* main.c */

/* Digital output pins */

#define RED_LED_PIN BIT0        /* LED on transmitter board */
#define GREEN_LED_PIN BIT6      /* Green LED on the TI Launchpad */
#define XMIT_PIN BIT2           /* Keys transmitter on or off */

#define EPOCH_PINS BIT7          /* Epoch toggle */
#define MORSE_PINS (RED_LED_PIN) /* Morse code output */
#define MSEQ_PINS (XMIT_PIN)     /* M-sequence output */


/**********************************************************************/
/* debugging */

/* #define DEBUG_MAIN */
#define DEBUG_MORSE
/* #define DEBUG_RINGBUFFER */
/* #define DEBUG_MSEQUENCE */

#ifdef DEBUG_MORSE

#define MESSAGE_PIN BIT4
#define WORD_PIN    BIT5
#define CODE_PIN    BIT6
#define CHAR_PIN    BIT7

inline uint8_t debug_morse_p1out(uint8_t bits);

#endif  /* DEBUG_MORSE */


#ifdef DEBUG_MSEQUENCE

#define MSEQ_PIN      BIT5 

#endif  /* DEBUG_MSEQUENCE */

#endif
