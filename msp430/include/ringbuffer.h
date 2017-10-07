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
#include "config.h"

/* See "Writing lock-free code: a corrected queue" in Dr Dobbs at
   http://www.drdobbs.com/parallel/writing-lock-free-code-a-corrected-queue/210604448 */

/* See also http://www.1024cores.net/home/relacy-race-detector, a C++
   library for exhaustive testing of (simple) multithreaded code.  Or
   use Alloy, http://alloy.mit.edu, a model checker. */

/* The MSP430 is a 16-bit machine, and so overflows of uint16_t's
   naturally wrap around.  Differences between head and tail, even
   when head has wrapped and tail hasn't, are always correct, since
   head is always greater than or equal to tail.  This is the wrapping
   trick referred to above.  It allows us to forgo the use of a
   counter to keep track of the number of elements in the buffer, and
   therefore to avoid having two threads accessing the same variable.
   Then, if we pay attention to the ordering of reads/writes and
   pointer increments, the ring buffer can be lock-free.  I think. */


typedef struct rbuf {
  const uint8_t* buffer;
  const size_t length;
  volatile uint16_t head;
  volatile uint16_t tail;
} ringbuffer;


/* Make a new ring buffer */
ringbuffer rbnew(uint8_t* buf, size_t len);

/* Initialize a ring buffer */
void rbinit(ringbuffer* rb);

/* Returns 1 if the buffer is full, 0 otherwise */
inline uint8_t rbfull(ringbuffer* rb);

/* Returns 1 if the buffer is empty, 0 otherwise */
inline uint8_t rbempty(ringbuffer* rb);

/* Gets the head of the ring buffer, returns 0 if empty */
uint8_t rbget(ringbuffer* rb);

/* Put a byte into the buffer; returns 0 if successful, 1 otherwise */
uint8_t rbput(ringbuffer* rb, uint8_t data);


#endif
