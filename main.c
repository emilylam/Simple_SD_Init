#include <msp430f5529.h>

#include "sd.h"
#include "spi.h"
#include "UART.h"

#define PERIPH_CLOCKRATE 8000000

static unsigned char sd_buffer[SD_BLOCKSIZE];
static sd_context_t sdc;
int ok;
int i, k;

void main(void)
{

	ok = 0;
	sd_buffer[0] = 0xAA;
	sd_buffer[1] = 0xFF;
	sd_buffer[2] = 0x00;

	// Lat
	sd_buffer[3] = 4;
	sd_buffer[4] = 4;
	sd_buffer[5] = 2;
	sd_buffer[6] = 0;
	sd_buffer[7] = '.';
	sd_buffer[8] = 8;
	sd_buffer[9] = 8;
	sd_buffer[10] = 2;
	sd_buffer[11] = 3;
	sd_buffer[12] = 'N';

	// Lat
	sd_buffer[13] = 0;
	sd_buffer[14] = 7;
	sd_buffer[15] = 1;
	sd_buffer[16] = 0;
	sd_buffer[17] = 6;
	sd_buffer[18] = '.';
	sd_buffer[19] = 3;
	sd_buffer[20] = 4;
	sd_buffer[21] = 3;
	sd_buffer[22] = 1;
	sd_buffer[23] = 'W';

	// Configure trigger bits.
	P8OUT &= ~BIT2;
	P8DIR |= BIT2;

	// Setup
	WDTCTL = WDTPW | WDTHOLD;					// Stop WDT
	uart_initialize();						  	// Setup UART
	spi_initialize();							// Start SPI
	spi_set_divisor(PERIPH_CLOCKRATE/200000);	// Set SCLK < 400kHz


	/* SPI Test -- Comment out when not used.
	 * MOSI connected to MISO. Direct echo.  */
	// spi_send_byte(0x40);

	// Initialize SD Card
	sdc.busyflag = 0;							// Busy Flag
	sdc.timeout_write = PERIPH_CLOCKRATE/8;		// Set Write Clock Rate
	sdc.timeout_read = PERIPH_CLOCKRATE/20;		// Set Read Clock Rate
	ok = init_SD(&sdc);							// Initialize SD
	spi_set_divisor(4);							// Speed up clock

	/* Read in the first block on the SD Card */
	if (ok == 1)
	{
		write_block(&sdc, 0x00, sd_buffer);
		sd_buffer[0] = 0x05;
		read_block(&sdc, 0x00, sd_buffer);
		for (k=0; k < 50; k++) {
			SendByte(sd_buffer[k]);
			for(i=0; i< 200; i++);
			k++;
		}
		read_block(&sdc, 0x01, sd_buffer);
		read_block(&sdc, 0x02, sd_buffer);
		read_block(&sdc, 0x03, sd_buffer);
		read_block(&sdc, 0x04, sd_buffer);
		read_block(&sdc, 0x04, sd_buffer);
	}

	/* Wait forever */
	while (1) { }
}

