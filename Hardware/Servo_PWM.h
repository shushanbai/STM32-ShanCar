#ifndef _SERVO_PWM_H
#define _SERVO_PWM_H

void Servo_PWM_Init(void);
void Servo_PWM_SetCompare(uint16_t x);

void Servo_SetAngle(float Angle);

#endif
