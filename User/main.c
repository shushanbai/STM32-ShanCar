#include "stm32f10x.h"
#include "OLED.h"
#include "Serial.h"
#include "CarGo.h"
#include "Track_Init.h"
#include "TrackTask.h"
#include "Servo_PWM.h"
#include "Ultrasonic.h"
#include "ServoTask.h"
#include "string.h"

typedef enum
{
    MODE_MANUAL = 0,
    MODE_TRACK,
    MODE_OBSTACLE
} CarMode;

CarMode Car_Mode = MODE_MANUAL;

int main(void)
{
    OLED_Init();
    Serial_Init();
    Car_Init();
    Track_Init();
    Servo_PWM_Init();
    Ultrasonic_Init();

    OLED_ShowString(1, 1, "Car System");
    OLED_ShowString(2, 1, "Manual Mode");

    while (1)
    {
        if (Serial_RxFlag == 1)
        {
            if (strcmp(Serial_RxPacket, "Control") == 0)
            {
                Car_Stop();
                Car_Mode = MODE_MANUAL;
                Serial_RxFlag = 0;

                OLED_ShowString(2, 1, "Manual Mode   ");
            }
            else if (strcmp(Serial_RxPacket, "AutoT") == 0)
            {
                Car_Stop();
                Car_Mode = MODE_TRACK;

                OLED_ShowString(2, 1, "Track Mode    ");
            }
            else if (strcmp(Serial_RxPacket, "AutoU") == 0)
            {
                Car_Stop();
                Car_Mode = MODE_OBSTACLE;

                OLED_ShowString(2, 1, "Obstacle Mode ");
				
            }
            else if (Car_Mode == MODE_MANUAL)
            {
                if (strcmp(Serial_RxPacket, "Go_Ahead") == 0)
                    Go_Ahead();
                else if (strcmp(Serial_RxPacket, "Go_Back") == 0)
                    Go_Back();
                else if (strcmp(Serial_RxPacket, "Turn_Left") == 0)
                    Turn_Left();
                else if (strcmp(Serial_RxPacket, "Turn_Right") == 0)
                    Turn_Right();
                else if (strcmp(Serial_RxPacket, "Self_Left") == 0)
                    Self_Left();
                else if (strcmp(Serial_RxPacket, "Self_Right") == 0)
                    Self_Right();
                else if (strcmp(Serial_RxPacket, "Stop") == 0)
                    Car_Stop();

                Serial_RxFlag = 0;
            }
            else
            {
                Serial_RxFlag = 0;
            }
        }


        switch (Car_Mode)
        {
            case MODE_MANUAL:
                break;

            case MODE_TRACK:
                Track_Task();
                break;

            case MODE_OBSTACLE:
                OA_Task();
                break;

            default:
                Car_Stop();
                Car_Mode = MODE_MANUAL;
                break;
        }
    }
}