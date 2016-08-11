/*
 * sd.c
 *
 *  Created on: Apr 19, 2014
 *      Author: emilylam
 */

#include "sd.h"
#include "sdcomm_spi.h"
#include "spi.h"

static unsigned char argument[4];

/* This function initializes the SD card -- forces it into SPI mode */
int init_SD(sd_context_t *sdc) {

	char i;										// Looping Variables
	unsigned char temp[1];						// Received Byte

	sdc->busyflag = 0;							// Busy Flag -> 0

	temp[0] = 0x00;

	spi_cs_assert();							// CS -> Low

	sd_delay(200);								// Pulse > 74 cycles
	spi_cs_deassert();							// CS -> High
	sd_delay(80);								// Pulse > 16 cycles

	spi_cs_assert();							// CS -> Low

	/* CMD 0 sequence -- This forces the SD card into SPI mode. Needs to be sent very slow.*/
	spi_send_byte(0xFF);						// Dummy Byte
	spi_send_byte(0x40);						// Send CMD0
	for(i=0; i< 10; i++);
	spi_send_byte(0x00);
	for(i=0; i< 10; i++);
	spi_send_byte(0x00);
	for(i=0; i< 10; i++);
	spi_send_byte(0x00);
	for(i=0; i< 10; i++);
	spi_send_byte(0x00);
	for(i=0; i< 10; i++);
	spi_send_byte(0x95);
	for(i=0; i< 10; i++);
	spi_send_byte(0xFF);						// Dummy Byte
	do
	{
		temp[0] = spi_rcv_byte();				// Receive and store
	}
	while (temp[0] != 0x01);					// Repeat until Idle Byte, 0x01, is received

	spi_send_byte(0xFF);						// Dummy Byte
	spi_cs_deassert();							// CS -> High

	sd_delay(80);								// Delay

	for (i=0; i<4; i++)							// Declare empty argument
		argument[i] = 0;

	do
	{
		/* CMD55 sequence */
		spi_cs_assert();						// CS -> Low
		sd_send_command(sdc, CMD55, argument);	// Send CMD55
		do
		{
			temp[0] = spi_rcv_byte();			// Receive and store
		}
		while (temp[0] == 0xFF);				// Try until I get something not 0xFF, want 0x01

		spi_send_byte(0xFF);					// Dummy Byte
		spi_cs_deassert();						// CS -> High

		sd_delay(10);							// Small Delay

		/* CMD41 sequence */
		spi_cs_assert();						// CS -> Low
		sd_send_command(sdc, ACMD41, argument);	// Send ACMD41
		do
		{
			temp[0] = spi_rcv_byte();			// Receive and store
		}
		while (temp[0] == 0xFF);				// Try until I get something not 0xFF, want 0x00
	}
	while (temp[0] != 0x00);

	spi_send_byte(0xFF);						// Dummy Byte
	spi_cs_deassert();							// CS -> High

	sd_delay(80);								// Delay
	for(i=0; i< 10; i++);

	/* CMD16 sequence */
	change_argument(argument, SD_BLOCKSIZE);	// Change argument to blocksize
	spi_cs_assert();							// CS -> Low
	sd_send_command(sdc, CMD16, argument);		// Send CMD16
	do
	{
		temp[0] = spi_rcv_byte();				// Receive and store
	}
	while (temp[0] != 0x00);					// Try until I get 0x00

	spi_send_byte(0xFF);						// Dummy Byte
	spi_cs_deassert();							// CS -> High

	if (temp[0] == 0x00)
		return 1;								// Successfully Initialized! =]
	else
		return 0;								// Did not initialize correctly . . .

}

// Delay code to give the SD Card time to process
void sd_delay(char number)
{
	char i;
	/* Null for now */
	for (i=0; i<number; i++)
	{
		/* Clock out an idle byte (0xFF) */
		spi_send_byte(0xFF);
	}
}

// Debug code to trigger the 'scope externally on rising edge
void trigger(void)
{
	char i;
	P8OUT |= BIT2;
	for(i=0; i< 10; i++);
	P8OUT &= ~BIT2;

}

void sd_send_command(sd_context_t *sdc, unsigned char cmd, unsigned char *argument)
{
	int i;
	spi_send_byte(0xFF);					// Dummy Byte
	spi_send_byte((cmd & 0x3F) | 0x40);		// Command Byte -- Converts CMD to HEX and send
	for (i=3; i>=0; i--)					// Argument -- MSB first
	{
		spi_send_byte(argument[i]);
	}
	spi_send_byte(0xFF);					// Ignore Checksum -- Send Don't Care
	spi_send_byte(0xFF);					// Dummy Byte

}

