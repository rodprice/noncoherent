#ifndef UART_H
#define UART_H

#include "ringbuffer.h"


void uart_init(ringbuffer *rb);
void uart_send(char character);
void uart_send_buffer(ringbuffer *rb);
void uart_send_buffer_ln(ringbuffer *rb);
void uart_send_string(char *string, uint8_t len);
void uart_send_string_ln(char *string, uint8_t len);

#endif
