
/* SPI functions to read and write from the Si4432 radio chip using
 * the MSP430 hardware SPI, USCI B0.  The hardware is set up to use
 * SMCLK running at 1 MHz, divided by 60 to give the SPI clock a 60
 * usec period.  An 8-bit register address is sent first, followed
 * immediately by either an 8-bit read or write.  The Si4430/31/32
 * data sheet has all the gory details. */


#include <stdint.h>
#include <msp430.h>
#include "beacon.h"
#include "util.h"
#include "spi.h"


/* Enable SPI to talk to the radio */
void enable_spi() {
  UCB0CTL1 = UCSWRST;           /* shut down USCI */
  
  /* set up SPI clock */
  UCB0CTL1 |= UCSSEL_2;         /* use SMCLK */
  UCB0BR1 = 0;                  /* don't divide SMCLK */
  UCB0BR0 = 1;
  
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
  P1SEL  |= ( SCK_PIN | SDO_PIN | SDI_PIN );
  P1SEL2 |= ( SCK_PIN | SDO_PIN | SDI_PIN );

  /* ready to go */
  UCB0CTL1 &= ~UCSWRST;
}

/* Write a new value into a register on the radio.  If only one
 * register of the radio is written, a 16-bit value must be sent to
 * the radio (the 8-bit address of the register followed by the new
 * 8-bit value of the register).  To write a register, the MSB of the
 * address is set to 1 to indicate a device write. 
*/
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

  __delay_cycles(600);          /* keep nSEL low past end of SCLK */
  P2OUT |= NSEL_PIN;            /* release Si4432 device */
  EXIT_CRITICAL();              /* restore status register */
}

/* Write a range of registers in the Si4432 */
void spi_burst_write(uint8_t addr, uint8_t *data, uint8_t len) {
  uint8_t i;
  volatile uint8_t value;

  SR_ALLOC();
  ENTER_CRITICAL();             /* disable interrupts */
  P2OUT &= ~NSEL_PIN;           /* select Si4432 device */

  while(!(IFG2 & UCB0TXIFG));   /* wait for last transmission */
  UCB0TXBUF = (addr | 0x80);    /* send register address for write */
  while(!(IFG2 & UCB0RXIFG));   /* wait for receive buffer full */
  value = UCB0RXBUF;            /* clear receive buffer */

  for (i=0; i<len; i++) {
    while(!(IFG2 & UCB0TXIFG)); /* wait for last transmission */
    UCB0TXBUF = data[i];        /* send next byte */
    while(!(IFG2 & UCB0RXIFG)); /* wait for receive buffer full */
    value = UCB0RXBUF;          /* clear receive buffer */
  }

  __delay_cycles(600);          /* keep nSEL low past end of SCLK */
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

  __delay_cycles(600);          /* keep nSEL low past end of SCLK */
  P2OUT |= NSEL_PIN;            /* release Si4432 device */
  EXIT_CRITICAL();              /* restore status register */

  return value;
}

/* Read a range of registers in the Si4432 */
uint8_t* spi_burst_read(uint8_t addr, uint8_t *data, uint8_t len) {
  uint8_t i;

  SR_ALLOC();
  ENTER_CRITICAL();             /* disable interrupts */
  P2OUT &= ~NSEL_PIN;           /* select Si4432 device */

  while(!(IFG2 & UCB0TXIFG));   /* wait for last transmission */
  UCB0TXBUF = (addr & 0x7F);    /* send register address */
  while(!(IFG2 & UCB0RXIFG));   /* wait for receive buffer full */
  data[0] = UCB0RXBUF;          /* read receive buffer */

  for (i=0; i<len; i++) {
    UCB0TXBUF = 0;              /* send dummy value */
    while(!(IFG2 & UCB0RXIFG)); /* wait for receive buffer full */
    data[i] = UCB0RXBUF;          /* read data from receive buffer */
  }

  __delay_cycles(600);          /* keep nSEL low past end of SCLK */
  P2OUT |= NSEL_PIN;            /* release Si4432 device */
  EXIT_CRITICAL();              /* restore status register */

  return data;
} /* unimplemented */
