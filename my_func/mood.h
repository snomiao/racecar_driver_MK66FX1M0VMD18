#ifndef __MOOD_H__
#define __MOOD_H__


#include "common.h"
#include "include.h"
#include "config.h"

/*模式一主函数
  功能：调节阈值并发送补偿
*/


void Init_Mood(void);
void Low_Speed();
void Mid_Speed();
void High_Speed();
void Start_Run();
void LED_Handler(int mood, int across_mood, int circle_mood);
#endif
