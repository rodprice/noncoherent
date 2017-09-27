/**
 * @file: main.c
 * @author: Rodney Price
 * @brief Driver for the cheap beacon with PN sequence
 */

#include <stdio.h>
#include <assert.h>
#define assert__(x) for ( ; !(x) ; assert(x) )

#include <stdint.h>
#include "ringbuffer.h"
#include "msequence.h"
#include "morse.h"


#include <string.h>

#define TAKE_A_FEW_TRIPS_THROUGH_THE_ISR 1
#define RUN_THROUGH_THE_LOOKUP_TABLE 0

char* byte2binary(uint8_t byte) {
  int i;
  uint8_t bits;
  static char binary[9] = { ' ',' ',' ',' ',' ',' ',' ',' ','\0' };
  bits = byte;
  for (i=0; i<8; i++) {
    if (bits & 0x80)
      binary[i] = '1';
    else
      binary[i] = '0';
    bits <<= 1;
  }
  return binary;
}

#if RUN_THROUGH_THE_LOOKUP_TABLE
static const char* letters = 
  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.,?'!/()&:;=+-_\"$@";
#endif

/* This code is for illustration and testing purposes only */
char* make_dots_and_dashes(uint8_t code) {
  int i;
  uint8_t ltr, lsb, msb;
  static char dotdash[10];
  for (i=0; i<9; i++)
    dotdash[i] = 0;
  i = 0;
  ltr = code;
  for (i=0; i<8; i++) {
    msb = ltr & 0x80;
    if (msb)                    /* generate either a dot or a dash */
      dotdash[i] = '-';
    else
      dotdash[i] = '.';
    lsb = ltr & 0x01;           /* grab the lsb */
    ltr <<= 1;                  /* left-shift the code */
    if (lsb)
      ltr |= 0x01;              /* make the new lsb a 1 */
    else
      ltr &= 0xFE;              /* make the new lsb a 0 */
    if ((ltr == 0) || (ltr == 255))
      break;
  }
  dotdash[++i] = ' ';             /* pause between letters */
  dotdash[++i] = '\0';
  return dotdash;
}

char* byte2binary(uint8_t byte);

#if TAKE_A_FEW_TRIPS_THROUGH_THE_ISR
/* Stand-in for digital write to pin */
void iowrite(uint8_t bit);
void printisr();
#endif

int main() {
  int i;

  #if RUN_THROUGH_THE_LOOKUP_TABLE
  
  uint8_t code;
  char* dotdash;
  for (i=0; i<strlen(letters); i++) {
    code = ascii2morse(letters[i]);
    dotdash = make_dots_and_dashes(code);
    printf("%x  %c   %s   %s\n", letters[i], letters[i], byte2binary(code), dotdash);
  }
  return 0;
  
  #endif

  
  #if TAKE_A_FEW_TRIPS_THROUGH_THE_ISR
  
  test_rbqueue();
  test_rbprint();
  test_rbpointers();
  /* morse_init_test(); */
  /* printf("ticks      mchar      mcode      signal\n"); */
  /* for (i=0; i<8; i++) { */
  /*   printisr(); */
  /*   isr(); */
  /* } */
  /* printisr(); */

  i= 0;
  return 0;
  
  #endif
}
