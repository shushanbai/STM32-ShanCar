#include "stm32f10x.h"                  // Device header   
#include "CarGo.h"
#include "Delay.h"
#include "Track_Init.h"

#include "OLED.h"
#include "Serial.h"
#include "string.h"
#include "ShowPacket.h"

uint8_t S1; 
uint8_t S2; 
uint8_t S3; 
uint8_t S4; 

/**
  * @brief  计算加权偏差
  * @param  无
  * @retval -3 到+3：检测黑线计算得到的偏差范围
  *         99：四路均未检测到黑线（丢线）
  *			33：四路均检测到黑线
  *
  * 传感器从左到右： S1	S2	S3	S4
  *	  		 权重： -3	-1	+1	+3
  * 低电平表示检测到黑线
  */

#define LOST  99
#define ALL  33



float Track_Error(void)
{
    int16_t sum = 0;
    int8_t count = 0;

    if (S1 == 0) { sum -= 3; count++; }
    if (S2 == 0) { sum -= 1; count++; }
    if (S3 == 0) { sum += 1; count++; }
    if (S4 == 0) { sum += 3; count++; }

    
    if (count == 0)//四路都没有检测到黑线
        return LOST;
	if (count == 4)//四路都有检测到黑线
		return ALL;
	

    return (float)sum / (float)count;
}



typedef enum
{
	TRACKING,				//正常循迹
	TRACK_LOST,				//丢线处理
	TRACK_ALL,				//4个口全检测到黑线
	ERROR_1,				//分离检测点【报错】
	ERROR_2,				//两侧检测【报错】
	BT						//蓝牙接管
} TrMode;
	

/**
  * @brief  黑线判断
  * @param  无
  * @retval 无
  */



float error ;

TrMode mode = TRACKING;

void Track_Detect(void)
{
	
    S1 = S1_State();
	S2 = S2_State();
	S3 = S3_State();
	S4 = S4_State();
	
	error = Track_Error();
	
/*	以下是两种等价写法，注释的是新学到的把S1~S4变为四位二进制的写法
	如：二进制 0101，换算成十进制是 5，也就是 0x05。
	位运算详见资料[位运算.png]*/
	
/*
	uint8_t Data =(S1 << 3) | (S2 << 2) | (S3 << 1) | S4;	//位运算
	switch (Data)		//根据 pattern 的值，选择对应的处理分支
	{
    case 0x05:
    case 0x0A:
        mode = ERROR_1;
        break;

    case 0x06:
        mode = ERROR_2;
        break;

    case 0x00:
        mode = TRACK_ALL;
        break;

    case 0x0F:
        mode = TRACK_LOST;
        break;

    default:
        mode = TRACKING;
        break;
	}*/

	if ((S1 == 0 && S2 == 1 && S3 == 0 && S4 == 1)||
			(S1 == 1 && S2 == 0 && S3 == 1 && S4 == 0))
			//分离检测点，单独处理// 0101//1010
		mode = ERROR_1;
	
	else if (S1 == 0 && S2 == 1 && S3 == 1 && S4 == 0)	
		//0110，两侧检测
		mode = ERROR_2;
	
	// 按加权偏差控制
	else if (error == LOST)//丢线
			mode = TRACK_LOST;	
		
	else if (error == ALL)//全检测黑线
			mode = TRACK_ALL;
		
	else if ((S1 == 0 && S2 == 0 && S3 == 1 && S4 == 0) ||
			(S1 == 0 && S2 == 1 && S3 == 0 && S4 == 0))
			//多点分布，需特别处理//0010//0100
			mode = ERROR_1;
		
		/*———————————寻迹行动———————————*/
	else 
		mode = TRACKING;

}
	
/**
  * @brief  丢线恢复
  * @param  无
  * @retval 无
  */
void Track_Recover(void)
{
	
}


/**
  * @brief  使用状态机执行循迹任务
  * @param  无
  * @retval 无
  */

void Track_Task(void)
{
	if (Serial_RxFlag == 1)
		mode = BT;
	switch (mode)
	{
		case TRACKING: 
			Track_Detect();
			if(mode == TRACKING)
			{
				if (error < 0)
				{
					Self_Left();
				}
				else if (error == 0)
				{
					Go_Ahead();
				}
				else
				{
					Self_Right();
				}
			}

			 else
				// 一旦检测到异常，立即停止
				Car_Stop();
			break;
		
		case TRACK_LOST:
			Car_Stop();
			OLED_ShowString(4, 1, "LOST	Line");
			Track_Recover();
			mode = BT;
			break;
		
		case TRACK_ALL:
			Car_Stop();
			OLED_ShowString(4, 1, "TR ALL Line");
			mode = BT;
			break;
		
		case ERROR_1:
			//多点分布 or 分离检测点
			Car_Stop();
			OLED_ShowString(4, 1, "Error Line");
			mode = BT;
			break;
		
		case ERROR_2:
			//两侧检测
			Car_Stop();
			OLED_ShowString(4, 1, "2 Side Line");
			mode = BT;
			break;
		
		case BT:
			//蓝牙接管代码
			if (Serial_RxFlag == 1)
			{
			//显示你发给它什么内容
				OLED_ShowString(4, 1, "BT:");
				OLED_ShowString(4, 4, "--------");
				OLED_ShowString(4, 4, Serial_RxPacket);
				
				if (strcmp(Serial_RxPacket, "Control") == 0)
				{
					ShowGo();
					OLED_ShowString(4, 13, "Ctl");
				}
				if (strcmp(Serial_RxPacket, "AutoT") == 0)
				{
					OLED_ShowString(4, 13, "AtT");
					mode = TRACKING;
				}
				Serial_RxFlag = 0;
			}
            break;
	}
}

/**
  * @brief  显示循迹结果
  * @param  无
  * @retval 无
  */
void Show_Track(void)
{
	S1 = S1_State();
	S2 = S2_State();
	S3 = S3_State();
	S4 = S4_State();
	
	OLED_ShowNum(3, 13, 1111, 4);
	if(S1 == 0)
		OLED_ShowNum(3, 13, 0, 1);
	if(S2 == 0)
		OLED_ShowNum(3, 14, 0, 1);
	if(S3 == 0)
		OLED_ShowNum(3, 15, 0, 1);
	if(S4 == 0)
		OLED_ShowNum(3, 16, 0, 1);
	
}
