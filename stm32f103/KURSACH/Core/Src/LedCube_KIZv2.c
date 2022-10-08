/*
 * LedCube_KIZv2.c
 *
 *  Created on: Mar 10, 2022
 *      Author: Кирилл
 */
#include "LedCube_KIZv2.h"
#include "LedCube_Alphabet.h"
#include "LedCube_UART.h"
#include <string.h>

LEDCUBE_KIZV2_SIZE_Cube Cube;				//Тут хранится состояние куба

LedCube_SymbolsStack Stack[STACK_SIZE];		//Для строк

LedCube_SymbolsString StringNow;

extern uint8_t SymbolsCube[];
extern UART_HandleTypeDef huart1;
//Запись в матрицу о состоянии светодиода осуществляется через написанные функции, иначе
//стоит помнить, что 0 - горит, 1 - не горит
//Необходимая кодировка cp1251
void LedCube_Init()              //first function for working
{
	LedCube_AllLedsOFF();

	memset(&Stack, 0, STACK_SIZE * sizeof(LedCube_SymbolsStack));
	memset(&StringNow, 0, sizeof(LedCube_SymbolsString));

    HAL_GPIO_WritePin(CT_ONE_PORT, CT_ONE_PIN, ON);               //layers can not be chosen

    LedCube_Frame();
}

void LedCube_ChooseLayerTrigger(uint8_t layer)   //Запись в триггер слоя информации о том, какой слой горит
{												 //Не стоит зажигать более одного слоя, плата может сгореть
    switch (layer)
    {
        case FIRSTLAYER:
        {
            HAL_GPIO_WritePin(IW_ONE_PORT, IW_ONE_PIN, OFF);
            HAL_GPIO_WritePin(IW_TWO_PORT, IW_TWO_PIN, ON);
            HAL_GPIO_WritePin(IW_THREE_PORT, IW_THREE_PIN, ON);
            HAL_GPIO_WritePin(IW_FOUR_PORT, IW_FOUR_PIN, ON);
            HAL_GPIO_WritePin(IW_FIVE_PORT, IW_FIVE_PIN, ON);
            break;
        }
        case SECONDLAYER:
        {
            HAL_GPIO_WritePin(IW_ONE_PORT, IW_ONE_PIN, ON);
            HAL_GPIO_WritePin(IW_TWO_PORT, IW_TWO_PIN, OFF);
            HAL_GPIO_WritePin(IW_THREE_PORT, IW_THREE_PIN, ON);
            HAL_GPIO_WritePin(IW_FOUR_PORT, IW_FOUR_PIN, ON);
            HAL_GPIO_WritePin(IW_FIVE_PORT, IW_FIVE_PIN, ON);
            break;
        }
        case THIRDLAYER:
        {
            HAL_GPIO_WritePin(IW_ONE_PORT, IW_ONE_PIN, ON);
            HAL_GPIO_WritePin(IW_TWO_PORT, IW_TWO_PIN, ON);
            HAL_GPIO_WritePin(IW_THREE_PORT, IW_THREE_PIN, OFF);
            HAL_GPIO_WritePin(IW_FOUR_PORT, IW_FOUR_PIN, ON);
            HAL_GPIO_WritePin(IW_FIVE_PORT, IW_FIVE_PIN, ON);
            break;
        }
        case FOURTHLAYER:
        {
            HAL_GPIO_WritePin(IW_ONE_PORT, IW_ONE_PIN, ON);
            HAL_GPIO_WritePin(IW_TWO_PORT, IW_TWO_PIN, ON);
            HAL_GPIO_WritePin(IW_THREE_PORT, IW_THREE_PIN, ON);
            HAL_GPIO_WritePin(IW_FOUR_PORT, IW_FOUR_PIN, OFF);
            HAL_GPIO_WritePin(IW_FIVE_PORT, IW_FIVE_PIN, ON);
            break;
        }
        case FIFTHLAYER:
        {
            HAL_GPIO_WritePin(IW_ONE_PORT, IW_ONE_PIN, ON);
            HAL_GPIO_WritePin(IW_TWO_PORT, IW_TWO_PIN, ON);
            HAL_GPIO_WritePin(IW_THREE_PORT, IW_THREE_PIN, ON);
            HAL_GPIO_WritePin(IW_FOUR_PORT, IW_FOUR_PIN, ON);
            HAL_GPIO_WritePin(IW_FIVE_PORT, IW_FIVE_PIN, OFF);
            break;
        }
        default:
        {
//            lck_4_Reset();
            break;
        }
    }
}

void LedCube_Frame()								 //Frame - кадр, то есть прогон всех пяти слоев
{
    for(int layer = FIRSTLAYER; layer < LEDCUBE_SIZE; ++layer)
    {
        LedCube_AutoSynchLayer(layer);
    }
}

