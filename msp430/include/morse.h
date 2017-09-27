#ifndef MORSE_H
#define MORSE_H

#include <stdint.h>


/*
 * Morse code definition.
 *
 * These codes are taken from the Wikipedia article "Morse code," at
 * https://en.wikipedia.org/wiki/Morse_code.  It includes all ITU-R
 * codes as listed in the article.
 *
 * The length of a dot is one unit.  A dash is three units.  The space
 * between parts of the same letter is one unit.  The space between
 * letters is three units.  The space between words is seven units.
 *
 * These codes use 0 for dot and 1 for dash, but they have a suffix as
 * well, which is used in the state machine below.  If the last bit to
 * be sent is 1 (a dash), then the suffix is all 0's, but if the last
 * bit to be sent is 0 (a dot), the suffix is all 1's.  The suffix
 * fills out the code until it has 8 bits.  For example, if the letter
 * to be sent is 'C', -.-., then the code is 10101111.  If the letter
 * to be sent is 'A', .-, then the code is 01000000.
 *
 * The state machine sends out the dots and dashes for a letter by
 * reading out the MSB of the code and sending out either a dot or a
 * dash.  It then reads out the current LSB of the code, shifts the
 * code to the right one place, and replaces the LSB of the new code
 * with the LSB it read out.  This way, once the state machine sees a
 * code of either all 0's or all 1's, it knows that the letter has
 * been sent.  The function "make_dots_and_dashes" in morse.c
 * illustrates the process.
 */

#define MORSE_LUT_SIZE 55

static const uint8_t morse[MORSE_LUT_SIZE] =
  {
    0b   01111,                 /* '1' */
    0b   00111,                 /* '2' */
    0b   00011,                 /* '3' */
    0b   00001,                 /* '4' */
    0b   00000,                 /* '5' */
    0b   10000,                 /* '6' */
    0b   11000,                 /* '7' */
    0b   11100,                 /* '8' */
    0b   11110,                 /* '9' */
    0b   11111,                 /* '0' */
    0b      01,                 /* 'A' */
    0b    1000,                 /* 'B' */
    0b    1010,                 /* 'C' */
    0b     100,                 /* 'D' */
    0b       0,                 /* 'E' */
    0b    0010,                 /* 'F' */
    0b    0000,                 /* 'H' */
    0b      00,                 /* 'I' */
    0b    0111,                 /* 'J' */
    0b     101,                 /* 'K' */
    0b    0100,                 /* 'L' */
    0b      11,                 /* 'M' */
    0b      10,                 /* 'N' */
    0b     111,                 /* 'O' */
    0b     010,                 /* 'R' */
    0b     000,                 /* 'S' */
    0b       1,                 /* 'T' */
    0b     001,                 /* 'U' */
    0b    0001,                 /* 'V' */
    0b     011,                 /* 'W' */
    0b    1001,                 /* 'X' */
    0b    1011,                 /* 'Y' */
    0b    1100,                 /* 'Z' */
    0b  010101,                 /* '.' */
    0b  110011,                 /* ',' */
    0b  001100,                 /* '?' */
    0b  011110,                 /* ''' */
    0b  101011,                 /* '!' */
    0b   10010,                 /* '/' */
    0b   10110,                 /* '(' */
    0b  101101,                 /* ')' */
    0b  111000,                 /* ':' */
    0b  101010,                 /* ';' */
    0b   10001,                 /* '=' */
    0b   01010,                 /* '+' */
    0b  100001,                 /* '-' */
    0b  010010,                 /* '"' */
    0b  011010,                 /* '@' */
    0b  000101,                 /* end of work */
    0b 0000000,                 /* error */
    0b     101,                 /* invitation to transmit, or 'K' */
    0b   10101,                 /* starting signal */
    0b   01010,                 /* new page signal */
    0b   00010,                 /* understood, or 'Ŝ' */
    0b   01000                  /* wait, or ampersand */
  };

volatile uint16_t mcode;
volatile uint8_t mbit;

/* Interrupt routine to send a single Morse character */
/* void sendchar(uint16_t morse, uint8_t pin) { */
/*   if (mbit > 1) { */
/*     P1OUT = pin;                /\* max mbit = 4 if dash, 2 if dot *\/ */
/*   } else if (mbit == 1) { */
/*     P1OUT = 0;                  /\* one unit space *\/ */
/*   } else { */
    
/*   } */
/* } */


#endif
