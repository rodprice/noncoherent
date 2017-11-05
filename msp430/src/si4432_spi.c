
/* SPI functions to read and write from the Si4432 radio chip using
 * the MSP430 hardware SPI, USCI B0.  The hardware is set up to use
 * SMCLK running at 1 MHz, divided by 60 to give the SPI clock a 60
 * usec period.  An 8-bit register address is sent first, followed
 * immediately by either an 8-bit read or write.  The Si4430/31/32
 * data sheet has all the gory details.
 *
 * Rod Price, Nov 2017 */


#include <msp430.h>

#include "util.h"
#include "si4432_spi.h"


/* Set up the B0 USCI to do SPI transfers */
void spi_init() {
  __delay_cycles(240000);       /* wait at least 15 ms */
  
  /* set up SPI */
  UCB0CTL1 = UCSWRST;           /* shut down USCI */
  
  /* set up SPI clock */
  UCB0CTL1 |= UCSSEL_2;         /* use SMCLK */
  UCB0BR1 = 0;                  /* divide SMCLK by 60 */
  UCB0BR0 = 60;
  
  /* running in 3-pin SPI because Si4432 wants 16-bit words */
  UCB0CTL0 = (           
    UCCKPH   |                  /* rising edge captures data */
    UCMSB    |                  /* MSB first */
    UCMODE_0 |                  /* 3-pin SPI */
    UCMST    |                  /* use the USCI clock */
    UCSYNC   );                 /* synchronous mode */
  
  /* set up SPI pins */
  P2OUT |= NSEL_PIN;            /* make sure nSEL is high to start */
  P2DIR |= NSEL_PIN;
  P1SEL  |= ( SCLK_PIN | SOMI_PIN | SIMO_PIN );
  P1SEL2 |= ( SCLK_PIN | SOMI_PIN | SIMO_PIN );

  /* ready to go */
  UCB0CTL1 &= ~UCSWRST;
}


/* Write a new value into a register on the radio.  If only one
 * register of the radio is written, a 16-bit value must be sent to
 * the radio (the 8-bit address of the register followed by the new
 * 8-bit value of the register).  To write a register, the MSB of the
 * address is set to 1 to indicate a device write. */

void spi_write_register(uint8_t reg, uint8_t data) {
  volatile uint8_t value;       /* stop compiler from optimizing */
  
  SR_ALLOC();
  ENTER_CRITICAL();             /* disable interrupts */
  P2OUT &= ~NSEL_PIN;           /* select Si4432 device */

  while(!(IFG2 & UCB0TXIFG));   /* wait for last transmission done */
  UCB0TXBUF = (reg | 0x80);     /* send register address */
  while(!(IFG2 & UCB0RXIFG));   /* wait for receive buffer full */
  value = UCB0RXBUF;            /* read receive buffer */
  UCB0TXBUF = data;             /* send data to register */
  while(!(IFG2 & UCB0RXIFG));   /* wait for receive buffer full */
  value = UCB0RXBUF;            /* read receive buffer */

  __delay_cycles(60);           /* keep nSEL low past end of SCLK */
  P2OUT |= NSEL_PIN;            /* release Si4432 device */
  EXIT_CRITICAL();              /* restore status register */
}


/* Read one register from the radio.  When reading a single register
 * of the radio, a 16 bit value must be sent to the radio (the 8-bit
 * address of the register followed by a dummy 8-bit value).  The
 * radio provides the value of the register during the second byte of
 * the SPI transaction.  Note that it is crucial to clear the MSB of
 * the register address to indicate a read cycle.
 */

uint8_t spi_read_register(uint8_t reg) {
  volatile uint8_t value;

  SR_ALLOC();
  ENTER_CRITICAL();             /* disable interrupts */
  P2OUT &= ~NSEL_PIN;           /* select Si4432 device */

  while(!(IFG2 & UCB0TXIFG));   /* wait for last transmission */
  UCB0TXBUF = (reg & 0x7F);     /* send register address */
  while(!(IFG2 & UCB0RXIFG));   /* wait for receive buffer full */
  value = UCB0RXBUF;            /* read receive buffer */
  UCB0TXBUF = 0;                /* send dummy value */
  while(!(IFG2 & UCB0RXIFG));   /* wait for receive buffer full */
  value = UCB0RXBUF;            /* read data from receive buffer */

  __delay_cycles(60);           /* keep nSEL low past end of SCLK */
  P2OUT |= NSEL_PIN;            /* release Si4432 device */
  EXIT_CRITICAL();              /* restore status register */

  return value;
}
