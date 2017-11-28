
#include <msp430.h>
#include "uart.h"
#include "util.h"


/* 9600 baud 8N1 register values.  See MSP430 user's guide for table. */
#define BR0  104                /* UCA0BR0 */
#define BR1  0                  /* UCA0BR1 */
#define MCTL 2                  /* UCA0MCTL */

ringbuffer *uart_rx_rb;         /* UART receive buffer */

void uart_init(ringbuffer *rb) {
  uart_rx_rb = rb;              /* make buffer available to interrupt routine */
  UCA0CTL1 |= UCSWRST;          /* hold UART UCA0 in reset */
  UCA0CTL1 |= UCSSEL_2;         /* clock source is SMCLK */
  UCA0BR0 = BR0;                /* set the baud rate */
  UCA0BR1 = BR1;
  UCA0MCTL = MCTL;              /* modulation control */
  UCA0CTL1 &= ~UCSWRST;         /* enable UART */
  IE2 |= UCA0RXIE;              /* enable UCA0 interrupts */
}

void uart_send(char character) {
  while (!(IFG2 & UCA0TXIFG));  /* wait for transmit buffer */
  UCA0TXBUF = character;        /* send character */
}

void uart_send_buffer(ringbuffer *rb) {
  while (!rbempty(rb))
    uart_send(rbget(rb));
}

void uart_send_buffer_ln(ringbuffer *rb) {
  while (!rbempty(rb))
    uart_send(rbget(rb));
  uart_send('\r');
  uart_send('\n');
}

void uart_send_string(char *string, uint8_t len) {
  int i;
  for (i=0; i<len; i++)
    uart_send(string[i]);
}

void uart_send_string_ln(char *string, uint8_t len) {
  int i;
  for (i=0; i<len; i++)
    uart_send(string[i]);
  uart_send('\r');
  uart_send('\n');
}
