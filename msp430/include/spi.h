
/* SPI functions to read and write from the Si4432 radio chip using
 * the MSP430 hardware SPI, USCI B0.  The hardware is set up to use
 * SMCLK running at 1 MHz, divided by 60 to give the SPI clock a 60
 * usec period.  An 8-bit register address is sent first, followed
 * immediately by either an 8-bit read or write.  The Si4430/31/32
 * data sheet has all the gory details. */

#ifndef SPI_H
#define SPI_H

#include <stdint.h>


/* Enable SPI to talk to the radio */
void enable_spi();

/* Read a register from the Si4432 */
uint8_t spi_read_register(uint8_t addr);

/* Read a range of registers in the Si4432 */
uint8_t* spi_burst_read(uint8_t addr, uint8_t *data, uint8_t len);

/* Write to a register in the Si4432 */
void spi_write_register(uint8_t addr, uint8_t data);

/* Write a range of registers in the Si4432 */
void spi_burst_write(uint8_t addr, uint8_t *data, uint8_t len);


#endif
