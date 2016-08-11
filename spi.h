#ifndef SPI_H
#define SPI_H

#include <msp430.h>
#include <msp430f5529.h>

void spi_initialize();
void spi_set_divisor(unsigned int divisor);
void spi_cs_assert();
void spi_cs_deassert();
void spi_send_byte(unsigned char input);
unsigned char spi_rcv_byte();
void spi_enable();
void spi_disable();

#endif

