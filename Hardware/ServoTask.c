#include "stm32f10x.h"                  // Device header
#include "Servo_PWM.h"   
#include "Ultrasonic.h"  
#include "CarGo.h"
#include "Delay.h"

#include "OLED.h"
#include "Serial.h"
#include "string.h"
#include "ShowPacket.h"


float F_Distance;			//正面安全距离
float LR_Distance;			//左右自转安全距离
uint8_t Turn_T;				//转弯所花的delay时间ms（自转）
uint8_t Body_T;				//绕过障碍物时，从检测无障碍到车身完全驶出的行驶时间。
							//也就是车走过车身这么长距离花的时间。ms

#define FRONT  0
#define LEFT   1
#define RIGHT  2

uint8_t Turn_Flag = 0;


void Servo_Turn(uint8_t flag)
{
	OLED_ShowString(4,16,"  ");
	if(flag == FRONT)
	{
		Servo_SetAngle(90);
		OLED_ShowString(4,16,"F");
	}
	if(flag == LEFT)
	{
		Servo_SetAngle(0);
		OLED_ShowString(4,16,"L");
	}

	if(flag == RIGHT)
	{
		Servo_SetAngle(180);
		OLED_ShowString(4,16,"R");
	}
	
}


typedef enum
{
    FORWARD,       // 正常前进
    SCAN_LR,          // 扫描左右
    TURN_L,        // 左转
    TURN_R,  	   // 右转
    BlueT,           //蓝牙接管
	PASS,			//小幅度绕过障碍物
	ERROR_1,				//直行超声检测错误/前方无障碍物
	ERROR_2					//转弯安全距离不够
	
} CarState;

CarState currentState = BlueT;

void OA_Task(void)	//obstacle avoidance
{
	float Front;
	float SCL;
	float SCR;
    switch (currentState)
    {
        case FORWARD:
        // 正常前进的代码
			Servo_Turn(FRONT);
			Front = Get_Distance();
			Go_Ahead();
		if (Front <= 0)
			{
				currentState = ERROR_1;
			}
		
			else if (Front < F_Distance)
			{
				Car_Stop();
				currentState = SCAN_LR;
				
			}
			else if(Front >= F_Distance)
			{
				Go_Ahead();
			}
            break;

        case SCAN_LR:
			Car_Stop();
		
            // 扫描并比较左右的距离		
			Servo_Turn(LEFT);
			Delay_ms(150);
			SCL = Get_Distance();
			Delay_ms(150);
			Servo_Turn(RIGHT);
			Delay_ms(150);
			SCR = Get_Distance();
		
		//比较左右距离
			if (SCL <= 0 || SCR <= 0)
				currentState = ERROR_2;
			else if(SCL > SCR )				//左面宽，右面窄
			{
				if(SCL >= LR_Distance)
				currentState = TURN_L;
				else
				currentState = ERROR_2;
			}
			else
			{
				if(SCR >= LR_Distance)
				currentState = TURN_R;
				else
				currentState = ERROR_2;
			}
			
            break;

        case TURN_L:
            // 左转的代码
			Self_Left();
			Delay_ms(Turn_T);
			Car_Stop();
			Turn_Flag = LEFT;
			currentState = PASS;
            break;

        case TURN_R:
            // 右转的代码
			Self_Right();
			Delay_ms(Turn_T);
			Car_Stop();
			Turn_Flag = RIGHT;
			currentState = PASS;
            break;
		
	/*———————————— Model —————————————*/

		case PASS:
			if(Turn_Flag == LEFT)
			{
				Servo_Turn(RIGHT);
				
				if(Get_Distance() >= F_Distance || Get_Distance() < 0)
				{
					Go_Ahead();
					Delay_ms(Body_T);
					Car_Stop();
					
					Self_Right();
					Servo_Turn(FRONT);
					Delay_ms(Turn_T);
					Car_Stop();
					
					Go_Ahead();
					currentState = FORWARD;
				}
			}
			if(Turn_Flag == RIGHT)
			{
				Servo_Turn(LEFT);
				Go_Ahead();
				if(Get_Distance() >= F_Distance || Get_Distance() < 0)
				{
					Delay_ms(Body_T);
					Car_Stop();
					Self_Left();
					Delay_ms(Turn_T);
					Car_Stop();
					Go_Ahead();
					currentState = FORWARD;
				}
			}
            break;
		
		case BlueT:
			//蓝牙接管代码
			if (Serial_RxFlag == 1)
			{
			//显示你发给它什么内容
				OLED_ShowString(3, 1, "                ");
				OLED_ShowString(3, 1, Serial_RxPacket);
				OLED_ShowString(4, 16, "B");
				
				if (strcmp(Serial_RxPacket, "Control") == 0)
				{
					ShowGo();
					OLED_ShowString(4, 14, "Ctl");
				}
				if (strcmp(Serial_RxPacket, "AutoU") == 0)
				{
					OLED_ShowString(4, 14, "AtU");
					currentState = FORWARD;
				}
				Serial_RxFlag = 0;
			}
            break;
		
	/*———————————— Error —————————————*/
		case ERROR_1:
			Car_Stop();
			OLED_ShowString(4,1,"       ");
			OLED_ShowString(4,1,"ERROR_1");//LED显示报错
			currentState = BlueT;//蓝牙模式
            break;
		
		case ERROR_2:
			Car_Stop();
			OLED_ShowString(4,1,"       ");
			OLED_ShowString(4,1,"ERROR_2");
			currentState = BlueT;
			break;
    }
}



