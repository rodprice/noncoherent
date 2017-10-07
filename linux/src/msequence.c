/*
   Functions implementing the fast m-sequence transform, as defined by
   Cohn and Lempel (1977).  These functions can also be used to
   generate an m-sequence.  This code updates the code in my 1985
   master's thesis to code that can be used in a microcontroller.
   Large parts of this code are auto-generated in a Mathematica
   notebook, m-sequence-generators.nb, where polynomials are defined
   and algorithms are tested.  Refer to that notebook for details on
   the algorithms below.

   Rodney Price, Sept 2017
*/

#include <stdint.h>
#include "msequence.h"


/* If short on memory, use this lookup table */
#if LUT_SIZE == 16
static const uint8_t lut[16] =
{
  0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4
};
#endif

/* This one will be faster */
#if LUT_SIZE == 256
static const uint8_t lut[256] = 
{
  0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
 };
#endif


/* Count the number of ones in an 8-bit word, if you're short on
   memory.  The left-most 8 bits will be ignored. */
#if ((REGISTER_LENGTH <= 8) && (LUT_SIZE == 16))
uint8_t count(uint16_t byte) {
  uint8_t hi = (byte & 0xF0) >> 4;
  uint8_t lo =  byte & 0x0F;
  return lut[hi] + lut[lo];
}
#endif

/* Count the number of bits in an 8-bit word.  The left-most 8 bits
   will be ignored. */
#if ((REGISTER_LENGTH <= 8) && (LUT_SIZE == 256))
uint8_t count(uint16_t byte) {
  return lut[byte & 0x00FF];
}
#endif

/* Count the number of ones in a 16-bit word, if you're short on memory */
#if ((REGISTER_LENGTH > 8) && (LUT_SIZE == 16))
uint8_t count(uint16_t word) {
  uint8_t nib3 = (word & 0xF000) >> 12;
  uint8_t nib2 = (word & 0x0F00) >> 8;
  uint8_t nib1 = (word & 0x00F0) >> 4;
  uint8_t nib0 =  word & 0x000F;
  return lut[nib3] + lut[nib2] + lut[nib1] + lut[nib0];
}
#endif

/* Count the number of ones in a 16-bit word */
#if ((REGISTER_LENGTH > 8) && (LUT_SIZE == 256))
uint8_t count(uint16_t word) {
  uint8_t hi = (word & 0xFF00) >> 8;
  uint8_t lo =  word & 0x00FF;
  return lut[hi] + lut[lo];
}
#endif


/* Implements a Fibonacci shift register */
uint16_t fibshift(uint16_t bits) {
  uint8_t carry = count(bits & POLY) & 0x01;
  return (carry << (REGLEN-1)) + (bits >> 1);
}

/* Implements a "backwards" Fibonacci shift register */
uint16_t backfibshift(uint16_t bits) {
  uint16_t carry = count(bits & RPOLY) & 0x01;
  return (bits << 1) + carry;
}

/* Implements a Galois shift register */
uint16_t galshift(uint16_t bits) {
  uint16_t carry = (bits & 0x01) << (REGLEN-1);
  uint16_t next = carry ? (bits^GPOLY) : bits;
  return carry + (next>>1);
}


/* Fast Hadamard transform of length 2^p on array a.  The transform is
   done in place.  There are no multiplies in this function. */
void hadamard(int16_t* a, uint16_t p) {
  uint16_t ell, x, i, j, k, m, n;
  int16_t temp;
  for(ell=0; ell<=p-1; ell++) {
    x = 1<<(p-2);
    for(m=0; m<x; m++) {
      k = m;
      for(n=0; n<(1<<ell); n++) {
        i = k;
        j = i+x;
        temp = a[i] + a[j];
        a[j] = a[i] - a[j];
        a[i] = temp;
        k += x+x;
      }
    }
  }
}


#if DEBUG


char* tobinarystring(uint16_t bits) {
  static char binarystring[REGLOAD+1];
  int i;
  uint16_t mask = REGLOAD;
  for (i=0; i<=REGLOAD; i++)
    binarystring[i] = '\0';
  for (i=0; i<REGLEN; i++) {
    binarystring[i] = (bits & mask) ? '1' : '0';
    mask >>= 1;
  }
  return binarystring;
}

