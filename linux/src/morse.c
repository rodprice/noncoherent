/**
 * morse.c, generation of Morse code from ASCII strings
 * Rodney Price, Sept 2017
 * 
 */


#include "morse.h"
#include "ringbuffer.h"

/* Access to the ASCII -> Morse lookup table */
uint8_t ascii2morse(char ascii) {
  if (48 <= ascii && ascii < 58) {
    return morse[ascii-48];     /* ASCII numerals start at 48 */
  } else if (65 <= ascii && ascii < 91) {
    return morse[ascii-65+10];  /* ASCII cap letters start at 65 */
  } else if (97 <= ascii && ascii < 123) {
    return morse[ascii-97+10];  /* ASCII small letters start at 97 */
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
    return -1;                  /* the rest aren't ASCII characters */
  }
}


/* Number of ticks (trips through the ISR) per time unit */
#define TIMEUNIT 24

#define LSB 0x01
#define MSB 0x80
#define SPACE 0xFF
#define NEXT(bits) (((bits) << 1) | LSB)
#define DONE(bits) (((bits) == 0x00) || ((bits) == 0xFF))

/* Character definitions */
#define DOT       0b10111111
#define DASH      0b11101111
#define SPACEDOT  0b00010111
#define SPACEDASH 0b00011101
#define SPACEWORD 0b00001111

static volatile uint8_t mcode;  /* the current Morse code being sent */
static volatile uint8_t mchar;  /* the character being sent */
static volatile uint8_t ticks;  /* ticks until end of current time unit */

static ringbuffer ring;


/* Prototype for interrupt routine */
void isr() {
  if (ticks == 0) {
    ticks = TIMEUNIT;
    if (DONE(mchar)) {
      if (DONE(mcode)) {
        if (rbempty(&ring)) {
          iowrite(0);           /* turn off the transmitter */
          return;
        } else { /* not done with buffer, done with letter, character */
          mcode = rbget(&ring); /* get a letter and queue up the next */
          if (mcode == SPACE)   /* we're at the end of a word */
            mchar = SPACEWORD;
          else                  /* get the current character */
            mchar = (mcode & MSB) ? SPACEDASH : SPACEDOT ;
          iowrite(mchar & LSB); /* send out the first bit */
          mchar = NEXT(mchar);  /* queue up the next bit */
          mcode = NEXT(mcode);  /* queue up the next character */
        }
      } else { /* not done with letter, done with character */
        mchar = (mcode & MSB) ? DASH : DOT ; /* get the next character */
        iowrite(mchar & LSB);   /* send out the first bit */
        mchar = NEXT(mchar);    /* queue up the next bit */
        mcode = NEXT(mcode);    /* queue up the next character */
      }
    } else { /* not done with letter or character */
      iowrite(mchar & 0x80);    /* send out the current bit */
      mchar = NEXT(mchar);      /* queue up the next bit */
    }
  } else {/* not done with time unit */
    ticks--;                    /* queue up the next tick */
  }
}

/* Stand-in for digital write to pin */
static int idx = 0;
static char signal[80];
void iowrite(uint8_t bit) {
  if (idx < 80) {
    if (bit == 0)
      signal[idx++] = '_';
    else
      signal[idx++] = 'o';
  }
}

char* byte2binary(uint8_t byte);

void morse_init_test() {
  int i;
  ringbuffer ring;
  idx = 0;
  for (i=0; i<80; i++)
    signal[i] = '\0';

  rbinit(&ring);
  rbput(&ring, 'A');
  rbput(&ring, 'D');
  rbput(&ring, '0');
  rbput(&ring, 'Y');
  rbput(&ring, 'X');
  
  ticks = 0;
  mchar = 0xFF;
  mcode = 0xFF;
}

#include <stdio.h>
  
void printisr() {
  printf("%s   %s   %s   %s\n",
         byte2binary(ticks),
         byte2binary(mchar),
         byte2binary(mcode),
         signal);
}

