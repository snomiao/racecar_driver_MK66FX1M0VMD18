#ifndef __CONFIG_H__
#define __CONFIG_H__


#include "common.h"
#include "include.h"
#include "mood.h"
#include "interrupt.h"
#include "speed.h"
#include "direction.h"
#include "control.h"
#include "img.h"

//LED定义
#define LED_RED  PTE27
#define LED_YEL  PTA5
#define LED_BLU  PTA7
#define LED_GRE  PTA9
#define LED_WHT  PTA8

//按键定义
#define S1_PIN	 PTB18
#define S2_PIN   PTB16
#define S3_PIN   PTB10
#define S4_PIN   PTB9

//红外定义
#define R1_PIN   PTD3
#define R2_PIN   PTD1
#define R3_PIN   PTD0
#define R4_PIN   PTC19
#define R5_PIN   PTC15

#define FAR     0
#define CLOSE   1

//舵机部分定义
#define SERVO_PIN       PTE7
#define SERVO_MID       150
#define SERVO_LEFT      130
#define SERVO_RIGHT     170

//图像部分定义
#define IMG_BLACK   0
#define IMG_WHITE   255

//速度定义
#define SET_SPEED   200
void All_Config(void);
void NVIC_Config(void);
void Interrupt_Config(void);
void My_Key_Init(void);
void My_LED_Init(void);

int Mood_Definition();

void Image_Fix_Config();

int gpio_check(PTXn_e ptxn);
void Light_Init();
#endif
