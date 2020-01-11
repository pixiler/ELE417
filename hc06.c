/*
 * esp8266.c
 *
 *  Created on: 4 Oca 2020
 *      Author: eren
 */
#include <msp430.h>
#include <hc06.h>

void bluetoohSerialInit(void)
{
    P2SEL0 |= (BIT5 | BIT6);                                        // P2.6 = TX    ,    P2.5 = RX
    UCA1CTLW0 |= UCSWRST;
    UCA1CTLW0 |= UCSSEL__SMCLK;

    UCA1BR0 = 6;
    UCA1BR1 = 0x00;
    UCA1MCTLW = (0x2200 | UCOS16 | UCBRF_13 );
    UCA1CTLW0 &= ~UCSWRST;
}
void bluetoothWrite(char *command)
{

    while(*command != '\0'){
        bluetoothSendArray(*command);
        command++;
    }
    bluetoothSendArray(*command);
}
void bluetoothSendArray(char TxArray)
{

        while(!(UCTXIFG & UCA1IFG));        // Wait for TX buffer to be ready for new data
        UCA1TXBUF = TxArray;             // Write the character at the location specified py the pointer
}


