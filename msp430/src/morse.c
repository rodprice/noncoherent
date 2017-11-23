/**
 * morse.c, generation of Morse code from ASCII strings
 * Rodney Price, Sept 2017, Nov 2017
 *
 * This implementation tries to be as parsimonious as possible
 * with the stack and other variables, at the possible expense
 * of longer code length.
 *
 */


#include <msp430.h>
#include "morse.h"
#include "beacon.h"


/* What to say */
static const char message[] = "AD0YX send";
extern volatile key lastkey;    /* lagging Morse key state */

#define SPACE_CODE 0x00
#define DOT        0b10111111   /* 0xBF or 191 */
#define DELAY_DOT  0b00101111   /* 0x2F or  47 */
#define DASH       0b11101111   /* 0xEF or 239 */
#define DELAY_DASH 0b00111011   /* 0x3B or  59 */
#define SPACE      0b00001111   /* 0x0F or  15 */

#define bool uint8_t            /* not the C99 bool type! */
#define shifter uint8_t         /* shift register */

/* Left shift that preserves the LSB */
#define SHIFT(byte) ((byte << 1) + (byte & 0x01))


/* Access to the ASCII -> Morse lookup table */
shifter ascii2morse(char ascii) {
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
    return SPACE_CODE;          /* anything else acts like a space */
  }
}


/* Internal state */
static shifter symbols;         /* the current Morse letter being sent */
static shifter keys;            /* the current symbol being sent */
static uint8_t letter;          /* points to current letter in message */


/* Get the current key (MSB) and queue up the next one */
key next_key() {
  key out;
  out = (keys & 0x80) ? ON : OFF ;
  keys = SHIFT(keys);
  return out;
}

/* Have we sent the entire symbol (all the keys)? */
bool done_symbol() {
  return keys == 0xFF;
}

/* Get the next symbol, including a one-unit space between the last */
shifter next_short_symbol() {
  bool out;
  out = (symbols & 0x80) ? DASH : DOT ;
  symbols = SHIFT(symbols);
  return out;
}

/* Get the next symbol, including a three-unit space between the last */
shifter next_long_symbol() {
  bool out;
  out = (symbols & 0x80) ? DELAY_DASH : DELAY_DOT ;
  symbols = SHIFT(symbols);
  return out;
}

/* Have we sent the entire letter (all the symbols)? */
bool done_letter() {
  return (symbols == 0x00) || (symbols == 0xFF);
}

/* Get the next letter in the message */
char next_letter() {
  return ascii2morse(message[letter++]);
}

/* Have we sent all the letters (excluding the NULL at the end)? */
bool done_message() {
  return (letter >= sizeof(message) || message[letter] == 0);
}

/* Load up the first symbol and key, but don't send yet */
key init_tock() {
  letter = 0;
  symbols = next_letter();
  keys = next_short_symbol();
  return DOWN;
}

/* One tick of the Morse code state machine, a new key every tick */
key tock() {
  if ( !done_symbol() ) {
    return next_key();
  }
  if ( !done_letter() ) {
    keys = next_short_symbol();
    return next_key();
  }
  if ( !done_message() ) {
    symbols = next_letter();
    if (symbols == SPACE_CODE) {
      symbols = 0xFF;
      keys = SPACE;
    } else {
      keys = next_long_symbol();
    }
    return next_key();
  } else {
    return DOWN;
  }
}
