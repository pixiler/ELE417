/*
 * esp8266.h
 *
 *  Created on: 4 Ocak 2020
 *      Author: eren
 */

#ifndef ESP8266_H_
#define ESP8266_H_


void espSendArray(char TxArray[]);
void espSerialInit(void);
void espGetArray(char espData);
int wait(char response[]);
int dataParsing(void);
void clear(void);
char *request(int type);
void espWrite(char *command, char *response, int clearData);
char *espData(void);

#endif /* ESP8266_H_ */
