#include "stm32f10x.h"                  // Device header
#include "OLED.h"    
#include "CarGo.h"
#include "Serial.h"
#include "Delay.h"


/**
  * @brief  初始化循迹模块端口
  * @param  无
  * @retval 无
  */
  
void Track_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	Car_Init();
	OLED_Init();
	Serial_Init();//蓝牙初始化
}




/**
  * @brief  各端口接受信号
  * @param  无
  * @retval 无
  */
uint8_t S1_State(void)
{
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
}

uint8_t S2_State(void)
{
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
}

uint8_t S3_State(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
}

uint8_t S4_State(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
}

