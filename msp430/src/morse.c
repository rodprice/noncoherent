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
    return morse[ascii-55];  /* ASCII cap letters start at 65 */
  } else if (97 <= ascii && ascii < 123) {
    return morse[ascii-87];  /* ASCII small letters start at 97 */
  } else if (ascii == '.') {
    return morse[36];
  } else if (ascii == ',') {
    return morse[37];
  } else if (ascii == '?') {
    return morse[38];
  } else if (ascii == '\'') {
    return morse[39];
  } else if (ascii == '!') {
    return morse[40];
  } else if (ascii == '/') {
    return morse[41];
  } else if (ascii == '(') {
    return morse[42];
  } else if (ascii == ')') {
    return morse[43];
  } else if (ascii == '&') {
    return morse[44];
  } else if (ascii == ':') {
    return morse[45];
  } else if (ascii == ';') {
    return morse[46];
  } else if (ascii == '=') {
    return morse[47];
  } else if (ascii == '+') {
    return morse[48];
  } else if (ascii == '-') {
    return morse[49];
  } else if (ascii == '_') {
    return morse[50];
  } else if (ascii == '"') {
    return morse[51];
  } else if (ascii == '$') {
    return morse[52];
  } else if (ascii == '@') {
    return morse[53];
  } else {
    return ONES;  /* the rest aren't ASCII characters */
  }
}

static uint8_t debug;           /* DEBUG */

static const char message[sizeof(MESSAGE)] = MESSAGE;

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

/* DEBUG: Clear the debug pins */
void newtock() {
  debug = P1OUT;
  debug &= ~( MESSAGE_START_PIN |
              CHARACTER_START_PIN |
              LETTER_START_PIN |
              WORD_START_PIN );
}

/* DEBUG: get the debug results */
uint8_t gettock() {
  return debug;
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
inline void nextchar() {
  if (mchar & BIT7)
    debug |= MORSE_PIN;   /* DEBUG */
    /* P1OUT |= MORSE_PIN;   /\* send a 1 *\/ */
  else
    debug &= ~MORSE_PIN;  /* DEBUG */
    /* P1OUT &= ~MORSE_PIN;  /\* send a 0 *\/ */
  mchar <<= 1;            /* queue up the next bit */
  mchar |= BIT0;
}

/* Sends a new bit at every call until ring buffer is empty */
BIT tock() {
  if (donechar()) {
    if (donecode()) {
      if (letter >= sizeof(MESSAGE)) { 
        return 0;
      } else {
        if (letter == 0)
          debug |= MESSAGE_START_PIN; /* DEBUG: signal start of message */
        mcode = message[letter++];
        debug |= LETTER_START_PIN; /* DEBUG: signal start of new letter */
        if (mcode == ONES) {
          debug |= WORD_START_PIN; /* DEBUG: signal start of new word */
          mchar = SPACEWORD;
        } else {
          nextspacecode();
        }
      }
    } else {
      debug |= CHARACTER_START_PIN; /* DEBUG: signal start of new character */
      nextcode();
    }
  }
  nextchar();
  return 1;
}
