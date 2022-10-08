/*
 * LedCube_UART.h
 *
 *  Created on: 24 Apr 2022
 *      Author: Кирилл
 */
#include <main.h>
#include "stdlib.h"
#include "LedCube_KIZv2.h"

#ifndef INC_LEDCUBE_UART_H_
#define INC_LEDCUBE_UART_H_

#define RX_BUFF_SIZE 		  STRING_SIZE + 6

#define END_OF_STRING		  '\0'
#define ZERO				  0
#define ONE					  1

typedef enum
{
	UART_OK,
    UART_TOO_BIG_STRING,
	UART_EMPTY_STRING,
	UART_UNUSUAL_NUMBER_OF_QUOTE,
	UART_UNCORRECT_FORMAT,
}LedCube_UartErrors;

typedef enum
{
	UART_SYMBOL,
}LedCube_UartCommands;

typedef struct
{
	uint8_t StringOrigin;
	uint8_t StringEnd;
	uint8_t Command;
	uint8_t RxData[RX_BUFF_SIZE];
	uint8_t	RxString[STRING_SIZE];
} LEDCUBE_KIZV2_Uart;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
void LedCube_UartInit();
void LedCube_UartResetString();
int LedCube_DoCommand();
int	LedCube_PrintSymbol();
void LedCube_FindCommand();
void LedCube_FindString();
void LedCube_UartPrintString(uint8_t * string);
void LedCube_UartPrintNumber(uint16_t number);

#endif /* INC_LEDCUBE_UART_H_ */
