#include "mood.h"

int set_speed;
int speed_decreasing;
float deviate;
float KP;
float KI;
float KD;

extern int HIDE_FLAG;
extern uint8 CNST;
extern Site_t site;                           //显示图像左上角位置
extern Size_t imgsize;             //图像大小
extern Size_t size;
extern uint8 imgbuff[CAMERA_SIZE];                             //定义存储接收图像的数组
extern uint8 img[CAMERA_H][CAMERA_W];                           //定义存储解压后的图像数组（用于读取像素值，图像处理）
extern int servo_duty;                                          //舵机参数
extern float deviate;
extern int actual_x;
/********************************
	描述：模式一主函数
		  调整阈值并获取补偿值
		  详细功能：灯光设置、按键检测、显示当前阈值、显示图像、舵机调节
		  
********************************/
void Init_Mood(void)
{
	gpio_set(LED_YEL , 1);
    lcd_init(WHITE);
    printf("\nMOOD ONE:  CNST_Check\n");
//        site.x = 50;    site.y = 100;
//	lcd_str(site,"MOOD ONE",FCOLOUR,BCOLOUR);		//显示模式
//        site.x = 0;    site.y = 100;
//	lcd_str(site,"CNST:",FCOLOUR,BCOLOUR);			//显示 CNST: 图像
	while(1)
	{
		if(key_check(KEY_D) == 0)					//检测按键
		{
			CNST++;
			SCCB_WriteByte(OV7725_CNST, CNST);
                        site.x = 0;    site.y = 0;
			lcd_num(site,CNST,YELLOW,BLUE);		//显示当前阈值
		}
		if(key_check(KEY_U) == 0)
		{
			CNST--;
			SCCB_WriteByte(OV7725_CNST, CNST);		//检测按键
                        site.x = 0;    site.y = 0;
			lcd_num(site,CNST,YELLOW,BLUE);		//显示当前阈值
		}
		site.x = 0;    site.y = 0;
		camera_get_img();
		lcd_img_binary_z(site, size, imgbuff, imgsize,BLACK,WHITE);    //获取并显示图像
        //vcan_sendimg(imgbuff, sizeof(imgbuff));
        lcd_num(site,CNST,YELLOW,BLUE);
        site.x = 2;     site.y = 55;
        lcd_num(site,servo_duty,YELLOW,BLUE);
		if(key_check(KEY_R) == 0)					//舵机左调
		{
            servo_duty = servo_duty -10;
            site.x = 2;     site.y = 20;
            lcd_str(site,"DUTY",YELLOW,BLUE);
            site.x = 2;     site.y = 55;
            lcd_num(site,servo_duty,YELLOW,BLUE);

		}
        if(key_check(KEY_L) == 0)					//舵机右调
        {
            servo_duty = servo_duty +10;
            site.x = 2;     site.y = 20;
            lcd_str(site,"DUTY",YELLOW,BLUE);
            site.x = 2;     site.y = 55;
            lcd_num(site,servo_duty,YELLOW,BLUE);
        }
	}
}

void Low_Speed()
{	
    lcd_clear(WHITE);
	//恒定速度并且无减速
	set_speed = SET_SPEED;				//额定速度
	speed_decreasing = 0;
    
    //动力PID系数设置
    KP = 0.04;
    KI = 0;
    KD = 0.06;
    
    
    //动力控制开启
    int start_flag = 0;
    while(1){
        camera_get_img();
        img_extract(img, imgbuff, CAMERA_SIZE);
        for(int i = 0; i < 60 ; i++)
            for(int j = 0 ; j < 80 ; j++)
              if(img[i][j] == IMG_WHITE){
                Start_Run();
                start_flag = 1;
              }
        if(start_flag)
          break;
    }
    //图像处理
    //int i = 1;
    while(1)
    {
        /*
        DELAY_MS(20);
        servo_duty = servo_duty + i;
        if(servo_duty == 170)
          i = -1;
        if(servo_duty == 130)
          i = 1;
        
        site.x = 2;     site.y = 55;
        lcd_str(site,"DUTY",YELLOW,BLUE);
        site.x = 2;     site.y = 75;
        lcd_num(site,servo_duty,YELLOW,BLUE);
        */
        
        Image_Handler();
    }
    
}

void Mid_Speed()
{	
	HIDE_FLAG = 1;
    gpio_set(LED_WHT,1);
    DELAY_MS(1000);
    gpio_set(LED_WHT,0);
    return ;
}

void High_Speed()
{	
	HIDE_FLAG = 0;
    gpio_set(LED_WHT,1);
    DELAY_MS(1000);
    gpio_set(LED_WHT,0);
    return ;
}


void Start_Run()
{
    //速度控制 10 ms 一次
    pit_init_ms(PIT1 , 10);

}

void LED_Handler(int mood, int across_mood, int circle_mood)
{
  
    site.y = 2;     site.x = 70;
    lcd_str(site,"E",FCOLOUR,BCOLOUR);
    site.y = 20;    site.x = 70;
    lcd_str(site,"A",FCOLOUR,BCOLOUR);
    site.y = 40;    site.x = 70;
    lcd_str(site,"C",FCOLOUR,BCOLOUR);
  
    if(mood == 0)
    {
        disable_irq (PIT3_IRQn);        //蓝色灯熄灭
        
        site.y = 2;    site.x = 80;
        lcd_num(site,0,FCOLOUR,BCOLOUR);
    }
    else if(mood == 1)
    {
        site.y = 2;    site.x = 80;
        lcd_num(site,1,FCOLOUR,BCOLOUR);
              
        pit_init_ms(PIT3 , 200);        
        enable_irq (PIT3_IRQn);
        if(across_mood == 0){
          site.y = 20;    site.x = 80;
          lcd_str(site,"0",FCOLOUR,BCOLOUR);
          gpio_set(LED_GRE,0);
          gpio_set(LED_WHT,0);
        }
        else if(across_mood == 1){
          site.y = 20;    site.x = 80;
          lcd_str(site,"1",FCOLOUR,BCOLOUR);
          gpio_set(LED_GRE,1);
          gpio_set(LED_WHT,0);
        }
        else if(across_mood == 2){
          site.y = 20;    site.x = 80;
          lcd_str(site,"2",FCOLOUR,BCOLOUR);
          gpio_set(LED_GRE,0);
          gpio_set(LED_WHT,1);
        }
        else if(across_mood == 3){
          site.y = 20;    site.x = 80;
          lcd_str(site,"3",FCOLOUR,BCOLOUR);
          gpio_set(LED_GRE,0);
          gpio_set(LED_WHT,0);
        }
        else if(across_mood == 4){
          site.y = 20;    site.x = 80;
          lcd_str(site,"4",FCOLOUR,BCOLOUR);
          gpio_set(LED_GRE,1);
          gpio_set(LED_WHT,1);
        }
          
    }
    else if(mood == 2)
    {
        site.y = 2;    site.x = 80;
        lcd_num(site,2,FCOLOUR,BCOLOUR);
    }


}