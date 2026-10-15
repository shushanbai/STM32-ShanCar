#include "stm32f10x.h"                  // Device header


void Servo_PWM_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1500;		//CCR
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM3, ENABLE);
	
	/*计算公式	CK_PSC=72MHZ
	PWM频率：	Freq = CK_PSC / (PSC + 1) / (ARR + 1)
	PWM占空比：	Duty = CCR / (ARR + 1)
	PWM分辨率：	Reso = 1 / (ARR + 1)
	*/
}

void Servo_PWM_SetCompare(uint16_t x)//封装成函数，方便调用更改CCR，进而更改占空比
{
	TIM_SetCompare4(TIM3 , x);//单独改变CCR
}


/**
  * @brief  设定舵机角度
  * @param  你想设定的角度（Angle）
  * @retval 无
  */
void Servo_SetAngle(float Angle)
{
	Servo_PWM_SetCompare(500 + Angle * 2000 / 180);	//500~2500
}
