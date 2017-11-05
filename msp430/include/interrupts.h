#ifndef INTERRUPTS_H
#define INTERRUPTS_H


#include <msp430.h>


/* M-sequence generation */
__attribute__((interrupt(TIMER0_A0_VECTOR))) void mseq_isr(void);

/* Morse code generation and clock */
__attribute__((interrupt(TIMER0_A1_VECTOR))) void morse_isr(void);

/* nIRQ interrupts */
__attribute__((interrupt(PORT2_VECTOR))) void port2_isr(void);


#endif
