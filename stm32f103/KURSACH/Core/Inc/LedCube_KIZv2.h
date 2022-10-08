/*
 * LedCube_KIZv2.h
 *
 *  Created on: Mar 10, 2022
 *      Author: Кирилл
 */
#include <main.h>
#ifndef INC_LEDCUBE_KIZV2_H_
#define INC_LEDCUBE_KIZV2_H_

//IW - Info way
#define IW_ONE_PIN      ONE_IW_Pin
#define IW_ONE_PORT     ONE_IW_GPIO_Port
#define IW_TWO_PIN      TWO_IW_Pin
#define IW_TWO_PORT     TWO_IW_GPIO_Port
#define IW_THREE_PIN    THREE_IW_Pin
#define IW_THREE_PORT   THREE_IW_GPIO_Port
#define IW_FOUR_PIN     FOUR_IW_Pin
#define IW_FOUR_PORT    FOUR_IW_GPIO_Port
#define IW_FIVE_PIN     FIVE_IW_Pin
#define IW_FIVE_PORT    FIVE_IW_GPIO_Port
//CT - Chosen trigger
#define CT_ONE_PIN      ONE_CT_Pin
#define CT_ONE_PORT     ONE_CT_GPIO_Port
#define CT_TWO_PIN      TWO_CT_Pin
#define CT_TWO_PORT     TWO_CT_GPIO_Port
#define CT_THREE_PIN    THREE_CT_Pin
#define CT_THREE_PORT   THREE_CT_GPIO_Port
#define CT_FOUR_PIN     FOUR_CT_Pin
#define CT_FOUR_PORT    FOUR_CT_GPIO_Port

#define LEDCUBE_SIZE          5
//define name triggers
#define FIRSTCOLUMN           0
#define SECONDCOLUMN          1
#define THIRDCOLUMN           2
#define FOURTHCOLUMN          3
#define FIFTHCOLUMN           4
#define CHOOSELAYERTRIGGER    5

#define FIRSTLAYER            0
#define SECONDLAYER           1
#define THIRDLAYER            2
#define FOURTHLAYER           3
#define FIFTHLAYER            4

#define FIRSTROW              0
#define SECONDROW             1
#define THIRDROW              2
#define FOURTHROW             3
#define FIFTHROW              4

#define ON                    GPIO_PIN_SET
#define OFF                   GPIO_PIN_RESET

#define STRING_SIZE			  100				//Максимальный размер принятой строки
#define STACK_SIZE		      20				//Максимальное количество хранимых строк

#define TIME_TO_NEW_SYMBOL	  500

typedef struct {
	uint8_t String[STRING_SIZE];		//Строка с символами
	uint16_t Length;					//Количество символов в данной строке
	uint16_t Mode;
} LedCube_SymbolsStack;

typedef struct
{
	uint8_t SymbolsString[STRING_SIZE][LEDCUBE_SIZE][LEDCUBE_SIZE];			//Строка, в символами для куба
	uint16_t NumberOfString;												//Номер строки в очереди
	uint16_t Length;														//Количество символов в данной строке
	uint16_t NextSymbol;													//Номер следующего символа для зажигания
	uint16_t FilliableStackElement;											//Номер элемента, который следующий заполняется в стэке
	uint8_t TurnOffStringFlag;												//0 - Продолжать работу функции LedCube_StartString(), 1 - выключение функции
} LedCube_SymbolsString;

typedef struct {
uint8_t     LedStatus[LEDCUBE_SIZE][LEDCUBE_SIZE][LEDCUBE_SIZE];
uint8_t     Queue;
//uint32_t  TimeFrameToFrame;
} LEDCUBE_KIZV2_SIZE_Cube;

/*
typedef struct {
uint16_t     row;
uint16_t     column;
uint16_t	 layer;
} LEDCUBE_Coordinate;
*/

typedef enum
{
	SYMBOL_STANDART,			//Плоская посередине
	SYMBOL_FIVE_STANDART,		//"Толстая буква"
}LedCube_SymbolMode;

/***********************FUNCTIONS**********************************/

/***********************FOR***USER**********************************/
void LedCube_Init();
void LedCube_Frame();

void LedCube_WriteLed(uint8_t row, uint8_t column, uint8_t layer, uint8_t status);
//void lck_4_Reset();
//void lck_4_ChangeString(uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2, uint8_t status);
void LedCube_AllLedsON();
void LedCube_AllLedsOFF();

void LedCube_StartString(uint16_t mode);

void LedCube_Test1();
void LedCube_Test2();
void LedCube_Test3();
void LedCube_Test4();
void LedCube_Test5();
void LedCube_AllTests();
void LedCube_Delay(int delay);

void LedCube_SetRowColumn(uint8_t* plane, uint16_t layer);
void LedCube_SetRowLayer(uint8_t* plane, uint16_t column);
void LedCube_SetColumnLayer(uint8_t* plane, uint16_t row);

/***********************FOR***USER**********************************/

/***********************INTERNAL**********************************/
uint8_t LedCube_ZeroOne(uint8_t state);
//void LedCube_TurnLedsInCourse(LEDCUBE_Coordinate * queue);
void LedCube_ResetColumn();
void LedCube_WriteColumn(uint8_t column, uint8_t layer);
void LedCube_AutoSynchLayer(uint8_t layer);
void LedCube_Synch(uint8_t choosenLayer);

void LedCube_TurnSymbol(uint16_t symbolNo, uint8_t symbolMode);
void LedCube_FillStack(uint8_t* string, uint16_t mode);
void LedCube_FillCubeString(uint8_t* string);
void LedCube_FillOneSymbolString(uint16_t symbol, uint16_t symbolNo);
/***********************INTERNAL**********************************/
#endif /* INC_LEDCUBE_KIZV2_H_ */