void LedCube_AutoSynchLayer(uint8_t layer)            // 1/5 frame
{
    if(layer > FIFTHLAYER || layer < FIRSTLAYER)
    {
    	layer = FIRSTLAYER;
        //несуществующий слой
        //choosenTrigger = FIRSTLAYER;
        //reset, error
    }

    LedCube_ResetColumn();									//
    LedCube_Synch(CHOOSELAYERTRIGGER);
    for(int column = FIRSTCOLUMN; column < LEDCUBE_SIZE; ++column)
    {
    	LedCube_ResetColumn();
        LedCube_Synch(column);
    }
    LedCube_ChooseLayerTrigger(layer);
    LedCube_Synch(CHOOSELAYERTRIGGER);

    for(int column = FIRSTCOLUMN; column < LEDCUBE_SIZE; ++column)
    {
        LedCube_WriteColumn(column, layer);
        LedCube_Synch(column);
    }
}

void LedCube_WriteColumn(uint8_t column, uint8_t layer)          //Column == 1-5 triggers
{
    HAL_GPIO_WritePin(IW_ONE_PORT, IW_ONE_PIN, Cube.LedStatus[FIRSTROW][column][layer]);
    HAL_GPIO_WritePin(IW_TWO_PORT, IW_TWO_PIN, Cube.LedStatus[SECONDROW][column][layer]);
    HAL_GPIO_WritePin(IW_THREE_PORT, IW_THREE_PIN, Cube.LedStatus[THIRDROW][column][layer]);
    HAL_GPIO_WritePin(IW_FOUR_PORT, IW_FOUR_PIN, Cube.LedStatus[FOURTHROW][column][layer]);
    HAL_GPIO_WritePin(IW_FIVE_PORT, IW_FIVE_PIN, Cube.LedStatus[FIFTHROW][column][layer]);
}

void LedCube_ResetColumn()          //Обнуление шины для избегания "перескакивания" на следующий слой
{
    HAL_GPIO_WritePin(IW_ONE_PORT, IW_ONE_PIN, ON);
    HAL_GPIO_WritePin(IW_TWO_PORT, IW_TWO_PIN, ON);
    HAL_GPIO_WritePin(IW_THREE_PORT, IW_THREE_PIN, ON);
    HAL_GPIO_WritePin(IW_FOUR_PORT, IW_FOUR_PIN, ON);
    HAL_GPIO_WritePin(IW_FIVE_PORT, IW_FIVE_PIN, ON);
}

void LedCube_Synch(uint8_t choosenTrigger)      //Запись на дешифратор для подачи синхры на выбранный триггер слоя
{
    HAL_GPIO_WritePin(CT_ONE_PORT, CT_ONE_PIN, ON);            //layers can not be chosen
    switch (choosenTrigger)
    {
        case FIRSTCOLUMN:
        {
            HAL_GPIO_WritePin(CT_TWO_PORT, CT_TWO_PIN, OFF);
            HAL_GPIO_WritePin(CT_THREE_PORT, CT_THREE_PIN, OFF);
            HAL_GPIO_WritePin(CT_FOUR_PORT, CT_FOUR_PIN, ON);
            break;
        }
        case SECONDCOLUMN:
        {
            HAL_GPIO_WritePin(CT_TWO_PORT, CT_TWO_PIN, OFF);
            HAL_GPIO_WritePin(CT_THREE_PORT, CT_THREE_PIN, ON);
            HAL_GPIO_WritePin(CT_FOUR_PORT, CT_FOUR_PIN, OFF);
            break;
        }
        case THIRDCOLUMN:
        {
            HAL_GPIO_WritePin(CT_TWO_PORT, CT_TWO_PIN, OFF);
            HAL_GPIO_WritePin(CT_THREE_PORT, CT_THREE_PIN, ON);
            HAL_GPIO_WritePin(CT_FOUR_PORT, CT_FOUR_PIN, ON);
            break;
        }
        case FOURTHCOLUMN:
        {
            HAL_GPIO_WritePin(CT_TWO_PORT, CT_TWO_PIN, ON);
            HAL_GPIO_WritePin(CT_THREE_PORT, CT_THREE_PIN, OFF);
            HAL_GPIO_WritePin(CT_FOUR_PORT, CT_FOUR_PIN, OFF);
            break;
        }
        case FIFTHCOLUMN:
        {
            HAL_GPIO_WritePin(CT_TWO_PORT, CT_TWO_PIN, ON);
            HAL_GPIO_WritePin(CT_THREE_PORT, CT_THREE_PIN, OFF);
            HAL_GPIO_WritePin(CT_FOUR_PORT, CT_FOUR_PIN, ON);
            break;
        }
        case CHOOSELAYERTRIGGER:
        {
            HAL_GPIO_WritePin(CT_TWO_PORT, CT_TWO_PIN, ON);
            HAL_GPIO_WritePin(CT_THREE_PORT, CT_THREE_PIN, ON);
            HAL_GPIO_WritePin(CT_FOUR_PORT, CT_FOUR_PIN, OFF);
            break;
        }
        default:
        {
//добавить ошибку
            break;
        }
    }
    HAL_GPIO_WritePin(CT_ONE_PORT, CT_ONE_PIN, OFF);              //layers can be chosen, запуск синхры
    HAL_GPIO_WritePin(CT_ONE_PORT, CT_ONE_PIN, ON);               //layers can not be chosen
}

