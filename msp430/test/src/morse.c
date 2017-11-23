/**
 * morse.c, generation of Morse code from ASCII strings
 * Rodney Price, Sept 2017, Nov 2017
 *
 * This implementation tries to be as parsimonious as possible
 * with the stack and other variables, at the possible expense
 * of longer code length.
 *
 */


#include <stdio.h>
#include <stdint.h>
#include "morse.h"


/* What to say */
static const char message[] = "AD0YX send";


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


key next_key() {
  key out;
  out = (keys & 0x80) ? ON : OFF ;
  keys = SHIFT(keys);
  return out;
}

bool done_symbol() {
  return keys == 0xFF;
}

shifter next_short_symbol() {
  bool out;
  out = (symbols & 0x80) ? DASH : DOT ;
  symbols = SHIFT(symbols);
  return out;
}

shifter next_long_symbol() {
  bool out;
  out = (symbols & 0x80) ? DELAY_DASH : DELAY_DOT ;
  symbols = SHIFT(symbols);
  return out;
}

bool done_letter() {
  return (symbols == 0x00) || (symbols == 0xFF);
}

char next_letter() {
  return ascii2morse(message[letter++]);
}

bool done_message() {
  return (letter >= sizeof(message) || message[letter] == 0);
}

key init_tock() {
  letter = 0;
  symbols = next_letter();
  keys = next_short_symbol();
  return DOWN;
}

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


/*************************************************************************
 * Debugging code
 *************************************************************************/

char key_string[] = "xxxxxxxx";
char* key2string(uint8_t byte) {
  key_string[0] = byte & 0x80 ? '1' : '0';
  key_string[1] = byte & 0x40 ? '1' : '0';
  key_string[2] = byte & 0x20 ? '1' : '0';
  key_string[3] = byte & 0x10 ? '1' : '0';
  key_string[4] = byte & 0x08 ? '1' : '0';
  key_string[5] = byte & 0x04 ? '1' : '0';
  key_string[6] = byte & 0x02 ? '1' : '0';
  key_string[7] = byte & 0x01 ? '1' : '0';
  return key_string;
}

char sym_string[] = "xxxxxxxx";
char* sym2string(uint8_t byte) {
  sym_string[0] = byte & 0x80 ? '1' : '0';
  sym_string[1] = byte & 0x40 ? '1' : '0';
  sym_string[2] = byte & 0x20 ? '1' : '0';
  sym_string[3] = byte & 0x10 ? '1' : '0';
  sym_string[4] = byte & 0x08 ? '1' : '0';
  sym_string[5] = byte & 0x04 ? '1' : '0';
  sym_string[6] = byte & 0x02 ? '1' : '0';
  sym_string[7] = byte & 0x01 ? '1' : '0';
  return sym_string;
}

char* show_key(key thiskey) {
  switch (thiskey) {
  case OFF:
    return "0";
  case ON:
    return "1";
  case DOWN:
    return "x";
  default:
    return "oops";
  }
}

key debug(key thiskey) {
  if (thiskey == DOWN) 
    printf("  down ");
  else
    printf("%c += %s ",
           message[letter-1],
           show_key(thiskey));
  printf("%s %s\n",
         key2string(keys),
         sym2string(symbols));
  return thiskey;
}

int main() {
  uint8_t i;
  printf("\n       keys     symbols\n");
  debug(init_tock());
  for (i=0; i<400; i++) {
    if (debug(tock()) == DOWN) {
      printf("\n");
      return 0;
    }
  }
  printf("\n");
  return 1;
}
