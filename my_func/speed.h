#ifndef __SPEED_H__
#define __SPEED_H__

#include "common.h"
#include "include.h"
#include "config.h"


#define LIMITED(x, a, b) MAX(MIN(x, b), a)


void PIT1_IRQHandler();

void Speed_Control();
void Speed_Realize_Left(float err);
void Speed_Realize_Right(float err);

float Deal_P_Left(float Kp, float err);
float Deal_I_Left(float Ki, float err);
float Deal_D_Left(float Kd, float err);

float Speed_I_Left(float err);
float Speed_D_Left(float err);

int16_t Get_Actual_Speed_Left();

float Deal_P_Right(float Kp, float err);
float Deal_I_Right(float Ki, float err);
float Deal_D_Right(float Kd, float err);

float Speed_I_Right(float err);
float Speed_D_Right(float err);

int16_t Get_Actual_Speed_Right();
#endif