void change_argument(unsigned char *argument, u32 value)
{
	argument[3] = (unsigned char)(value >> 24);
	argument[2] = (unsigned char)(value >> 16);
	argument[1] = (unsigned char)(value >> 8);
	argument[0] = (unsigned char)(value);
}


// Read/Write stuff

int read_block(sd_context_t *sdc, u32 blockaddr, unsigned char *data)
{
	unsigned long int i = 0;
	unsigned char temp[1];
	/* Adjust the block address to a linear address */
	blockaddr <<= SD_BLOCKSIZE_NBITS;
	/* Wait until any old transfers are finished */
	sd_wait_notbusy (sdc); //<-- fix this later
	/* Pack the address */
	change_argument(argument, blockaddr);

	/* Need to add size checking -- CMD 17 */
	spi_cs_assert();							// CS -> Low
	sd_send_command(sdc, CMD17, argument);		// Send CMD16
	do
	{
		temp[0] = spi_rcv_byte();				// Receive and store
	}
	while (temp[0] != 0x00);					// Try until I get 0x00

	spi_send_byte(0xFF);						// Dummy Byte
	spi_cs_deassert();							// CS -> High


	/* Check for an error, like a misaligned read
	if (response[0] != 0)
	return 0;*/

	/* Re-assert CS to continue the transfer */
	spi_cs_assert();

	/* Wait for the token */

	do
	{
		temp[0] = spi_rcv_byte();
	}
	while (temp[0] != 0xFE);

	for(i=0; i< SD_BLOCKSIZE; i++)
	{
		data[i] = spi_rcv_byte();
	}

	do
	{
		temp[0] = spi_rcv_byte();				// Receive and store
	}
	while (temp[0] != 0xFF);					// Try until I get 0x00

	spi_send_byte(0xFF);						// Dummy Byte
	spi_cs_deassert();							// CS -> High

	shift_align(data);

	return 1;
}

int write_block(sd_context_t *sdc, u32 blockaddr, unsigned char *data)
{
	unsigned long int i = 0;
	unsigned char temp[1];
	/* Adjust the block address to a linear address */
	blockaddr <<= SD_BLOCKSIZE_NBITS;
	/* Wait until any old transfers are finished */
	sd_wait_notbusy (sdc); //<-- fix this later
	/* Pack the address */
	change_argument(argument, blockaddr);

	/* Need to add size checking -- CMD 24 */
	spi_cs_assert();							// CS -> Low
	sd_send_command(sdc, CMD24, argument);		// Send CMD24
	do
	{
		temp[0] = spi_rcv_byte();				// Receive and store
	}
	while (temp[0] != 0x00);					// Try until I get 0x00

	spi_send_byte(0xFF);						// Dummy Byte
	spi_cs_deassert();							// CS -> High

	/* Re-assert CS to continue the transfer */
	spi_cs_assert();

	// Write needs an extra byte before writing
	spi_rcv_byte();

	/* Wait for the token */

	/*do
	{
		temp[0] = spi_rcv_byte();
	}
	while (temp[0] != 0xFE);*/

	for(i=0; i< SD_BLOCKSIZE; i++)
	{
		spi_send_byte(data[i]);
	}

	do
	{
		temp[0] = spi_rcv_byte();				// Receive and store
	}
	while (temp[0] == 0xFF);					// Try until I get 0x00

	do
	{
		temp[0] = spi_rcv_byte();				// Receive and store
	}
	while (temp[0] != 0xFF);					// Try until I get 0x00

	spi_send_byte(0xFF);						// Dummy Byte
	spi_cs_deassert();							// CS -> High

	return 1;
}

void sd_wait_notbusy (sd_context_t *sdc)
{
	if (sdc->busyflag == 1)
	{
		while (spi_rcv_byte() != 0xFF);
		sdc->busyflag = 0;
	}
	/* Deassert CS */
	spi_cs_deassert();
	/* Send some extra clocks so the card can resynchronize on the next
	transfer */
	sd_delay(2);
}

void shift_align(unsigned char *ar)
{
	unsigned char old;
	unsigned char new;
	old = 0x01;
	new = 0x02;
    int i;
    for(i=0; i<SD_BLOCKSIZE; i++)
    {
    	new = ar[i];
    	old = old & 0x03;
    	old = old * 0x40;
    	new = new/0x04;
    	new = old | new;
    	old = ar[i];
    	ar[i] = new;

    }

}

