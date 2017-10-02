#ifndef MSEQUENCE_H
#define MSEQUENCE_H

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

/*
 * Irreducible polynomials
 *
 * A polynomial is written x^n + c_{n-1} x^{n-1} + ... + c_1 x + c_0,
 * where n is the degree of the polynomial.  In the following, 
 * numbers are base 10, and represent the polynomial coefficients
 * c_{n-1} to c_0, written as a binary number.
 *
 * n = 4  {3,9};                      (all possible)
 * n = 5  {05,09,15,23,27,29};        (all possible)
 * n = 6  {03,27,33,39,45,51};        (all possible)
 * n = 7  {                           (all possible)
 *   003,009,015,017,029,039,043,057,063,
 *   065,075,083,085,101,111,113,119,125};
 * n = 8  {                           (all possible)
 *   029,043,045,077,095,099,101,105,
 *   113,135,141,169,195,207,231,245};
 * n = 10 {009,111,231,825,945};      (only a few)
 * n = 12 {0235,0473,0705,1073,1971}; (only a few)
 *
 * Each block below contains #define's that define the sequence that
 * follows.  The polynomials used are the first ones in the lists
 * above.
 */


#include "config.h"


/* 7-bit sequence */
#if (REGISTER_LENGTH == 3)
#define REGLEN 3
#define SEQLEN 7
#define POLY 3
#define RPOLY 6
#define GPOLY 5
#define REGLOAD 4
#endif

/* 15-bit sequence */
#if (REGISTER_LENGTH == 4)
#define REGLEN 4
#define SEQLEN 15
#define POLY 3
#define RPOLY 12
#define GPOLY 9
#define REGLOAD 8
#endif

/* 31-bit sequence */
#if (REGISTER_LENGTH == 5)
#define REGLEN 5
#define SEQLEN 31
#define POLY 5
#define RPOLY 20
#define GPOLY 9
#define REGLOAD 16
#endif

/* 63-bit sequence */
#if (REGISTER_LENGTH == 6)
#define REGLEN 6
#define SEQLEN 63
#define POLY 3
#define RPOLY 48
#define GPOLY 33
#define REGLOAD 32
#endif

/* 127-bit sequence */
#if (REGISTER_LENGTH == 7)
#define REGLEN 7
#define SEQLEN 127
#define POLY 3
#define RPOLY 96
#define GPOLY 65
#define REGLOAD 64
#endif

/* 255-bit sequence */
#if (REGISTER_LENGTH == 8)
#define REGLEN 8
#define SEQLEN 255
#define POLY 29
#define RPOLY 184
#define GPOLY 113
#define REGLOAD 128
#endif

/* 1023-bit sequence */
#if (REGISTER_LENGTH == 10)
#define REGLEN 10
#define SEQLEN 1023
#define POLY 9
#define RPOLY 576
#define GPOLY 129
#define REGLOAD 512
#endif

/* 4095-bit sequence */
#if (REGISTER_LENGTH == 12)
#define REGLEN 12
#define SEQLEN 4095
#define POLY 235
#define RPOLY 3440
#define GPOLY 2785
#endif


/* Implements a Galois shift register */
uint16_t galshift(uint16_t bits);


#endif
