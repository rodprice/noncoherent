/**
 * morse.c, generation of Morse code from ASCII strings
 * Rodney Price, Sept 2017
 * 
 */

#ifndef MORSE_H
#define MORSE_H

#include <stdint.h>


/*
 * Morse code definition.
 *
 * These codes are taken from the Wikipedia article "Morse code," at
 * https://en.wikipedia.org/wiki/Morse_code.  It includes all codes as
 * listed in the article, including prosigns, excluding the codes
 * marked as non-English.
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

#define MORSE_LUT_SIZE 61

/* Morse code lookup table */
static const uint8_t morse[MORSE_LUT_SIZE] =
  {
    0b11111000,                 /* '0' */
    0b01111000,                 /* '1' */
    0b00111000,                 /* '2' */
    0b00011000,                 /* '3' */
    0b00001000,                 /* '4' */
    0b00000111,                 /* '5' */
    0b10000111,                 /* '6' */
    0b11000111,                 /* '7' */
    0b11100111,                 /* '8' */
    0b11110111,                 /* '9' */
    0b01000000,                 /* 'A' */
    0b10001111,                 /* 'B' */
    0b10101111,                 /* 'C' */
    0b10011111,                 /* 'D' */
    0b01111111,                 /* 'E' */
    0b00101111,                 /* 'F' */
    0b11011111,                 /* 'G' */
    0b00001111,                 /* 'H' */
    0b00111111,                 /* 'I' */
    0b01110000,                 /* 'J' */
    0b10100000,                 /* 'K' */
    0b01001111,                 /* 'L' */
    0b11000000,                 /* 'M' */
    0b10111111,                 /* 'N' */
    0b11100000,                 /* 'O' */
    0b01101111,                 /* 'P' */
    0b11010000,                 /* 'Q' */
    0b01011111,                 /* 'R' */
    0b00011111,                 /* 'S' */
    0b10000000,                 /* 'T' */
    0b00100000,                 /* 'U' */
    0b00010000,                 /* 'V' */
    0b01100000,                 /* 'W' */
    0b10010000,                 /* 'X' */
    0b10110000,                 /* 'Y' */
    0b11001111,                 /* 'Z' */
    0b01010100,                 /* '.' */
    0b11001100,                 /* ',' */
    0b00110011,                 /* '?' */
    0b01111011,                 /* ''' */
    0b10101100,                 /* '!' */
    0b10010111,                 /* '/' */
    0b10110111,                 /* '(' */
    0b10110100,                 /* ')' */
    0b01000111,                 /* '&' */
    0b11100011,                 /* ':' */
    0b10101011,                 /* ';' */
    0b10001000,                 /* '=' */
    0b01010111,                 /* '+' */
    0b10000100,                 /* '-' */
    0b00110100,                 /* '_' */
    0b01001011,                 /* '"' */
    0b00010010,                 /* '$' */
    0b01101011,                 /* '@' */
    0b00010100,                 /* end of work */
    0b00000001,                 /* error */
    0b10100000,                 /* invitation to transmit, or 'K' */
    0b10101000,                 /* starting signal */
    0b01010111,                 /* new page signal */
    0b00010111,                 /* understood, or 'Ŝ' */
    0b01000111                  /* wait, or ampersand */
  };

/* Change an ASCII code into a Morse code */
uint8_t ascii2morse(char ascii);

/* Is it done yet?  Is it done yet?  Is it done yet? */
uint8_t donemsg();

/* Initialize the Morse code generator */
void inittock();

/* Generate Morse code sequence */
uint8_t tock();

/* Start sending Morse code */
void morse_start();

/* Stop sending Morse code */
void morse_stop();

#endif
