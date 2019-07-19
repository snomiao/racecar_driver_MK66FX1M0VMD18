#ifndef __CONTROL_H__
#define __CONTROL_H__


#include "common.h"
#include "include.h"
#include "config.h"


void Left_Full_Turn();
void Right_Full_Turn();
void Search_Light();
void Normal_Run();
void Motion_Handler(int dif , int lr);

#endif
