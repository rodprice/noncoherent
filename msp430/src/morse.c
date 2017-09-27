

#include <stdio.h>
#include <stdint.h>
#include "morse.h"


/* ASCII numerals start at 48, capital letters at 65 */
uint16_t ascii2morse(char ascii) {
  if (48 <= ascii && ascii < 58) {
    return morse[ascii-48];     /* ASCII numerals start at 48 */
  } else if (65 <= ascii && ascii < 91) {
    return morse[ascii-65];     /* ASCII cap letters start at 65 */
  } else if (97 <= ascii && ascii < 123) {
    return morse[ascii-97];     /* ASCII small letters start at 97 */
    /* fix the following special characters */
  } else if (ascii == '+') {
    return morse[37];           /* 00101010 */
  } else if (ascii == '-') {
    return morse[38];           /* 01100001 */
  } else if (ascii == '=') {
    return morse[39];           /* 00110001 */
  } else if (ascii == '.') {
    return morse[40];           /* 01101010 */
  } else if (ascii == ',') {
    return morse[41];           /* 01110011 */
  } else if (ascii == '?') {
    return morse[42];           /* 01001100 */
  } else if (ascii == '/') {
    return morse[43];           /* 00101001 */
  } else {
    return -1;                  /* oops */
  }
}


/* This code is for illustration and testing purposes only */
char* make_dots_and_dashes(uint8_t code) {
  int i;
  uint8_t ltr, lsb, msb;
  static char morse[9];
  i = 0;
  ltr = code;
  do {
    msb = ltr & 0x80;
    if (msb)                    /* generate either a dot or a dash */
      morse[i++] = '-';
    else
      morse[i++] = '.';
    lsb = ltr & 0x01;           /* grab the lsb */
    ltr <<= 1;                  /* left-shift the code */
    if (lsb)
      ltr |= 0x01;              /* make the new lsb a 1 */
    else
      ltr &= 0xFE;              /* make the new lsb a 0 */
  } while ((ltr != 0) || (ltr != 255));
  morse[i++] = ' ';             /* pause between letters */
  morse[i] = '\n';
  return morse;
}


/* Run through all the codes */
void list_the_codes() {
  int i;
  printf("my code   morse code\n");
  for (i=0; i<MORSE_LUT_SIZE; i++) {
    code = morse[i];
    dotdash = make_dots_and_dashes(code);
    printf("%8b  %s\n", morse[i], make_dots_and_dashes(morse[i]));
}

