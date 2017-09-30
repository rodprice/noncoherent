/**
 * morse.c, generation of Morse code from ASCII strings
 * Rodney Price, Sept 2017
 *
 * This implementation tries to be as parsimonious as possible
 * with the stack and other variables, at the possible expense
 * of longer code length.
 *
 */


#include "morse.h"
#include "ringbuffer.h"


/* Change these should you ever wish to use 16-bit registers */
typedef uint8_t BIT;
typedef uint8_t REGISTER;

#define BIT0 0x01
#define BIT1 0x02
#define BIT7 0x80
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
    return -1;  /* the rest aren't ASCII characters */
  }
}


static REGISTER mcode;  /* the current Morse code being sent */
static REGISTER mchar;  /* the character being sent */
static REGISTER ticks;  /* ticks until end of current time unit */

static ringbuffer ring;


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
  BIT msb;
  msb = mchar & BIT7;
  mchar <<= 1;
  mchar |= BIT0;
  return msb;
}

/* Sends a new bit at every call until ring buffer is empty */
BIT tock() {
  if (donechar()) {
    if (donecode()) {
      if (rbempty(&ring)) { 
        mcode = ZEROS;
        mchar = ZEROS;
        return 0;         /* wait for new letters */
      } else {
        mcode = rbget(&ring);
        if (mcode == ONES)
          mchar = SPACEWORD;
        else
          nextspacecode();
      }
    } else {
      nextcode();
    }
  }
  iowrite(nextchar());
  return 1;
}


#if DEBUG_MORSE

#include <stdio.h>
#include <string.h>

char* byte2binary(uint8_t byte);

static int idx = 0;
static int count = 0;
static char msg[RINGSIZE];
static char signal[SIGNAL_LENGTH];

/* Debugging for tock()*/
void printock() {
  char* tmchar = byte2binary(mchar);
  printf("%s   ", tmchar);
  char* tmcode = byte2binary(mcode);
  printf("%s   ", tmcode);
  rbprint(&ring);
  printf("   %s\n", signal);
}

/* Check for a 'o' followed by n '_' in the signal buffer */
int trailing(int n) {
  int i;
  if (idx > n) {
    for (i=idx-n; i<idx; i++)
      if (signal[i] != '_')
        return 0;
    if (signal[idx-n-1] == 'o')
      return 1;
  }
  return 0;
}

/* Stand-in for digital write to pin */
void iowrite(uint8_t bit) {
  int trail3, trail7;
  trail3 = trailing(3);
  trail7 = trailing(7);
  
  if (idx < SIGNAL_LENGTH) {
    if (bit == 0)
      signal[idx++] = '_';
    else
      signal[idx++] = 'o';
  }

  if (idx == 1 || trail7 || trail3) {
    printf("\n%c -> %c", msg[count++], signal[idx-1]);
  } else {
    printf("%c", signal[idx-1]);
  }
}

void morse_init_test(char* message) {
  int i;

  idx = 0;
  count = 0;
  for (i=0; i<SIGNAL_LENGTH; i++)
    signal[i] = '\0';
  for (i=0; i<=strlen(message); i++)
    msg[i] = message[i];

  /* This is done in setup */
  rbinit(&ring);
  
  /* This is done in the main loop */
  for (i=0; i<strlen(msg); i++)
    rbput(&ring, ascii2morse(msg[i]));
  
  /* Clean up */
  ticks = 0;
  mchar = ONES;
  mcode = ONES;
}

void test_tock(char* message) {
  int i;

  morse_init_test(message);
  printf("\nmessage = %s\n", msg);
  for (i=0; i<SIGNAL_LENGTH; i++) {
    tock();
  }
  printf("\n\n%d iterations\n\n", i);
}  


char* byte2binary(uint8_t byte) {
  int i;
  uint8_t mask = 0x80;
  static char binary[9] = { ' ',' ',' ',' ',' ',' ',' ',' ','\0' };
  for (i=0; i<8; i++) {
    binary[i] = mask & byte ? '1' : '0' ;
    mask >>= 1;
  }
  return binary;
}

static const char* letters = 
  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.,?'!/()&:;=+-_\"$@";

void run_through_the_lookup_table() {
  int i;

  uint8_t code;
  char* dotdash;
  for (i=0; i<strlen(letters); i++) {
    code = ascii2morse(letters[i]);
    dotdash = make_dots_and_dashes(code);
    printf("%x  %c   %s   %s\n", letters[i], letters[i], byte2binary(code), dotdash);
  }
}

/* First prototype of interrupt-driven state machine */
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
  dotdash[++i] = ' ';           /* pause between letters */
  dotdash[++i] = '\0';
  return dotdash;
}

#endif
