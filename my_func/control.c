#include "control.h"

extern Site_t site;                           //ÏÔÊ¾Í¼Ïñ×óÉÏ½ÇÎ»ÖÃ
extern int dis;
extern int dir;
extern int set_speed;
extern int  need_speed_left;
extern int  need_speed_right;
extern int img_fix[60];                                  //ÈüµÀ¿í¶È²¹³¥£¨°ë¿í£©
void Left_Full_Turn()
{
    gpio_set(LED_WHT , 1);
    gpio_set(LED_GRE , 0);
    gpio_set(LED_BLU , 0);
    set_speed = SET_SPEED/4;
    Motion_Handler(dir,1);
    Servo_Realize(0);
}


void Right_Full_Turn()
{
    gpio_set(LED_WHT , 1);
    gpio_set(LED_GRE , 0);
    gpio_set(LED_BLU , 0);
    set_speed = SET_SPEED/4;
    Motion_Handler(dir,2);
    Servo_Realize(79);
}


void Search_Light()
{
    gpio_set(LED_WHT , 0);
    gpio_set(LED_GRE , 1);
    gpio_set(LED_BLU , 0);
    set_speed = SET_SPEED/1.5;
    Motion_Handler(dir,3);
    Servo_Realize(79);
}

void Normal_Run()
{   
    gpio_set(LED_WHT , 0);
    gpio_set(LED_GRE , 0);
    gpio_set(LED_BLU , 1);
    
    set_speed = SET_SPEED;
    
    site.x = 2;     site.y = 55;
    lcd_str(site,"DIR",FCOLOUR,BCOLOUR);
    site.x = 2;     site.y = 75;
    lcd_num(site,(int)dir,FCOLOUR,BCOLOUR);
    
    printf("dir:%d\n",dir);
    
//    if(dis > 50){
//      set_speed = SET_SPEED/3;
//      dir = 79;
//    }
//    else
//      set_speed = SET_SPEED;    
    
    Motion_Handler(dir,0);
    Servo_Realize(dir + img_fix[dis]);

}


void Motion_Handler(int dif , int lr)
{
    if(lr == 0)
    {
        need_speed_left = set_speed;
        need_speed_right = set_speed;
    }
    else if(lr == 1)
    {
        need_speed_left = set_speed *100 /170;
        need_speed_right = set_speed *170 /100;
    }
    else if(lr == 2)
    {
        need_speed_left = set_speed *170 /100;
        need_speed_right = set_speed *100 /170;
    }
    else if(lr == 3)
    {
        need_speed_left = set_speed *110 /100;
        need_speed_right = set_speed *100 /150;
    }
    

}
