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
    return 0;
  } else {
    return -1;
  }
}

uint8_t rbget(ringbuffer* rb) {
  uint8_t data = -1;
  if (!rbempty(rb)) {
    data = rb->buffer[rb->tail & (RINGSIZE-1)];
    rb->tail++;   /* incrementing tail here avoids need for lock */
  }
  return data;
}

#if DEBUG_RINGBUFFER

#include <stdio.h>

void rbqueue(ringbuffer* rb) {
  int i;
  if (rbempty(rb)) {
    printf("|");
  } else {
    for (i=rb->tail; i<rb->head; i++)
      printf("%c", rb->buffer[WRAP(i)]);
  }
}

void rbprint(ringbuffer* rb) {
  int i;
  printf("[");
  for (i=0; i<RINGSIZE; i++) {
    if ((i == WRAP(rb->tail) && i == WRAP(rb->head)))
      printf("|");
    else if (WRAP(rb->tail) <= WRAP(rb->head)) {
      if ((WRAP(rb->tail) <= i) && (i < WRAP(rb->head)))
        printf("%c", rb->buffer[i]);
      else
        printf(" ");
    } else {
      if ((i < WRAP(rb->head)) || (WRAP(rb->tail) <= i))
        printf("%c", rb->buffer[i]);
      else
        printf(" ");
    }
  }
  printf("]");
}

void rbpointers(ringbuffer* rb) {
  int i;
  printf("[");
  for (i=0; i<RINGSIZE; i++) {
    if ((i == WRAP(rb->tail) && i == WRAP(rb->head)))
      printf("|");
    else if (i == WRAP(rb->tail))
      printf("T");
    else if (i == WRAP(rb->head))
      printf("H");
    else
      printf(" ");
  }
  printf("]");
}

void test_rbqueue() {
  ringbuffer ring;
  rbinit(&ring);
  printf("initial  ... "); rbqueue(&ring); printf("\n");
  rbput(&ring, 'A'); printf("put -> %c ... ",'A'); rbqueue(&ring); printf("\n");
  rbput(&ring, 'B'); printf("put -> %c ... ",'B'); rbqueue(&ring); printf("\n");
  rbput(&ring, 'C'); printf("put -> %c ... ",'C'); rbqueue(&ring); printf("\n");
  rbput(&ring, 'D'); printf("put -> %c ... ",'D'); rbqueue(&ring); printf("\n");
  rbput(&ring, 'E'); printf("put -> %c ... ",'E'); rbqueue(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbqueue(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbqueue(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbqueue(&ring); printf("\n");
  rbput(&ring, 'F'); printf("put -> %c ... ",'F'); rbqueue(&ring); printf("\n");
  rbput(&ring, 'G'); printf("put -> %c ... ",'G'); rbqueue(&ring); printf("\n");
  rbput(&ring, 'H'); printf("put -> %c ... ",'H'); rbqueue(&ring); printf("\n");
  rbput(&ring, 'I'); printf("put -> %c ... ",'I'); rbqueue(&ring); printf("\n");
  rbput(&ring, 'J'); printf("put -> %c ... ",'J'); rbqueue(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbqueue(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbqueue(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbqueue(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbqueue(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbqueue(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbqueue(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbqueue(&ring); printf("\n");
}

void test_rbprint() {
  ringbuffer ring;
  rbinit(&ring);
  printf("initial  ... "); rbprint(&ring); printf("\n");
  rbput(&ring, 'A'); printf("put -> %c ... ",'A'); rbprint(&ring); printf("\n");
  rbput(&ring, 'B'); printf("put -> %c ... ",'B'); rbprint(&ring); printf("\n");
  rbput(&ring, 'C'); printf("put -> %c ... ",'C'); rbprint(&ring); printf("\n");
  rbput(&ring, 'D'); printf("put -> %c ... ",'D'); rbprint(&ring); printf("\n");
  rbput(&ring, 'E'); printf("put -> %c ... ",'E'); rbprint(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbprint(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbprint(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbprint(&ring); printf("\n");
  rbput(&ring, 'F'); printf("put -> %c ... ",'F'); rbprint(&ring); printf("\n");
  rbput(&ring, 'G'); printf("put -> %c ... ",'G'); rbprint(&ring); printf("\n");
  rbput(&ring, 'H'); printf("put -> %c ... ",'H'); rbprint(&ring); printf("\n");
  rbput(&ring, 'I'); printf("put -> %c ... ",'I'); rbprint(&ring); printf("\n");
  rbput(&ring, 'J'); printf("put -> %c ... ",'J'); rbprint(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbprint(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbprint(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbprint(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbprint(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbprint(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbprint(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbprint(&ring); printf("\n");
}

void test_rbpointers() {
  ringbuffer ring;
  rbinit(&ring);
  printf("initial  ... "); rbpointers(&ring); printf("\n");
  rbput(&ring, 'A'); printf("put -> %c ... ",'A'); rbpointers(&ring); printf("\n");
  rbput(&ring, 'B'); printf("put -> %c ... ",'B'); rbpointers(&ring); printf("\n");
  rbput(&ring, 'C'); printf("put -> %c ... ",'C'); rbpointers(&ring); printf("\n");
  rbput(&ring, 'D'); printf("put -> %c ... ",'D'); rbpointers(&ring); printf("\n");
  rbput(&ring, 'E'); printf("put -> %c ... ",'E'); rbpointers(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbpointers(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbpointers(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbpointers(&ring); printf("\n");
  rbput(&ring, 'F'); printf("put -> %c ... ",'F'); rbpointers(&ring); printf("\n");
  rbput(&ring, 'G'); printf("put -> %c ... ",'G'); rbpointers(&ring); printf("\n");
  rbput(&ring, 'H'); printf("put -> %c ... ",'H'); rbpointers(&ring); printf("\n");
  rbput(&ring, 'I'); printf("put -> %c ... ",'I'); rbpointers(&ring); printf("\n");
  rbput(&ring, 'J'); printf("put -> %c ... ",'J'); rbpointers(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbpointers(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbpointers(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbpointers(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbpointers(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbpointers(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbpointers(&ring); printf("\n");
  printf("get -> %c ... ", rbget(&ring)); rbpointers(&ring); printf("\n");
}

#endif