int mismatch(uint8_t bit, uint16_t reg2) {
  uint8_t reg;
  reg = (REGLOAD & reg2) ? 1 : 0;
  return bit ^ reg;
}

/* Check whether the MSB of an array matches the bits array at given lag  */
int matchbits(int lag, uint16_t* sequence) {
  int i;
  for (i=0; i<SEQLEN; i++) {
    if (mismatch(bits[i], sequence[(i+lag) % SEQLEN]))
      return 0;                 /* failure */
  }
  return 1;                     /* success */
}

static uint16_t galois;         /* Galois shift register */

void test_galshift() {
  int i=0, lag;
  static uint16_t record[SEQLEN+REGLEN+1];
  /* Run the Galois shift register */
  galois = REGLOAD;
  printf("%d -> %d (%s)\n", i, ((REGLOAD & galois) ? 1 : 0), tobinarystring(galois));
  record[0] = galois;
  for (i=1; i<SEQLEN+1; i++) {
    galois = galshift(galois);
    record[i] = galois;
    printf("%d -> %d (%s)\n", i, ((REGLOAD & galois) ? 1 : 0), tobinarystring(galois));
  }
  /* Print out results for various lags */
  /* for (lag=0; lag<SEQLEN; lag++) { */
  /*   printf("\nlag = %d\n", lag); */
  /*   for (i=0; i<SEQLEN; i++) { */
  /*     printf("%d,", bits[i]); */
  /*   } */
  /*   printf("\n"); */
  /*   for (i=0; i<SEQLEN; i++) { */
  /*     printf("%d,", (REGLOAD & record[(i + lag) % SEQLEN]) ? 1 : 0); */
  /*   } */
  /*   printf("\n"); */
  /*   for (i=0; i<SEQLEN; i++) { */
  /*     printf("%d,", mismatch(bits[i], record[(i + lag) % SEQLEN])); */
  /*   } */
  /*   printf("\n"); */
  /* } */
  /* Check for matches at various lags */
  for (lag=0; lag<SEQLEN; lag++) {
    if (matchbits(lag, record)) {
      printf("\nFound match to m-sequence at lag = %d\n", lag);
      return;
    }
  }
  printf("\nNo match to m-sequence found.\n");
}


static uint16_t fibonacci;         /* Fibonacci shift register */

void test_fibshift() {
  int i=0, lag;
  static uint16_t record[SEQLEN+REGLEN+1];
  /* Run the Fibonacci shift register */
  fibonacci = REGLOAD;
  printf("%d -> %d (%s)\n", i, ((REGLOAD & fibonacci) ? 1 : 0), tobinarystring(fibonacci));
  record[0] = fibonacci;
  for (i=1; i<SEQLEN+1; i++) {
    fibonacci = fibshift(fibonacci);
    record[i] = fibonacci;
    printf("%d -> %d (%s)\n", i, ((REGLOAD & fibonacci) ? 1 : 0), tobinarystring(fibonacci));
  }
  for (lag=0; lag<SEQLEN; lag++) {
    if (matchbits(lag, record)) {
      printf("\nFound match to m-sequence at lag = %d\n", lag);
      return;
    }
  }
  printf("\nNo match to m-sequence found.\n");
}


static uint16_t backfibonacci;         /* Fibonacci shift register */

void test_backfibshift() {
  int i, lag;
  static uint16_t record[SEQLEN+REGLEN+1];
  /* Run the Fibonacci shift register */
  backfibonacci = 1;
  i = 0;
  printf("%d -> %d (%s)\n", i, ((REGLOAD & backfibonacci) ? 1 : 0), tobinarystring(backfibonacci));
  record[0] = backfibonacci;
  for (i=1; i<SEQLEN; i++) {
    backfibonacci = backfibshift(backfibonacci);
    record[i] = backfibonacci;
    printf("%d -> %d (%s)\n", i, ((REGLOAD & backfibonacci) ? 1 : 0), tobinarystring(backfibonacci));
  }
  for (lag=0; lag<SEQLEN; lag++) {
    if (matchbits(lag, record)) {
      printf("\nFound match to m-sequence at lag = %d\n", lag);
      return;
    }
  }
  printf("\nNo match to m-sequence found.\n");
}

#endif
