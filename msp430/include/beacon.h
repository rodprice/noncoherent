#ifndef CONFIG_H
#define CONFIG_H

#include <msp430.h>
#include <stdint.h>


/**********************************************************************/
/* main */

/* Port 1 pins */
#define XMIT_LED_PIN  BIT0
#define READY_LED_PIN BIT1
#define BUTTON_PIN    BIT2
#define TXON_PIN      BIT3
#define RXON_PIN      BIT4
#define SCK_PIN       BIT5
#define SDO_PIN       BIT6
#define SDI_PIN       BIT7

/* Port 2 pins */
#define GPIO0_PIN     BIT0
#define GPIO1_PIN     BIT1
#define GPIO2_PIN     BIT2
#define NSEL_PIN      BIT3
#define NIRQ_PIN      BIT4
#define SDN_PIN       BIT5

/* Synonyms */
#define XMIT_CLOCK_PIN GPIO0_PIN /* transmit clock from radio */
#define XMIT_DATA_PIN  GPIO1_PIN /* transmit data to radio */
#define RECV_DATA_PIN  GPIO2_PIN /* receive data from radio */


/**********************************************************************/
/* Radio output */

/* Generates an FM tone by toggling the frequency of the transmitter
   at fixed intervals.  An FSK transmitter sends one frequency for a
   one and another for a zero, so we send ones for a TONE_PERIOD,
   then zeros for a TONE_PERIOD, and so on.  The resulting modulation
   is a square wave, but using GFSK rather than FSK should soften
   that into something approximating a pure audio tone. */

#define AUDIO_TICKS 4           /* 4096/(2*4) = 512 Hz */


/**********************************************************************/
/* m sequence */

/* Defines the bit rate of the m-sequence generator.  This sets
   TIMER_A to request an interrupt after MSEQ_TICKS of ACLK.  If ACLK
   is 32768 Hz and MSEQ_TICKS is 8, then an interrupt occurs 4096
   times per second.  A new bit of the m-sequence is sent at every
   interrupt call.  Given these settings, sending a 4095-bit
   m-sequence will take just less than 1 sec. */
#define MSEQ_TICKS 8            /* 0.25 ms */

/* Defines the number of m-sequences periods to be transmitted */
#define PERIODS 64

/* Defines the number of bits in the shift register.  Choices
   currently are 3, 4, 5, 6, 7, 8, 10, and 12. */
#define REGISTER_LENGTH 8


/**********************************************************************/
/* morse code */

/* Defines the time unit of the Morse code generator.  A Morse code
   rate of 5 wpm corresponds to a time unit of 240 ms.   */
#define MORSE_TICKS 256          /* 6 ms */

#define MESSAGE "AD0YX sending m-sequence    "


/**********************************************************************/
/* ring buffer */

/* Number of characters in the buffer. Must be a power of 2. */
#define RINGSIZE 8



/**********************************************************************/
/* Debugging flags */

/* #define DEBUG_MAIN */
/* #define DEBUG_MORSE */
/* #define DEBUG_RINGBUFFER */
/* #define DEBUG_MSEQUENCE */


/* Set up MSP430 clocks, watchdog */
void configure_clocks();

/* Set up MSP430 I/O pins */
void configure_pins();

/* Enable interrupts on the nIRQ pin */
void enable_nirq();

/* Enable transmit clock interrupts */
void enable_clock_irq();

/* Start the timer, reset and start the clock */
void timer_start();

/* Stop the timer and pause the clock */
void timer_stop();


#endif
