/**
 * Lock-free ring buffer.
 * Rodney Price, Sept 2017
 *
 * Uses the wrapping trick explained in the Simply Embedded blog entry
 * http://www.simplyembedded.org/tutorials/interrupt-free-ring-buffer/
 * I don't need all the machinery in that implementation, so I wrote my
 * own.  Because my target microprocessor is so memory-constrained, I
 * try to be as parsimonious with the stack as possible.
 */

#include "ringbuffer.h"


/* Length of buffer must be a power of two */
ringbuffer rbnew(uint8_t *ring, uint8_t len) {
  ringbuffer rb;
  rb.buffer = ring;
  rb.len = len;
  rbinit(&rb);
  return rb;
}

/* Assumes rb->buffer already points to real buffer */
void rbinit(ringbuffer* rb) {
  uint8_t i;
  rb->head = 0;
  rb->tail = 0;
  for (i=0; i<rb->len; i++)
    rb->buffer[i] = 0;
}

/* Use wrapping trick by assigning to uint8_t variable */
uint8_t rbfull(ringbuffer* rb) {
  uint8_t wrapped;
  wrapped = rb->head - rb->tail;
  return (wrapped == rb->len);
}

uint8_t rbempty(ringbuffer* rb) {
  return ((rb->head - rb->tail) == 0);
}

uint8_t rbput(ringbuffer* rb, uint8_t data) {
  if (!rbfull(rb)) {
    rb->buffer[rb->head & (rb->len-1)] = data;
    rb->head++;
    return 0;
  } else {
    return -1;
  }
}

uint8_t rbget(ringbuffer* rb) {
  uint8_t data = -1;
  if (!rbempty(rb)) {
    data = rb->buffer[rb->tail & (rb->len-1)];
    rb->tail++;   /* incrementing tail here avoids need for lock */
  }
  return data;
}

