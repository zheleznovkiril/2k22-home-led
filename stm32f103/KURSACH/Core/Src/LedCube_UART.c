/*
 * LedCube_UART.c
 *
 *  Created on: 24 Apr 2022
 *      Author: Кирилл
 */
//#include <main.h>
#include "LedCube_UART.h"
LEDCUBE_KIZV2_Uart Uart;
extern UART_HandleTypeDef huart1;

void LedCube_UartInit()
{
	LedCube_UartResetString();
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, Uart.RxData, RX_BUFF_SIZE);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart->Instance == USART1)
    {
        char TxData[STRING_SIZE];
        uint8_t endForTransmit[] = {'\r', '\0'};
    	//HAL_UART_Transmit(&huart1, (uint8_t *)TxData, 9, 1);
    	uint16_t uartErrCode = LedCube_CheckingUartString();
    	if(LedCube_CheckingUartString() != UART_OK)
    	{
    		switch(uartErrCode)
    		{
    			case UART_TOO_BIG_STRING:
    			{
    				LedCube_UartPrintString("Too big string!");
    				break;
    			}
    			case UART_EMPTY_STRING:
    			{
    				LedCube_UartPrintString("Received string is empty!");
    				break;
    			}
    			case UART_UNUSUAL_NUMBER_OF_QUOTE:
      			{
      				LedCube_UartPrintString("Unusual number of quote!");
    				break;
    			}
    			case UART_UNCORRECT_FORMAT:
      			{
      				LedCube_UartPrintString("Uncorrect format!");
    				break;
    			}
    			default:
    			{
    				LedCube_UartPrintString("Something wrong!");
    				break;
    			}
    		}

        	HAL_UARTEx_ReceiveToIdle_IT(&huart1, Uart.RxData, RX_BUFF_SIZE);
    		return;
    	}

    	LedCube_FindCommand();
    	LedCube_FindString();

    	LedCube_FillStack(Uart.RxString, Uart.Command);
    	LedCube_FillCubeString(Uart.RxString);

    	LedCube_UartPrintString(Uart.RxData);
    	LedCube_UartPrintString(Uart.RxString);

    	LedCube_UartResetString();
        HAL_UARTEx_ReceiveToIdle_IT(&huart1, Uart.RxData, RX_BUFF_SIZE);
    }
}

void LedCube_UartResetString()
{
	memset(&Uart.RxString, END_OF_STRING, sizeof(&Uart.RxString));
	memset(&Uart.RxData, END_OF_STRING, sizeof(&Uart.RxData));
	Uart.StringOrigin = ZERO;
	Uart.StringEnd    = ZERO;
}

int LedCube_DoCommand()
{
	int uartErrCode = LedCube_CheckingUartString();
	if(uartErrCode != UART_OK)
	{
		return uartErrCode;
	}

	switch (2)//LedCube_FindCommand())
	{
		case UART_SYMBOL:
			return LedCube_PrintSymbol();
			break;
	}

	return UART_OK;
}

int	LedCube_PrintSymbol()
{

	return UART_OK;
}


void LedCube_FindCommand()				//Запись номера команды
{
	char commandStr[3] = {};

	commandStr[1] = Uart.RxData[1];
	commandStr[2] = Uart.RxData[2];
	commandStr[3] = '\0';

	Uart.Command = atoi(commandStr);

}

void LedCube_FindString()
{
	int startOfString = 5;
	for(int i = startOfString; i < RX_BUFF_SIZE; i++)
	{
		Uart.RxString[i - startOfString] = Uart.RxData[i];
		if(Uart.RxData[i] == 13)
		{
			Uart.RxString[i - startOfString - 1] = '\0';
			break;
		}
	}
}

int LedCube_CheckingUartString()			//Проверка всевозможных ошибок в полученной строке
{
	int placeOfEnd = Uart.StringOrigin;			//Место конца строки
	int quoteCount = ZERO;					//Счетчик количества кавычек

	if(Uart.RxData[0] != '"')					//Проверка, что первый символ - кавычка
	{
		return UART_UNCORRECT_FORMAT;
	}

	for(; placeOfEnd < RX_BUFF_SIZE; ++placeOfEnd)
	{
		if('\0' == Uart.RxData[placeOfEnd])		//Проверка заканчивается ли строка
		{
			break;
		}

		if('"' == Uart.RxData[placeOfEnd])		//Счетчик количества кавычек
		{
			++quoteCount;
		}
	}

	if(ZERO == placeOfEnd)
	{
		LedCube_UartResetString();
		return UART_EMPTY_STRING;			// Пришла пустая строка
	}

	if(RX_BUFF_SIZE == placeOfEnd)
	{
		LedCube_UartResetString();
		return UART_TOO_BIG_STRING;			// Не встретился конец строки - слишком большая строка
	}

	if(1 == (quoteCount % 2))
	{
		LedCube_UartResetString();
		return UART_UNUSUAL_NUMBER_OF_QUOTE; //Проверка на четность количества кавычек
	}

	for(int i = 1; i < 3; ++i)						//После первой ковычки должно идти две цифры
	{
		if(Uart.RxData[i] > '9' || Uart.RxData[i] < '0')
		{
			return UART_UNCORRECT_FORMAT;
		}
	}

	if(Uart.RxData[3] != '"')							//После двух цифр должна идти кавычка
	{
		return UART_UNCORRECT_FORMAT;
	}

	Uart.StringOrigin = ZERO;
	Uart.StringEnd    = placeOfEnd;
	return UART_OK;
}

void LedCube_UartPrintNumber(uint16_t number)
{
	uint8_t string[10] = {};
	itoa(number, string, 10);
	LedCube_UartPrintString(string);
}

void LedCube_UartPrintString(uint8_t * string)
{
	uint8_t TxString[STRING_SIZE] = {};
	uint8_t endForTransmit[3] = {13, 10, '\0'};				//сделай перенос строки
	strcpy(TxString, string);
	strcat(TxString, endForTransmit);
	HAL_UART_Transmit(&huart1, (uint8_t *)string, strlen(string), 10);
}
