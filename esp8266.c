/*
 * esp8266.c
 *
 *  Created on: 4 Oca 2020
 *      Author: eren
 */
#include <msp430.h>
#include <esp8266.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

char receivedData[20][100] = {};
unsigned int raw = 0;
unsigned int col = 0;
char dataParsed[20];
char payload[20];
unsigned int CRC[2];
uint16_t payloadCRC = 0;
int found = 0;
extern int flag;

void espSerialInit(void)
{
    P1SEL0 |= BIT4 | BIT5 ;                                              // P1.4 = TX    ,    P1.5 = RX

        UCA0CTLW0 |= UCSWRST;
        UCA0CTLW0 |= UCSSEL__SMCLK;

        UCA0BR0 = 6;                                                       // 1MHz SMCLK/9600 BAUD
        UCA0BR1 = 0x00;
        UCA0MCTLW = (0x2200 | UCOS16 | UCBRF_13 );
        UCA0CTLW0 &= ~UCSWRST;
        UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
    __delay_cycles(10000);
}
void espWrite(char *command, char *response, int clearData)
{
    espSendArray(command);
    flag = 0;
    while(wait(response) == 0 && flag == 0);
    TA0CTL |= TACLR;
    if(flag == 1){
        clear();
        espWrite(command,response,clearData);
    }
    if(clearData == 1){
        clear();
    }
}
void espSendArray(char *TxArray)
{
    while(*TxArray)     // loop until binary zero (EOS)
    {
        while(!(UCTXIFG & UCA0IFG));        // Wait for TX buffer to be ready for new data
        UCA0TXBUF = *TxArray++;             // Write the character at the location specified py the pointer
    }
}

void espGetArray(char espData)
{
    receivedData[col][raw++] = espData;
    if(espData == '\r'){
        raw = 0;
        col++;
    }
}

int wait(char *response)     //wait esp return "OK"
{
    unsigned int i;
    for(i = 0; i<20; i++){
        if(strstr(receivedData[i],response) != 0){
            return 1;
        }
    }
    return 0;
}
char *espRead(void)
{
    while(dataParsing() == 0);

    return espData();
}
char *espData(void)
{
    int lenght = dataParsed[1];
    int i;
    char payload[20] = {'\0'};
    int count = 0;
    for(i=2;i<lenght-2;i++){
        payload[count++] = dataParsed[i];
    }
    CRC[0] = dataParsed[lenght-2];
    CRC[1] = dataParsed[lenght-1];

    payloadCRC = (uint16_t)CRC[0] << 8 | (uint16_t)CRC[1];

    return payload;
}
int dataParsing(void){
    unsigned int i;
    unsigned int index = 0;
    char *parse;
    for(i = 0; i<20; i++){
        if(strstr(receivedData[i],"+IPD,") != 0){
            __delay_cycles(20000);
            parse = strstr(receivedData[i],":");
            while(*parse++){
                dataParsed[index++] = *parse;
                __delay_cycles(10000);
            }
            return 1;
        }
    }
    return 0;
}

char *request(int type)
{
    static char requestData[3] = {0xEE, ' ', '\0'};
    /*
     * 01   Temperature
     * 02   Air Pressure
     * 03   Humidity
     * 04   Wind Speed
     * 05   Cloundiness
     * 06   Weather Status
     * 07   Current Time
     * 08   Current Date
     * 09   Uptime
     * 0A   System Temperature
     * 0B   Connected # of Clients
     * FF   Greeting Message
     * */
    switch (type) {
        case 1:
            requestData[1] = 0x01;
            break;
        case 2:
            requestData[1] = 0x02;
            break;
        case 3:
            requestData[1] = 0x03;
            break;
        case 4:
            requestData[1] = 0x04;
            break;
        case 5:
            requestData[1] = 0x05;
            break;
        case 6:
            requestData[1] = 0x06;
            break;
        case 7:
            requestData[1] = 0x07;
            break;
        case 8:
            requestData[1] = 0x08;
            break;
        case 9:
            requestData[1] = 0x09;
            break;
        case 10:
            requestData[1] = 0x0A;
            break;
        case 11:
            requestData[1] = 0x0B;
            break;
        default:
            break;
    }
    return requestData;
}

void clear(void)
{
    unsigned int i=0;
    unsigned int k=0;
    for(i = 0; i<20; i++)
    {
        for(k = 0; k<100; k++)
        {
            receivedData[i][k] = '\0';
        }
    }
    col= 0;
    raw = 0;
}

