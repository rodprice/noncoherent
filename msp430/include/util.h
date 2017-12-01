#ifndef UTIL_H
#define UTIL_H


#include <stdint.h>

/* Macros to disable interrupts in critical sections */
#define SR_ALLOC() uint16_t __sr
#define ENTER_CRITICAL() __sr = _get_interrupt_state(); __disable_interrupt()
#define EXIT_CRITICAL() __set_interrupt_state(__sr); __nop()


#define bool uint8_t            /* not the C99 bool type! */
#define true  1
#define false 0
#define error 0xFF


/* Set or clear bits in mask for port 1 (P1OUT) */
void sendbit_port1(uint8_t bit, uint8_t mask);

/* Set or clear bits in mask for port 2 (P2OUT) */
void sendbit_port2(uint8_t bit, uint8_t mask);


#endif
