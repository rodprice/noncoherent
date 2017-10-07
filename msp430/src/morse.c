/**
 * morse.c, generation of Morse code from ASCII strings
 * Rodney Price, Sept 2017
 *
 * This implementation tries to be as parsimonious as possible
 * with the stack and other variables, at the possible expense
 * of longer code length.
 *
 */


#include <msp430.h>
#include <stdint.h>

#include "morse.h"
#include "config.h"


/* Change these should you ever wish to use 16-bit registers */
typedef uint8_t BIT;
typedef uint8_t REGISTER;

#define ONES 0xFF
#define ZEROS 0x00
#define DOT  0b10111111
#define DASH 0b11101111
#define SPACEDOT  0b00101111
#define SPACEDASH 0b00111011
#define SPACEWORD 0b00001111


/* Access to the ASCII -> Morse lookup table */
REGISTER ascii2morse(char ascii) {
  if (48 <= ascii && ascii < 58) {
    return morse[ascii-48];     /* ASCII numerals start at 48 */
  } else if (65 <= ascii && ascii < 91) {
    return morse[ascii-55];     /* ASCII cap letters start at 65 */
  } else if (97 <= ascii && ascii < 123) {
    return morse[ascii-87];     /* ASCII small letters start at 97 */
  } else {
    switch(ascii) {
    case '.': return morse[36];
    case ',': return morse[37];
    case '?': return morse[38];
    case '\'': return morse[39];
    case '!': return morse[40];
    case '/': return morse[41];
    case '(': return morse[42];
    case ')': return morse[43];
    case '&': return morse[44];
    case ':': return morse[45];
    case ';': return morse[46];
    case '=': return morse[47];
    case '+': return morse[48];
    case '-': return morse[49];
    case '_': return morse[50];
    case '"': return morse[51];
    case '$': return morse[52];
    case '@': return morse[53];
    }
    return ONES;  /* the rest aren't ASCII characters */
  }
}

/* Internal state */
static REGISTER mcode;  /* the current Morse code being sent */
static REGISTER mchar;  /* the character being sent */
static uint8_t letter;  /* points to current letter in message */

/* Initialize the Morse code generator */
inline void inittock() {
  mchar = ONES;
  mcode = ONES;
  letter = 0;
}

/* All the bits in the character have been sent */
inline BIT donechar() {
  return mchar == ZEROS || mchar == ONES;
}

/* All the characters in the code have been sent */
inline BIT donecode() {
  return mcode == ZEROS || mcode == ONES;
}

/* Start sending a new character and queue up the next one */
inline void nextspacecode() {
  if (mcode & BIT7)  /* three-unit space between letters */
    mchar = SPACEDASH;
  else
    mchar = SPACEDOT;
  mcode <<= 1;       /* queue up the next character */
  if (mcode & BIT1)
    mcode |= BIT0;
}

/* Start sending a new character and queue up the next one */
inline void nextcode() {
  if (mcode & BIT7)  /* one-unit space between characters */
    mchar = DASH;
  else
    mchar = DOT;
  mcode <<= 1;       /* queue up the next character */
  if (mcode & BIT1)
    mcode |= BIT0;
}

/* Send a new bit and queue up the next one */
inline BIT nextchar() {
  mchar <<= 1;       /* queue up the next bit */
  mchar |= BIT0;
  return (mchar & BIT7) ? 1 : 0 ;
}

/* Sends a new bit at every call until ring buffer is empty */
#if DEBUG
BIT tock() {
  if (donechar()) {
    P1OUT |= CHARACTER_START_PIN;     /* DEBUG signal new character */
    if (donecode()) {
      P1OUT |= LETTER_START_PIN;      /* DEBUG signal new letter */
      if (letter >= sizeof(MESSAGE)) { 
        P1OUT &= ~MESSAGE_START_PIN;  /* DEBUG signal end of message */
        return 0;
      } else {
        if (letter == 0)              /* DEBUG */
          P1OUT |= MESSAGE_START_PIN; /* DEBUG signal start of message */
        mcode = ascii2morse(message[letter++]);
        if (mcode == ONES) {
          P1OUT |= WORD_START_PIN;    /* DEBUG signal new word */
          mchar = SPACEWORD;
        } else {
          P1OUT &= ~WORD_START_PIN;   /* DEBUG */
          P1OUT &= ~LETTER_START_PIN; /* DEBUG */
          nextspacecode();
        }
      }
    } else {
      P1OUT &= ~WORD_START_PIN;       /* DEBUG */
      P1OUT &= ~LETTER_START_PIN;     /* DEBUG */
      nextcode();
    }
  } else {                            /* DEBUG */
    P1OUT &= ~CHARACTER_START_PIN;    /* DEBUG */
  }
  return nextchar();
}
#else
BIT tock() {
  if (donechar()) {
    if (donecode()) {
      if (letter >= sizeof(MESSAGE)) { 
        return 0;
      } else {
        mcode = ascii2morse(message[letter++]);
        if (mcode == ONES) {
          mchar = SPACEWORD;
        } else {
          nextspacecode();
        }
      }
    } else {
      nextcode();
    }
  }
  return nextchar();
}
#endif
