/*
 * UART.h
 *
 *  Created on: Mar 20, 2014
 *      Author: emilylam
 */

#ifndef UART_H_
#define UART_H_

#include <msp430.h>
#include <msp430f5529.h>

// Initialize UART
void uart_initialize();
// Echo RX to TX
void echo();
// Send a byte of data
void SendByte(unsigned char send_byte);
// Debug function, send out strings over UART.
void UARTSendArray(unsigned char *TxArray, unsigned char ArrayLength);
void bcUartInit(void);


#endif /* UART_H_ */
