#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "string.h"

#include "CarGo.h"
	
void ShowPacket(void)
{
	OLED_ShowString(1, 1, "Hello");
	OLED_ShowString(3, 1, "OLED_OK");
	if (Serial_RxFlag == 1)
		{
			//显示你发给它什么内容
			OLED_ShowString(2, 1, "                ");
			OLED_ShowString(2, 1, Serial_RxPacket);
			Serial_RxFlag = 0;
		}
}

void ShowGo(void)
{
	OLED_ShowString(1, 1, "IN:");
	OLED_ShowString(2, 1, "OUT:");
		if (Serial_RxFlag == 1)
		{
			//显示你发给它什么内容
			OLED_ShowString(1, 4, "                ");
			OLED_ShowString(1, 4, Serial_RxPacket);
			OLED_ShowString(3, 1, "11_COMMAND");
			
			
			if (strcmp(Serial_RxPacket, "Go_Ahead") == 0)
			{
				Go_Ahead();
				Serial_SendString("Go_Ahead_OK\r\n");		//串口发送
				OLED_ShowString(2, 1, "OUT:                ");
				OLED_ShowString(2, 5, "Go_Ahead_OK");		//屏幕显示
			}
			else if (strcmp(Serial_RxPacket, "Go_Back") == 0)
			{
				Go_Back();
				Serial_SendString("Go_Back_OK\r\n");
				OLED_ShowString(2, 1, "OUT:                ");
				OLED_ShowString(2, 5, "Go_Back_OK");
			}
			else if (strcmp(Serial_RxPacket, "Stop") == 0)
			{
				Car_Stop();
				Serial_SendString("Stop_OK\r\n");
				OLED_ShowString(2, 1, "OUT:                ");
				OLED_ShowString(2, 5, "Stop_OK");
			}

			else if (strcmp(Serial_RxPacket, "Turn_Right") == 0)
			{
				Turn_Right();
				Serial_SendString("Turn_Right_OK\r\n");
				OLED_ShowString(2, 1, "OUT:                ");
				OLED_ShowString(2, 5, "Turn_Right_OK");
			}
			else if (strcmp(Serial_RxPacket, "Turn_Left") == 0)
			{
				Turn_Left();
				Serial_SendString("Turn_Left_OK\r\n");
				OLED_ShowString(2, 1, "OUT:                ");
				OLED_ShowString(2, 5, "Turn_Left_OK");
			}

			else if (strcmp(Serial_RxPacket, "Self_Right") == 0)
			{
				Self_Right();
				Serial_SendString("Self_Right_OK\r\n");
				OLED_ShowString(2, 1, "OUT:                ");
				OLED_ShowString(2, 5, "Self_Right_OK");
			}
			else if (strcmp(Serial_RxPacket, "Self_Left") == 0)
			{
				Self_Left();
				Serial_SendString("Self_Left_OK\r\n");
				OLED_ShowString(2, 1, "OUT:                ");
				OLED_ShowString(2, 5, "Self_Left_OK");
			}
	//错误输入
			else
			{
				Serial_SendString("ERROR_COMMAND\r\n");
				OLED_ShowString(2, 1, "OUT:                ");
				OLED_ShowString(2, 5, "ERROR_COMMAND");
			}
			
			Serial_RxFlag = 0;
		}
	if (Serial_RxFlag == 0)
	{
		OLED_ShowString(3, 1, "NO_COMMAND");
	}
}
