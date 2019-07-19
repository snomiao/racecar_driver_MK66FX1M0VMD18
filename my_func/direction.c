#include "direction.h"

extern Site_t site;                           //显示图像左上角位置

int time_flag;
int servo_duty = SERVO_MID;

float Set_Dir = 40.0;
float Actual_Dir = 0.0;
float Servo_err = 0.0;
float Servo_err_last = 0.0;
float Servo_Kp = 0.0;
float Servo_Kd = 0.0;
float Servo_PWM_Duty = 0.0;

void Servo_Realize(float dir)
{
    Actual_Dir = dir;
    Servo_err_last = Servo_err;
    Servo_err = Set_Dir - Actual_Dir;
    
    //对K进行重新赋值
    if(get_diff_f(Servo_err,0) < 5)
    {
        Servo_Kp = 0.0;
    }
    else if(get_diff_f(Servo_err,0) < 7)
    {
        Servo_Kp = 0.1;
    }
    else if(get_diff_f(Servo_err,0) < 10)
    {
        Servo_Kp = 0.2;
    }
    else if(get_diff_f(Servo_err,0) < 20)
    {
        Servo_Kp = 0.3;
    }
    else 
    {
        Servo_Kp = 0.4;
    }
    
    //对D进行重新赋值
    if(get_diff_f(Servo_err,0) < 5)
    {
        Servo_Kd = 1;
    }
    else if(get_diff_f(Servo_err,0) < 7)
    {
        Servo_Kd = 2;
    }
    else if(get_diff_f(Servo_err,0) < 10)
    {
        Servo_Kd = 3;
    }
    else if(get_diff_f(Servo_err,0) < 20)
    {
        Servo_Kd = 1;
    }
    else 
    {
        Servo_Kd = 1;
    }
    Servo_PWM_Duty = Servo_Kp * Servo_err + Servo_Kd * (Servo_err - Servo_err_last);
    //printf("Servo_PWM_Duty: %f\n",Servo_PWM_Duty);

    Servo_PWM_Duty = Servo_Duty_Protect(Servo_PWM_Duty + SERVO_MID);
    
    
    servo_duty = (int)Servo_PWM_Duty;
    //printf("Servo_PWM_Duty: %f\n",Servo_PWM_Duty);
    site.x = 35;     site.y = 55;
    lcd_str(site,"DUTY",FCOLOUR,BCOLOUR);
    site.x = 35;     site.y = 75;
    lcd_num(site,servo_duty,FCOLOUR,BCOLOUR);

}





float get_diff_f(float a,float b)
{
    if(a>b)
      return (a-b);
    else 
      return (b-a);
}

float Servo_Duty_Protect(float duty)
{
    
//    if(duty > SERVO_RIGHT)
//      return SERVO_RIGHT;
//    else if(duty < SERVO_LEFT)
//      return SERVO_LEFT;
//    else
      return duty;

}



void PIT0_IRQHandler()              //控制红色 LED 灯反转
{
    time_flag++;
    if(time_flag >= 2000)
      time_flag = 0;
    if(time_flag <= servo_duty)
      gpio_set(SERVO_PIN, 1);
    if(time_flag > servo_duty)
      gpio_set(SERVO_PIN, 0);
    
    PIT_Flag_Clear(PIT0);
}

