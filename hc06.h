/*
 * hc06.h
 *
 *  Created on: 8 Oca 2020
 *      Author: eren
 */

#ifndef HC06_H_
#define HC06_H_

void bluetoohSerialInit(void);
void bluetoothWrite(char *command);
void bluetoothSendArray(char TxArray);

#endif /* HC06_H_ */
