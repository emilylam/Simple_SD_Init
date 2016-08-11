/*
 * UART.c
 *
 *  Created on: Mar 20, 2014
 *      Author: emilylam
 */


#include <msp430.h>
#include <msp430f5529.h>
#include "UART.h"

/* Initialize and enable the SPI module */
void uart_initialize() {

	P3SEL = BIT3+BIT4;                        // P3.4,5 = USCI_A0 TXD/RXD
	UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	UCA0BR0 = 6;                              // 1MHz 9600 (see User's Guide)
	UCA0BR1 = 0;                              // 1MHz 9600 (see User's Guide)
	UCA0MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0, over sampling
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

}

/* Echo data received on RX pin to the TX pin */
void echo() {
	UCA0TXBUF = UCA0RXBUF;
}


/* Send number of bytes Specified in ArrayLength in the array at using the hardware UART 0*/
void UARTSendArray(unsigned char *TxArray, unsigned char ArrayLength){
 /*
 	Example usage: UARTSendArray("Hello", 5);
 	int data[2]={1023, 235};
 	UARTSendArray(data, 4);
 	Note because the UART transmits bytes it is necessary to send two bytes for each integer
 	hence the data length is twice the array length
 */

	while(ArrayLength--){ 					  // Loop until StringLength == 0 and post decrement
		while(!(UCA0IFG&UCTXIFG)); 			  // Wait for TX buffer to be ready for new data
		UCA0TXBUF = *TxArray; 				  // Write the character at the location specified py the pointer
		TxArray++; 							  // Increment the TxString pointer to point to the next character
	}
}

void SendByte(unsigned char send_byte) {
	UCA0TXBUF = send_byte;
}

void bcUartInit(void)
{
	P4SEL |= BIT4+BIT5;         			  // Configure these pins as TXD/RXD
	UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCA1CTL1 |= UCSSEL_2;                     // SMCLK
	UCA1BR0 = 6;                              // 1MHz 9600 (see User's Guide)
	UCA1BR1 = 0;                              // 1MHz 9600 (see User's Guide)
	UCA1MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0, over sampling
	UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	UCA1IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

}
