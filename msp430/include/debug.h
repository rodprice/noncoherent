/* Debugging code.  Includes function definitions as well as headers
   to avoid cluttering up other code. */

#ifndef DEBUG_H
#define DEBUG_H


#include <msp430.h>
#include <stdint.h>

/**********************************************************************/
/* Morse code */

/* Digital output pins */
#define RED_LED_PIN BIT0        /* LED on transmitter board */
#define GREEN_LED_PIN BIT6      /* Green LED on the TI Launchpad */
#define XMIT_PIN BIT2           /* Keys transmitter on or off */

#define EPOCH_PINS BIT7          /* Epoch toggle */
#define MORSE_PINS (XMIT_PIN | RED_LED_PIN)  /* Morse code output */
#define MSEQ_PINS (XMIT_PIN | GREEN_LED_PIN) /* M-sequence output */
#define TICKER_PIN BIT1          /* ticks when m-sequence starts */


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


#ifdef DEBUG_MORSE

#include "morse.h"

#define MESSAGE_PIN BIT4
#define WORD_PIN    BIT5
#define CODE_PIN    BIT3
#define CHAR_PIN    BIT7

#define DEBUG_MORSE_MASK ((MESSAGE_PIN) | \
                          (WORD_PIN)    | \
                          (CODE_PIN)    | \
                          (CHAR_PIN))


/* Set or clear P1OUT bits in mask */
#define SENDBIT_P1OUT_DEFAULT(bit,mask) \
  if (bit) {                            \
    (P1OUT) |= (mask);                  \
  } else {                              \
    (P1OUT) &= ~(mask);                 \
  }

/* Set or clear P2OUT bits in mask */
#define SENDBIT_P2OUT_DEFAULT(bit,mask) \
  if (bit) {                            \
    (P2OUT) |= (mask);                  \
  } else {                              \
    (P2OUT) &= ~(mask);                 \
  }

inline uint8_t debug_morse_p1out(uint8_t bits);

/* Duplicates functionality of SENDBIT while sending to debug pins too */
uint8_t debug_morse_sendbit(uint8_t bit, uint8_t mask);

/* Stop sending Morse code */
inline void debug_morse_stop() {
  TACCTL1 = 0;                   /* stop morse_isr() interrupts */
  P1OUT &= ~(MORSE_PINS);        /* turn out the lights */
#ifdef DEBUG_MORSE
  P1OUT &= ~(DEBUG_MORSE_MASK);  /* don't forget any */
#endif
}

#else  /* DEBUG_MORSE */

static uint8_t debug = ZEROS;

/* Returns debug outputs to be placed in P1OUT */
uint8_t debug_morse_sendbit(uint8_t bit, uint8_t mask) {
  uint8_t p1out;
  p1out = P1OUT;
  /* Duplicate SENDBIT_P1OUT_DEFAULT functionality */
  if (bit)
    p1out |= mask;
  else
    p1out &= ~mask;
  /* Add debug bits */
  if (debug) {
    /* set debug pins  */
    p1out |= (debug & DEBUG_MORSE_MASK);
    debug = ZEROS;
  } else {
    /* clear debug pins */
    p1out &= ~(DEBUG_MORSE_MASK);
  }
  return p1out;
}

/* Sends a new bit at every call until message is sent */
BIT tock() {
  if (donechar()) {
    debug |= CHAR_PIN;          /* DEBUG signal end of character */
    if (donecode()) {
      debug |= CODE_PIN;        /* DEBUG signal end of code */
      if (donemsg()) {
        debug |= MESSAGE_PIN;   /* DEBUG signal end of message */
        return 0;
      } else {
        if (code_ptr == 0)      /* DEBUG */
          debug |= MESSAGE_PIN; /* DEBUG signal start of message */
        mcode = ascii2morse(message[code_ptr++]);
        if (mcode == ONES) {
          debug |= WORD_PIN;    /* DEBUG signal start of word space */
          mchar = SPACEWORD;
        } else {
          debug |= CODE_PIN;    /* DEBUG signal start of code */
          nextspacecode();
        }
      }
    } else {
      debug |= CODE_PIN;        /* DEBUG signal start of code */
      nextcode();
    }
  }
  return nextchar();
}

#endif  /* DEBUG_MORSE */
#endif  /* DEBUG_MORSE */


#ifdef DEBUG_MSEQUENCE


#endif  /* DEBUG_MSEQUENCE */


#endif
