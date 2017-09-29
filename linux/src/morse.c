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


#define LSB 0x01
#define MSB 0x80
#define DOT       0b10111111
#define DASH      0b11101111
#define SPACEDOT  0b00101111
#define SPACEDASH 0b00111011
#define SPACEWORD 0b00001111

static uint8_t mcode;  /* the current Morse code being sent */
static uint8_t mchar;  /* the character being sent */
static uint8_t ticks;  /* ticks until end of current time unit */

static ringbuffer ring;

typedef uint8_t BIT;

/* I'm sure there's a better way to do this without putting stuff on
   the stack, but... this shifts out the msb of reg, while shifting in
   whatever happened to be in the lsb. */
BIT next(uint8_t *reg) {
  BIT msb, lsb;
  msb = *reg & MSB;
  lsb = *reg & LSB;
  *reg <<= 1;
  if (lsb)
    *reg |= LSB;
  else
    *reg &= 0xFE;
  return msb;
}

/* Raturns 1 iff work is done */
inline uint8_t done(uint8_t bits) {
  return ((bits == 0x00) || (bits == 0xFF));
}

/* This doesn't do spacing between letters and words ... yet */
BIT tock() {
  /* get a new bit and queue up the next one */
  if (done(mchar))
    {  /* get a new mchar and queue up the next one */
      if (done(mcode))
        {  /* get a new mcode and queue up the next one */
          if (rbempty(&ring))
            { /* wait for new letters in the buffer */
              mcode = 0x00;
              mchar = 0x00;
              return 0;
            }
          else
            {
              mcode = rbget(&ring);
              /* mcode = ascii2morse(rbget(&ring)); */
            if (mcode == 0xFF)
              mchar = SPACEWORD;
            else
              mchar = next(&mcode) ? SPACEDASH : SPACEDOT ;
            }
        }
      else
        {
          mchar = next(&mcode) ? DASH : DOT ;
        }
    }
  iowrite(next(&mchar));
  return 1;
}


#if DEBUG_MORSE

#include <stdio.h>
#include <string.h>

char* byte2binary(uint8_t byte);

static int idx = 0;
static int count = 0;
static char* msg = "AD0YX OOPS";
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
  if (idx==1 || trail7 || trail3) {
    printf("\n%c -> %c", msg[count++], signal[idx-1]);
  } else {
    printf("%c", signal[idx-1]);
  }
}

char* morse_init_test() {
  int i;
  
  idx = 0;
  for (i=0; i<SIGNAL_LENGTH; i++)
    signal[i] = '\0';

  /* This is done in setup */
  rbinit(&ring);
  
  /* This is done in the main loop */
  msg = "AD0YX OOPS";
  for (i=0; i<strlen(msg); i++)
    rbput(&ring, ascii2morse(msg[i]));
  
  /* Clean up */
  ticks = 0;
  mchar = 0xFF;
  mcode = 0xFF;

  return msg;
}

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

void test_tock() {
  int i;
  char* message;

  message = morse_init_test();
  printf("\nmessage = %s\n", message);
  for (i=0; i<SIGNAL_LENGTH; i++) {
    if (!tock())
      break;
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

#endif