void LedCube_WriteLed(uint8_t row, uint8_t column, uint8_t layer, uint8_t state)
{
    Cube.LedStatus[row][column][layer] = LedCube_ZeroOne(state);
}

uint8_t LedCube_ZeroOne(uint8_t state)		//из 1 делает 0, а из 0 - 1
{
    state = (state + 1) % 2;
    return state;
}

void LedCube_AllLedsON()
{
	memset(&Cube, OFF, sizeof(Cube));   //Для включения необходимо подать ноль
}

void LedCube_AllLedsOFF()
{
	memset(&Cube, ON, sizeof(Cube));    //Для выключения необходимо подать единицу
}

void LedCube_StartString(uint16_t mode)
{
	StringNow.TurnOffStringFlag = 0;
	int symbolNo = 0;
	while(StringNow.TurnOffStringFlag == 0)				//Для выхода из функции надо изменить значение этой глобальной переменной
	{
		LedCube_TurnSymbol(symbolNo, mode);
		LedCube_Delay(TIME_TO_NEW_SYMBOL);
		++symbolNo;
		if(symbolNo >= StringNow.Length)
		{
			symbolNo = 0;
			LedCube_Test5();
		}
	}
}

void LedCube_TurnSymbol(uint16_t symbolNo, uint8_t symbolMode)			//Зажигает в определенном режиме символ из строки символов
{
	LedCube_AllLedsOFF();

	switch(symbolMode)
	{
	case SYMBOL_STANDART:
	{
		uint16_t layer = 2;		//Центр

		for(int column = 0; column < LEDCUBE_SIZE; column++)
		{
			for(int row = 0; row < LEDCUBE_SIZE; row++)
			{
				if(StringNow.SymbolsString[symbolNo][row][column] == ON)
				{
					LedCube_WriteLed(row, column, layer, ON);
				}
			}
		}

		break;
	}
	case SYMBOL_FIVE_STANDART:
	{
		for(int column = 0; column < LEDCUBE_SIZE; column++)
		{
			for(int layer = 0; layer < LEDCUBE_SIZE; layer++)
			{
				if(StringNow.SymbolsString[symbolNo][layer][column] == ON)
				{
					for(int row = 0; row < LEDCUBE_SIZE; ++row)
					{
						LedCube_WriteLed(layer, column, row, ON);
					}
				}
			}
		}
		break;
	}
	default:
	{
		break;
	}
	}
}

void LedCube_FillStack(uint8_t* string, uint16_t mode)			//Заполнение стэка
{
	if(strlen(string) < STRING_SIZE)
	{
		if(StringNow.FilliableStackElement >= STACK_SIZE)						//При переполнении заполнение начнется с нулевого элемента
		{
			StringNow.FilliableStackElement = 0;
		}

		memset(&Stack[StringNow.FilliableStackElement], 0, sizeof(LedCube_SymbolsStack));

		strcpy(Stack[StringNow.FilliableStackElement].String, string);
		Stack[StringNow.FilliableStackElement].Length = strlen(string);
		Stack[StringNow.FilliableStackElement].Mode = mode;

		StringNow.Length = Stack[StringNow.FilliableStackElement].Length;
		StringNow.NumberOfString = StringNow.FilliableStackElement;

		++StringNow.FilliableStackElement;										//Инкремент, для автоматического выбора следующего элемента
	}
}

void LedCube_FillCubeString(uint8_t* string)
{
	for(int i = 0; i < strlen(string); ++i)
	{
		LedCube_FillOneSymbolString(string[i], i);
	}
	StringNow.Length = strlen(string);
	StringNow.TurnOffStringFlag = 1;
}

