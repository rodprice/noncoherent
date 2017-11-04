#ifndef UTIL_H
#define UTIL_H


/* Macros to disable interrupts in critical sections */
#define SR_ALLOC() uint16_t __sr
#define ENTER_CRITICAL() __sr = _get_interrupt_state(); __disable_interrupt()
#define EXIT_CRITICAL() __set_interrupt_state(__sr)



#endif
