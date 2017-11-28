/**
 * Lock-free ring buffer.
 * Rodney Price, Sept 2017
 *
 * Uses the wrapping trick from the Simply Embedded blog entry
 * http://www.simplyembedded.org/tutorials/interrupt-free-ring-buffer/
 * I don't need all the machinery in that implementation, so I wrote my own.
 */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>


typedef struct rbuf {
  uint8_t* buffer;
  uint8_t len;
  volatile uint8_t head;
  volatile uint8_t tail;
} ringbuffer;

/* Makes a new ringbuffer and initializes it */
ringbuffer rbnew(uint8_t* ring, uint8_t len);

/* Initializes a ringbuffer */
void rbinit(ringbuffer* rb);

/* Returns 1 if the buffer is full, 0 otherwise */
uint8_t rbfull(ringbuffer* rb);

/* Returns 1 if the buffer is empty, 0 otherwise */
uint8_t rbempty(ringbuffer* rb);

/* Gets the head of the ring buffer, -1 if empty */
uint8_t rbget(ringbuffer* rb);

/* Puts a new uint8_t at the tail of the ring buffer */
uint8_t rbput(ringbuffer* rb, uint8_t data);


#endif
