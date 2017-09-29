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


int main() {
  run_through_the_lookup_table();
  test_tock();
  return 0;
}
