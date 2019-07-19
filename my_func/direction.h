#ifndef __DIRECTION_H__
#define __DIRECTION_H__

#include "common.h"
#include "include.h"
#include "config.h"

void Servo_Realize(float dir);
float get_diff_f(float a,float b);
float Servo_Duty_Protect(float duty);

void PIT0_IRQHandler();
#endif
