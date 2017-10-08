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


/* Implements a Galois shift register */
uint16_t galshift(uint16_t bits) {
  uint16_t carry = (bits & 0x01) << (REGLEN-1);
  uint16_t next = carry ? (bits^GPOLY) : bits;
  return carry + (next>>1);
}
