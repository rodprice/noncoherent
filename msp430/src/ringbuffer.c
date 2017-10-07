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


/* create variable __sr on the stack */
#define SR_ALLOC() uint16_t __sr
/* save the status register and disable interrupts */
#define ENTER_CRITICAL() __sr = _get_interrupt_state(); __disable_interrupt()
/* restore the saved value of the status register */
#define EXIT_CRITICAL() __set_interrupt_state(__sr)


/* len must be a power of two, or bad things will happen. */
ringbuffer rbnew(uint8_t* buf, size_t len) {
  uint8_t i;
  ringbuffer rb;
  rb.buffer = buf;
  rb.length = len;
  rbinit(&rb);
  return rb;
}

/* This has to be atomic, if rbget or rbput might be running concurrently.  */
void rbinit(ringbuffer* rb) {
  SR_ALLOC();
  ENTER_CRITICAL();
  rb->head = 0;
  rb->tail = 0;
  for (i=0; i<len; i++)
    buf[i] = 0;
  EXIT_CRITICAL();
}

/* ------------------------------------------------------------------------ */
/* This ring buffer is supposed to be lock-free, but just in case... */

#if DEBUG

inline uint8_t rbfull(ringbuffer* rb) {
  uint8_t diff;
  SR_ALLOC();
  ENTER_CRITICAL();
  diff = (rb->head - rb->tail) == rb->length);
  EXIT_CRITICAL();
  return diff;
}

inline uint8_t rbempty(ringbuffer* rb) {
  uint8_t diff;
  SR_ALLOC();
  ENTER_CRITICAL();
  diff = (rb->head - rb->tail) == 0);
  EXIT_CRITICAL();
  return diff;
}

uint8_t rbput(ringbuffer* rb, uint8_t data) {
  uint8_t diff;
  SR_ALLOC();
  ENTER_CRITICAL();
  diff = (rb->head - rb->tail);
  if (diff != rb->length) {
    rb->buffer[diff & (rb->length-1)] = data;
    rb->head++;
  }
  EXIT_CRITICAL();
  return diff == rb->length;  /* 0 if success, 1 if failure */
}

uint8_t rbget(ringbuffer* rb) {
  uint8_t diff, data;
  data = 0;
  SR_ALLOC();
  ENTER_CRITICAL();
  diff = (rb->head - rb->tail);
  if (diff != 0) {
    data = rb->buffer[diff & (rb->length-1)];
    rb->tail++;   /* incrementing tail here avoids need for lock */
  }
  EXIT_CRITICAL();
  return data;
}

#else

inline uint8_t rbfull(ringbuffer* rb) {
  return (rb->head - rb->tail) == rb->length;
}

inline uint8_t rbempty(ringbuffer* rb) {
  return rb->head == rb->tail;
}

uint8_t rbput(ringbuffer* rb, uint8_t data) {
  if (!rbfull(rb)) {
    rb->buffer[(rb->head++ - rb->tail) & (rb->length-1)] = data;
    return 0;                   /* success */
  } else {
    return 1;                   /* failure */
  }
}

uint8_t rbget(ringbuffer* rb) {
  if (!rbempty(rb))
    return rb->buffer[(rb->head - rb->tail++) & (rb->length-1)];
  else
    return -1;                  /* failure */
}

/* uint8_t rbput(ringbuffer* rb, uint8_t data) { */
/*   if (!rbfull(rb)) { */
/*     rb->buffer[(rb->head - rb->tail) & (rb->length-1)] = data; */
/*     rb->head++; */
/*     return 0; */
/*   } else { */
/*     return 1; */
/*   } */
/* } */

/* uint8_t rbget(ringbuffer* rb) { */
/*   uint8_t data; */
/*   data = 0; */
/*   if (!rbempty(rb)) { */
/*     data = rb->buffer[(rb->head - rb->tail) & (RINGSIZE-1)]; */
/*     rb->tail++;   /\* incrementing tail here avoids need for lock *\/ */
/*   } */
/*   return data; */
/* } */

#endif
