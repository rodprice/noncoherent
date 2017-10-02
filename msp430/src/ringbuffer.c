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


#define WRAP(bits) ((bits) & (RINGSIZE-1))

static uint8_t ring[RINGSIZE];

void rbinit(ringbuffer* rb) {
  uint8_t i;
  rb->head = 0;
  rb->tail = 0;
  rb->buffer = ring;
  for (i=0; i<RINGSIZE; i++)
    rb->buffer[i] = ' ';
}

/* Use wrapping trick by assigning to uint8_t variable */
uint8_t rbfull(ringbuffer* rb) {
  uint8_t wrapped;
  wrapped = rb->head - rb->tail;
  return (wrapped == RINGSIZE);
}

uint8_t rbempty(ringbuffer* rb) {
  return ((rb->head - rb->tail) == 0);
}

uint8_t rbput(ringbuffer* rb, uint8_t data) {
  if (!rbfull(rb)) {
    rb->buffer[rb->head & (RINGSIZE-1)] = data;
    rb->head++;
    return 0;    /* success */
  } else {
    return 1;    /* failure */
  }
}

uint8_t rbget(ringbuffer* rb) {
  uint8_t data = -1;  /* indicates failure */
  if (!rbempty(rb)) {
    data = rb->buffer[rb->tail & (RINGSIZE-1)];
    rb->tail++;   /* incrementing tail here avoids need for lock */
  }
  return data;
}