void LedCube_FillOneSymbolString(uint16_t symbol, uint16_t symbolNo)			//Заполнение строки символов в виде матрицы
{
	int symbolInAlphabet = LEDCUBE_SIZE * LEDCUBE_SIZE * LedCube_FindSymbol(symbol); //Сразу умножаю на 25 номер символа, чтобы перевести одномерный массив в двумерный

	for(int layer = 0; layer < LEDCUBE_SIZE; layer++)
	{
		int symbolLayer = LEDCUBE_SIZE * layer;				//Аналогично с symbolNo

		for(int column = 0; column < LEDCUBE_SIZE; column++)
		{
			StringNow.SymbolsString[symbolNo][layer][column] = SymbolsCube[symbolInAlphabet + symbolLayer + column];
		}
	}
}

void LedCube_Test1()
{
	  int set = ON;
	  LedCube_AllLedsOFF();
	  for(int z = 0; z < 5; ++z)
		  for(int y = 0; y < 5; ++y)
			  for(int x = 0; x < 5; ++x)
			  {
				  LedCube_WriteLed(x, y, z, set);
				  LedCube_Delay(20);
			  }
}

void LedCube_Test2()
{
	  int set = OFF;
	  for(int z = 0; z < 5; ++z)
		  for(int y = 0; y < 5; ++y)
			  for(int x = 0; x < 5; ++x)
	  		  {
	  			  LedCube_WriteLed(x, y, z, set);
	  			  LedCube_Delay(20);
	  		  }
}

void LedCube_Test3()
{
	int delay = 10;
	for(int i = 0; i < 50; ++i)
	{
		LedCube_AllLedsON();
		LedCube_Delay(delay);
		LedCube_AllLedsOFF();
		LedCube_Delay(delay);
		delay += 2;
	}
}

void LedCube_AllTests()
{
    LedCube_Test1();
    LedCube_Test2();
    LedCube_Test3();
}

void LedCube_Test4()
{
	int delay = 10;
	for(int i = 0; i < 20; ++i)
	{
		LedCube_AllLedsON();
		LedCube_Delay(delay);
		LedCube_AllLedsOFF();
		LedCube_Delay(delay);
		delay += 2;
	}
}

void LedCube_Test5()
{
	LedCube_AllLedsOFF();
	int delay = 5;
	for(int z = 5; z > 0; --z)
	{
		LedCube_WriteLed(0, 0, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(0, 1, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(0, 2, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(0, 3, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(0, 4, z-1, ON);
		LedCube_Delay(delay);

		LedCube_WriteLed(1, 4, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(2, 4, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(3, 4, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(4, 4, z-1, ON);
		LedCube_Delay(delay);

		LedCube_WriteLed(4, 3, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(4, 2, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(4, 1, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(4, 0, z-1, ON);
		LedCube_Delay(delay);

		LedCube_WriteLed(4, 0, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(3, 0, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(2, 0, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(1, 0, z-1, ON);
		LedCube_Delay(delay);

		LedCube_WriteLed(1, 1, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(1, 2, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(1, 3, z-1, ON);
		LedCube_Delay(delay);

		LedCube_WriteLed(2, 3, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(3, 3, z-1, ON);
		LedCube_Delay(delay);

		LedCube_WriteLed(3, 2, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(3, 1, z-1, ON);
		LedCube_Delay(delay);

		LedCube_WriteLed(2, 1, z-1, ON);
		LedCube_Delay(delay);
		LedCube_WriteLed(2, 2, z-1, ON);
		LedCube_Delay(delay);
	}
}

void LedCube_Delay(int delay)
{
	for(int i = 0; i < delay * 1000; ++i);
}




void LedCube_SetRowColumn(uint8_t* plane, uint16_t layer)
{
	for(int row = 0; row < LEDCUBE_SIZE; ++row)
	{
		for(int column = 0; column < LEDCUBE_SIZE; ++column)
		{
			Cube.LedStatus[row][column][layer] = LedCube_ZeroOne(plane[(row * LEDCUBE_SIZE) + column]);
		}
	}
}

void LedCube_SetRowLayer(uint8_t* plane, uint16_t column)
{
	for(int row = 0; row < LEDCUBE_SIZE; ++row)
	{
		for(int layer = 0; layer < LEDCUBE_SIZE; ++layer)
		{
			Cube.LedStatus[row][column][layer] = LedCube_ZeroOne(plane[(row * LEDCUBE_SIZE) + layer]);
		}
	}
}

void LedCube_SetColumnLayer(uint8_t* plane, uint16_t row)
{
	for(int column = 0; column < LEDCUBE_SIZE; ++column)
	{
		for(int layer = 0; layer < LEDCUBE_SIZE; ++layer)
		{
			Cube.LedStatus[row][column][layer] = LedCube_ZeroOne(plane[(column * LEDCUBE_SIZE) + layer]);
		}
	}
}
