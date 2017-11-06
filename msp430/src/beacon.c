
#include <msp430.h>
#include <stdint.h>
#include <stddef.h>
#include "beacon.h"
#include "util.h"


extern volatile uint32_t clock; /* increments every two seconds */


/* Read calibration data from TLV, verify checksum */
/* http://www.simplyembedded.org/tutorials/msp430-configuration/ */
static int _verify_cal_data(void)
{
    uint8_t len = 31;
    uint16_t *data = (uint16_t *) 0x10c2;
    uint16_t crc = 0;
 
    while (len-- > 0) {
        crc ^= *(data++);
    }
 
    return (TLV_CHECKSUM + crc);
}


/* Set up MSP430 clocks, watchdog */
void configure_clocks() {
  WDTCTL = WDTPW + WDTHOLD;    /* hold the watchdog */
  DCOCTL = 0;                  /* set DCO clock to lowest setting */
  BCSCTL1 = CALBC1_1MHZ;       /* set MCLK to 1 MHz from cal data */
  DCOCTL = CALDCO_1MHZ;        /* set DCOCLK to 1 MHz from cal data */
  /* BCSCTL1 = CALBC1_8MHZ;       /\* set MCLK to 8 MHz from cal data *\/ */
  /* DCOCTL = CALDCO_8MHZ;        /\* set DCOCLK to 8 MHz from cal data *\/ */
  /* BCSCTL2 = DIVS_3;            /\* set SMCLK to 1 MHZ (divide by 8) *\/ */
  BCSCTL3 |= LFXT1S_0 | XCAP_3; /* ACLK from 32768 Hz crystal, 12.5 pF */
  __delay_cycles(500000);      /* wait for 32768 Hz oscillator to start */
}

/* Set up MSP430 I/O pins */
void configure_pins() {
  /* Port 1 pins */
  P1OUT = 0;                   /* all pins set low */
  P1DIR =         (            /* BUTTON_PIN and SDO_PIN are inputs */
    XMIT_LED_PIN  |            /* transmit light off */
    READY_LED_PIN |            /* power/ready light off */
    TXON_PIN      |            /* transmit command off */
    RXON_PIN      |            /* receive command off */
    SCK_PIN       |            /* SPI SCLK pin set low */
    SDI_PIN       );           /* SPI SIMO pin set low */
  /* Port 2 pins */
  P2OUT = NSEL_PIN;            /* nSEL high, SDN low */
  P2DIR = NSEL_PIN | SDN_PIN;  /* all other pins are inputs */
}

/* Enable interrupts on the nIRQ pin */
void enable_nirq() {
  P2IES |= NIRQ_PIN;            /* high-to-low requests interrupt */
  P2IFG &= ~NIRQ_PIN;           /* clear flag, just in case */
  P2IE |= NIRQ_PIN;             /* enable interrupt on pin */
}

/* Enable transmit clock interrupts */
void enable_clock_irq() {
  P2DIR |= GPIO1_PIN;           /* pin drives Si4432 tx data */
  P2DIR &= ~GPIO0_PIN;          /* pin accepts Si4423 tx clock */
  P2IES |= GPIO0_PIN;           /* high-to-low requests interrupt */
  P2IFG &= ~GPIO0_PIN;          /* clear flag, just in case */
  P2IE |= GPIO0_PIN;            /* enable interrupt on pin */  
}

/* Start the timer, reset and start the clock */
void timer_start() {
  TACCR0 = 0;                   /* stop the timer */
  TACTL |= TACLR;               /* set timer count to 0 */
  clock = 0;                    /* reset the clock */
  /* do something with TAIE to enable interrupts */
  TACTL = TASSEL0 | ID0 | MC_2 | TACLR; /* timer source ACLK, continuous mode */
}

/* Stop the timer and pause the clock */
void timer_stop() {
  TACCR0 = 0;                    /* stop the timer */
  TACTL |= TACLR;                /* set TAR to 0 */
}


/* SPI functions to read and write from the Si4432 radio chip using
 * the MSP430 hardware SPI, USCI B0.  The hardware is set up to use
 * SMCLK running at 1 MHz, divided by 60 to give the SPI clock a 60
 * usec period.  An 8-bit register address is sent first, followed
 * immediately by either an 8-bit read or write.  The Si4430/31/32
 * data sheet has all the gory details. */

/* Enable SPI to talk to the radio */
void enable_spi() {
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

/* Read a range of registers in the Si4432 */
uint8_t* spi_burst_read(uint8_t addr, uint8_t len) {
  return NULL;
} /* unimplemented */

/* Write a range of registers in the Si4432 */
void spi_burst_write(uint8_t addr, uint8_t *data, uint8_t len) {
} /* unimplemented */


int main(int argc, char *argv[])
{
  configure_clocks();
  configure_pins();
  enable_nirq();
  enable_spi();

  si4432_reset();
  si4432_check_device();
  si4432_configure_gpio();
  si4432_set_frequency();
  si4432_init_rx_modem();
  si4432_init_tx_modem();

  __nop();
  __enable_interrupt();
  while (1) {
    LPM3;                       /* sleep */
  }
}
