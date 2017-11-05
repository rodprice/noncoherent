/* Utility functions used throughout */

#include "util.h"
#include <msp430.h>

inline void sendbit_port1(uint8_t bit, uint8_t mask) {
  if (bit) {
    P1OUT |= mask;
  } else {
    P1OUT &= ~mask;
  }
}

inline void sendbit_port2(uint8_t bit, uint8_t mask) {
  if (bit) {
    P2OUT |= mask;
  } else {
    P2OUT &= ~mask;
  }
}
