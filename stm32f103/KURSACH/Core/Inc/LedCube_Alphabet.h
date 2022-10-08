/*
 * LedCube_Alphabet.h
 *
 *  Created on: 1 May 2022
 *      Author: Кирилл
 */

#ifndef INC_LEDCUBE_ALPHABET_H_
#define INC_LEDCUBE_ALPHABET_H_

#include <main.h>

#define NUMBER_OF_SYMBOLS	  44
#define ON                    GPIO_PIN_SET
#define OFF                   GPIO_PIN_RESET
#define LEDCUBE_SIZE          5
#define SYMBOL_LENGHT		  LEDCUBE_SIZE*LEDCUBE_SIZE

uint16_t LedCube_FindSymbol(uint16_t symbol);
void LedCube_GiveSymbol(uint8_t ** arrayForSymbol, uint16_t symbol);

#endif /* INC_LEDCUBE_ALPHABET_H_ */
