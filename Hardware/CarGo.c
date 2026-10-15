#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "Delay.h"

int16_t L=100;
int16_t R=-100;//Speed


void Car_Init(void)
{
	Motor_Init();
}

void Go_Ahead(void)
{
	Motor_SetSpeed(L,R);
}

void Go_Back(void)
{
	Motor_SetSpeed(-L,-R);
}

void Turn_Left(void)
{
	Motor_SetSpeed(0,R);
}


void Turn_Right(void)
{
	Motor_SetSpeed(L,0);
}

void Self_Left(void)
{
	Motor_SetSpeed(-L,R);
}


void Self_Right(void)
{	
	Motor_SetSpeed(L,-R);
}


void Car_Stop(void)
{
	Motor_SetSpeed(0,0);
}
