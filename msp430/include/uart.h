#ifndef UART_H
#define UART_H

#include "ringbuffer.h"

/* Default recv callback; echo character through UART */
void uart_echo(char character);

/* Echo received characters back through the UART */
void uart_recv_echo();

/* Default xmit callback; do nothing */
void uart_stub();

/* Initialize hardware UART */
void uart_init();

/* Stop asynchronous sends and receives */
uint8_t uart_stop();

/* Receive characters into buffer asynchronously */
void uart_recv_buffer(ringbuffer *rb);

/* Transmit characters from buffer asynchronously */
void uart_xmit_buffer(ringbuffer *rb);

/* Send a single character */
void uart_send(char character);

/* Send the entire contents of a buffer; blocks until done */
void uart_send_buffer(ringbuffer *rb);

/* Send an entire buffer, followed by \r\n; blocks until done */
void uart_send_buffer_ln(ringbuffer *rb);

/* Send a string; blocks until done */
void uart_send_string(char *string, uint8_t len);

/* Send a string, followed by \r\n; blocks until done */
void uart_send_string_ln(char *string, uint8_t len);

#endif
