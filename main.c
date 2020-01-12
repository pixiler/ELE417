#include <msp430.h> 
#include <esp8266.h>
#include <hc06.h>
#include <crc16modbus.h>
#include <ssd1306_lib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <icon.h>

/**
 * main.c
 */

#define led0 BIT0
#define led1 BIT1
     volatile unsigned int validArr[12] ={ 0};
unsigned char data;
volatile int lenght = 0;
extern int CRC[2];
uint16_t cal_crc;
extern uint16_t payloadCRC;
int valid;
int count_req = 1;

char * printString;
char arr[30];
volatile int i,j,k;

char  * payload;
char bluetoothPayload[100] = {'\0'};
void GPIO_init(void);
int validData(char *payload);
void espServer(void);
void oled(char str[],int x,int y);
void sendMail(void);


int flag = 0;
int countTimer = 0;
char MessageErr[10] = "DATA ERROR";


int main(void)
{
    //WDTCTL = WDTPW + WDTCNTCL + WDTTMSEL;
    WDTCTL = WDTPW | WDTHOLD;
    //WDTCTL = WDTPW | WDTCNTCL;
	PM5CTL0 &= ~LOCKLPM5;       //Disable the GPIO power-on default high-impedance mode
	                                    //to activate previously configured port settings

	ssd1306Init();//call at the begining once
	fillDisplay (0x00);//you can use this function to clear screen

	bluetoohSerialInit();
	espSerialInit();
	GPIO_init();

     __bis_SR_register(GIE);  // intruppt enable
     //espWrite("AT+RST\r\n", "invalid",1);

     espWrite("ATE0\r\n", "OK",1);

     espWrite("AT+CWJAP=\"ELE417\",\"EmbeddeD\"\r\n", "OK", 1);

     espWrite("AT+CIPSTART=\"TCP\",\"10.50.106.10\",10000\r\n","OK",1);

     espWrite("AT+CIPSEND=2\r\n", ">", 0);

     espWrite("hi","SEND OK",1);


while(1){
     for(count_req; count_req<12; count_req++){
         fillDisplay (0x00);//you can use this function to clear screen
         espWrite("AT+CIPSEND=2\r\n", ">", 0);
         espWrite(request(count_req), "SEND OK", 0);


         payload = (char *)espRead();
         valid = validData(payload);

         if(validData(payload)){
             oled(payload,6,0);
             strcat(bluetoothPayload,payload);
             strcat(bluetoothPayload,"|");
         }

         else{
             oled(MessageErr,6,0);
            strcat(bluetoothPayload,MessageErr);
            strcat(bluetoothPayload,"|");
         }

         switch (count_req) {
            case 1:
                drawImage(88, 24, 40, 40, temperature, 0);
                break;
            case 2:
                drawImage(88, 24, 40, 40, meter, 0);
                break;
            case 3:
                drawImage(88, 24, 40, 40, humidity, 0);
                break;
            case 4:
                drawImage(88, 24, 40, 40, windy, 0);
                break;
            case 5:
                drawImage(88, 24, 40, 40, climate, 0);
                break;
            case 6:
                drawImage(88, 24, 40, 40, snowy, 0);
                break;
            case 7:
                drawImage(88, 24, 40, 40, time_date, 0);
                break;
            case 8:
                drawImage(88, 24, 40, 40, scheduling, 0);
                break;
            case 9:
                drawImage(88, 24, 40, 40, uptime, 0);
                break;
            case 10:
                drawImage(88, 24, 40, 40, fan, 0);
                break;
            case 11:
                drawImage(88, 24, 40, 40, segmentation, 0);
                break;

            default:
                break;
        }


     clear();
     __delay_cycles(500000);
     }
     count_req = 1;
     bluetoothPayload[strlen(bluetoothPayload)-1] = '\n';
     bluetoothWrite(bluetoothPayload);
     int clr_bluetooth;
         for(clr_bluetooth = 0; clr_bluetooth<100; clr_bluetooth++)
         {

             bluetoothPayload[clr_bluetooth] = '\0';

         }
}
*/
	return 0;
}
#pragma vector = USCI_A0_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    switch(UCA0IV)
    {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
        data = UCA0RXBUF;                     // read the received char - also clears Interrupt
        espGetArray(data);                           // Process the received char
        break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
    }
}
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void){

    countTimer++;
    if(countTimer == 60){
        countTimer = 0;
        flag = 1;
    }
    TA0CTL &= ~TAIE;
}

void GPIO_init(void)
{
    P1OUT &= ~(BIT0 | BIT1);    // Clear P1.0
    P1DIR |= BIT0 | BIT1;       // RED = 0, GREEN = 1
    P1OUT &= ~BIT6;
    P1DIR |= BIT6;
    __delay_cycles(500000);
    P1OUT |= BIT6;
    __delay_cycles(300000);

    TA0CCTL0 = CCIE;

    TA0CTL = TASSEL_2 | MC_2 | TACLR | TAIE;
}
int validData(char *payload)
{
    cal_crc = calculateCRC((uint8_t *)payload,strlen(payload));
    if(payloadCRC == cal_crc){
        return 1;
    }
    return 0;
}

void oled(char* str,int x,int y){//first input is string, second chose between 6x8 and 12x16 punto(enter 6 or 12),third input is line where to write
    if (x==6){
    if (strlen(str) <=20){
       //sprintf (printString, "%s", str);
        draw6x8Str(0,  y, str, 1, 0);
        //drawImage(10, 10, 40, 40, climate, 0);
    }
    else if(strlen(str) >20){
        for (j=0;j<(strlen(str)-20);j++) {
            char arr[20] = "";
            for(i=j;i<j+20; i++) {
                arr[i-j] = str[i];
            }
            //sprintf (printString, "%s", arr);
            draw6x8Str(0,  y, str, 1, 0);
        }
    }
    }

    else if(x==12){
        if (strlen(str) <=10){
                sprintf (printString, "%s", str);
                draw12x16Str(0,  y, printString, 1);
            }
            else if(strlen(str) >10){
                for (j=0;j<(strlen(str)-10);j++) {
                    char arr[9] = "";
                    for(i=j;i<j+10; i++) {
                        arr[i-j] = str[i];
                    }
                    sprintf (printString, "%s", arr);
                    draw12x16Str(0,  y, printString,  1);
                }
            }
    }

}
