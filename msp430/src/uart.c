
#include <msp430.h>
#include "uart.h"
#include "util.h"


/* 9600 baud 8N1 register values.  See MSP430 user's guide for table. */
#define BR0  104                /* UCA0BR0 */
#define BR1  0                  /* UCA0BR1 */
#define MCTL 2                  /* UCA0MCTL */

/* Callbacks for UART receive and transmit */
extern void (*uart_rx_callback)(char character);
extern void (*uart_tx_callback)();

static ringbuffer *uart_rx_rb;  /* callback recv buffer */
static ringbuffer *uart_tx_rb;  /* callback xmit buffer */

static uint8_t uart_rx_dropped; /* counts dropped characters */

/* Don't do anything */
void uart_stub() {}

/* Echo received characters back through the UART */
void uart_echo(char character) {
  uart_send(character);
  if (character == '\r')
    uart_send('\n');
}

void uart_recv_echo() {
  uart_rx_callback = uart_echo;
  IE2 |= UCA0RXIE;            /* turn on recv interrupt */
}

static void xmit_buffer_callback() {
  if (!rbempty(uart_tx_rb)) {
    UCA0TXBUF = rbget(uart_tx_rb); /* send next character in buffer */   
  } else {
    IE2 &= ~UCA0TXIE;           /* done; turn off xmit interrupts */
  }
}

void uart_xmit_buffer(ringbuffer *rb) {
  uart_tx_rb = rb;
  uart_tx_callback = xmit_buffer_callback;
  if (!rbempty(rb)) {            /* if buffer empty, do nothing */
    while (!(IFG2 & UCA0TXIFG)); /* wait for transmit buffer */
    UCA0TXBUF = rbget(rb);       /* send first character in buffer */   
    IE2 |= UCA0TXIE;             /* enable UCA0 xmit interrupts */
  }
}

static void recv_buffer_callback(char character) {
  if (rbput(uart_rx_rb, character) == error) {
    if (uart_rx_dropped != -1)  /* don't wrap count back to zero */
      uart_rx_dropped++;        /* but count dropped characters */
  }
}

void uart_recv_buffer(ringbuffer *rb) {
  uart_rx_rb = rb;
  uart_rx_callback = recv_buffer_callback;
  if (!rbfull(rb)) {            /* if buffer full, do nothing */
    uart_rx_dropped = 0;        /* no characters dropped yet */
    IE2 |= UCA0RXIE;            /* turn on recv interrupt */
  }
}

void uart_init() {
  UCA0CTL1 |= UCSWRST;          /* hold UART UCA0 in reset */
  UCA0CTL1 |= UCSSEL_2;         /* clock source is SMCLK */
  UCA0BR0 = BR0;                /* set the baud rate */
  UCA0BR1 = BR1;
  UCA0MCTL = MCTL;              /* modulation control */
  UCA0CTL1 &= ~UCSWRST;         /* enable UART */
}

uint8_t uart_stop() {
  IE2 &= ~UCA0RXIE;             /* disable UCA0 recv interrupts */
  IE2 &= ~UCA0TXIE;             /* disable UCA0 xmit interrupts */
  return uart_rx_dropped;
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
