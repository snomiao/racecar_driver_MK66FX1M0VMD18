#include "speed.h"

extern Site_t site;                           //显示图像左上角位置
extern int set_speed;
extern int speed_decreasing;
extern float KP;
extern float KI;
extern float KD;

int  actual_speed_left;
int  actual_speed_right;
int  need_speed_left;
int  need_speed_right;

int FoB = 0;
int wall_flag = 0;

void PIT1_IRQHandler()
{
    //进行电机控制调整
    Speed_Control();
    PIT_Flag_Clear(PIT1);
}





void Speed_Control()
{
    actual_speed_left = Get_Actual_Speed_Left();
    actual_speed_right = Get_Actual_Speed_Right();
    
    if(FoB){
        need_speed_left = set_speed * -1;
        need_speed_right = set_speed * -1;
        FoB--;
    }
    
    float err_left = need_speed_left - actual_speed_left;
    float err_right = need_speed_right - actual_speed_right;
    
    
    if(ABS(actual_speed_left) <= 25 || ABS(actual_speed_right) <= 25)
      wall_flag++;
    else
      wall_flag = 0;
    
    if(wall_flag > 40)
      FoB = 50;
    
    
    site.x = 85;     site.y = 55;
    lcd_str(site,"FoB",FCOLOUR,BCOLOUR);
    site.x = 85;     site.y = 75;
    lcd_num(site,FoB,FCOLOUR,BCOLOUR);
    
    
    if(err_left < -40 && err_right < -40){     //点刹
        ftm_pwm_duty(FTM0, FTM_CH3, 0);          //左电机
        ftm_pwm_duty(FTM3, FTM_CH0, 0);          //右电机
    }
    if(err_left > 50 && err_right > 50){        //点加
        ftm_pwm_duty(FTM0, FTM_CH3, 100);          //左电机
        ftm_pwm_duty(FTM3, FTM_CH0, 100);          //右电机
    
    }
    
    Speed_Realize_Left(err_left);
    Speed_Realize_Right(err_right);

}

/********************************************
 * 速度PID控制实现函数
 * 
 */
void Speed_Realize_Left(float err)
{
    //增量式PID
  
    //在当前偏差下的 PID 分段处理
    float kp = KP;
    float ki = KI;
    float kd = KD;
    // PID 计算
    static float motor_out_left = 0;
    motor_out_left += kp * err + ki * Speed_I_Left(err) + kd * Speed_D_Left(err);
    //计算电机占空比，并加上保护
    float pwm_duty = LIMITED(50 + motor_out_left, 5, 95);
    //调整电机占空比
    ftm_pwm_duty(FTM0, FTM_CH3, pwm_duty);
}

void Speed_Realize_Right(float err)
{
    //增量式PID
  
    //在当前偏差下的 PID 分段处理
    float kp = KP;
    float ki = KI;
    float kd = KD;
    // PID 计算
    static float motor_out_right = 0;
    motor_out_right += kp * err + ki * Speed_I_Right(err) + kd * Speed_D_Right(err);
    //计算电机占空比，并加上保护
    float pwm_duty = LIMITED(50 + motor_out_right, 5, 95);
    //调整电机占空比
    ftm_pwm_duty(FTM3, FTM_CH0, pwm_duty);
}




// 积分函数，这个函数把曾经输入过的值全部加起来
float Speed_I_Left(float err){
  static float sum_err_left=0;
  sum_err_left += err;
  return sum_err_left;
}

// 微分函数，这个函数返回本次输入与上次输入的值的差
float Speed_D_Left(float err){
  static float last_err_left;
  static char init = 0;
  if(!init){
    last_err_left=err;
    init=1;
  }
  float d = err - last_err_left;
  last_err_left = err;
  return d;
}



// 积分函数，这个函数把曾经输入过的值全部加起来
float Speed_I_Right(float err){
  static float sum_err_right=0;
  sum_err_right += err;
  return sum_err_right;
}

// 微分函数，这个函数返回本次输入与上次输入的值的差
float Speed_D_Right(float err){
  static float last_err_right;
  static char init = 0;
  if(!init){
    last_err_right=err;
    init=1;
  }
  float d = err - last_err_right;
  last_err_right = err;
  return d;
}






int16_t Get_Actual_Speed_Left()
{
    int32_t val;    
    val = ftm_quad_get(FTM2);          //获取FTM 正交解码 的脉冲数(负数表示反方向)    
    ftm_quad_clean(FTM2);    
    //val = -val;
    site.x = 2;    site.y = 2;
	lcd_str(site,"LEF",FCOLOUR,BCOLOUR);
    site.x = 28;    site.y = 2;
	lcd_num(site,need_speed_left,FCOLOUR,BCOLOUR);
    site.x = 28;    site.y = 20;
    if(val >= 0)
      lcd_num(site,ABS(val),FCOLOUR,BCOLOUR);
    else
      lcd_num(site,ABS(val),RED,BCOLOUR);
    return val;
}

int16_t Get_Actual_Speed_Right()
{
    int32_t val;    
    val = ftm_quad_get(FTM1);          //获取FTM 正交解码 的脉冲数(负数表示反方向)    
    ftm_quad_clean(FTM1);    
    val = -val;
    site.x = 50;    site.y = 2;
	lcd_str(site,"RIG",FCOLOUR,BCOLOUR);
    site.x = 76;    site.y = 2;
	lcd_num(site,need_speed_right,FCOLOUR,BCOLOUR);
    site.x = 76;    site.y = 20;
    if(val >= 0)
      lcd_num(site,ABS(val),FCOLOUR,BCOLOUR);
    else
      lcd_num(site,ABS(val),RED,BCOLOUR);
    return val;
}
