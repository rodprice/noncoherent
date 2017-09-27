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
static const uint8 lut[256] = 
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
  return lut4[nib3] + lut4[nib2] + lut4[nib1] + lut4[nib0];
}
#endif

/* Count the number of ones in a 16-bit word */
#if ((REGISTER_LENGTH > 8) && (LUT_SIZE == 256))
uint8_t count(uint16_t word) {
  uint8_t hi = (word & 0xFF00) >> 8;
  uint8_t lo =  word & 0x00FF;
  return lut8[hi] + lut8[lo];
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
  return (carry << (REGLEN-1)) + (bits >> 1);
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
