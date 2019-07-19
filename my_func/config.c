#include "config.h"

//LCD部分结构体变量定义
Site_t site = {0, 0};                  //显示图像左上角位置
Size_t imgsize = {CAMERA_W, CAMERA_H}; //图像大小
Size_t size;                           //显示区域图像大小

// 摄像头部分变量定义
uint8 imgbuff[CAMERA_SIZE];    //定义存储接收图像的数组
uint8 img[CAMERA_H][CAMERA_W]; //原始图像
uint8 CNST = 120;              //摄像头阈值

//图像处理部分变量定义
int img_fix[60]; //赛道宽度补偿（半宽）

int HIDE_FLAG = 1;

void All_Config()
{
    DisableInterrupts;

    // size.H = LCD_H;
    // size.W = LCD_W;

    // //LCD 初始化
    // lcd_init(WHITE);
    // site.x = 20;
    // site.y = 70;
    // lcd_str(site, "CONFIGURING", FCOLOUR, BCOLOUR);
    // printf("\nConfiging...\n");

    // //按键 初始化
    // My_Key_Init();
    // key_init(KEY_U);
    // key_init(KEY_D);
    // key_init(KEY_L);
    // key_init(KEY_R);

    // //LED 初始化
    // My_LED_Init();

    // //红外初始化
    // Light_Init();

    //中断优先级初始化
    // NVIC_Config();

    //串口初始化
    // uart_init(UART4, 115200);

    // //摄像头初始化
    // camera_init(imgbuff);
    // SCCB_WriteByte(OV7725_CNST, CNST);
    // Image_Fix_Config();

    //编码器初始化
    ftm_quad_init(FTM2, FTM_PS_1, FTM2_QDPHA_PIN, FTM2_QDPHB_PIN); //左编码器
    // ftm_quad_init(FTM1, FTM_PS_1, FTM1_QDPHA_PIN, FTM1_QDPHB_PIN);

    // //舵机初始化
    // pit_init_us(PIT0, 10); //pit0 作为软件FTM使用
    // gpio_init(PTE7, GPO, 0);

    // //电机初始化
    // ftm_pwm_init(FTM0, FTM_CH3, 10000, 50, FTM0_CH3_PIN); //左电机
    // ftm_pwm_init(FTM3, FTM_CH0, 10000, 50, FTM3_CH0_PIN); //右电机
    // gpio_init(PTE3, GPO, 1);

    //电机初始化
    ftm_pwm_init(FTM2, FTM_CH1, 10000, 50, FTM2_CH1_PIN); //舵机
    ftm_pwm_init(FTM1, FTM_CH0, 10000, 50, FTM1_CH0_PIN); //电调
    // gpio_init(PTE3, GPO, 1);


    //中断服务函数初始化
    Interrupt_Config();

    lcd_clear(WHITE);
    lcd_str(site, "CONFIG  OK", FCOLOUR, BCOLOUR);
    printf("\nConfig OK\n");

    EnableInterrupts
}

void NVIC_Config()
{
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4); //设置优先级分组,4bit 抢占优先级,没有亚优先级

    NVIC_SetPriority(DMA0_DMA16_IRQn, 0); //配置优先级
    NVIC_SetPriority(PORTA_IRQn, 1);      //配置优先级
    NVIC_SetPriority(PIT0_IRQn, 0);       //配置优先级
    NVIC_SetPriority(PIT1_IRQn, 2);       //配置优先级
    NVIC_SetPriority(PIT2_IRQn, 3);       //配置优先级
    NVIC_SetPriority(PIT3_IRQn, 3);       //配置优先级
}

void Interrupt_Config()
{
    set_vector_handler(PORTA_VECTORn, PORTA_IRQHandler); //设置 PORTA 的中断服务函数为 PORTA_IRQHandler
    enable_irq(PORTA_IRQn);

    set_vector_handler(DMA0_DMA16_VECTORn, DMA0_IRQHandler); //设置 DMA0 的中断服务函数为 DMA0_IRQHandler
    enable_irq(DMA0_DMA16_IRQn);

    set_vector_handler(PIT0_VECTORn, PIT0_IRQHandler); //设置 PIT0 的中断服务函数为 PIT0_IRQHandler
    enable_irq(PIT0_IRQn);

    set_vector_handler(PIT1_VECTORn, PIT1_IRQHandler); //设置 PIT1 的中断服务函数为 PIT1_IRQHandler
    enable_irq(PIT1_IRQn);

    set_vector_handler(PIT2_VECTORn, PIT2_IRQHandler); //设置 PIT2 的中断服务函数为 PIT2_IRQHandler
    enable_irq(PIT2_IRQn);

    set_vector_handler(PIT3_VECTORn, PIT3_IRQHandler); //设置 PIT2 的中断服务函数为 PIT2_IRQHandler
    enable_irq(PIT3_IRQn);
}

void My_Key_Init(void)
{
    gpio_init(S1_PIN, GPI, 1);
    //port_init_NoALT(S1_PIN, PULLUP);         //保持复用不变，仅仅改变配置选项

    gpio_init(S2_PIN, GPI, 1);
    //port_init_NoALT(S2_PIN, PULLUP);         //保持复用不变，仅仅改变配置选项

    gpio_init(S3_PIN, GPI, 1);
    //port_init_NoALT(S3_PIN, PULLUP);         //保持复用不变，仅仅改变配置选项

    gpio_init(S4_PIN, GPI, 1);
    //port_init_NoALT(S4_PIN, PULLUP);         //保持复用不变，仅仅改变配置选项
}

void My_LED_Init(void)
{
    gpio_init(LED_RED, GPO, 0);
    gpio_init(LED_YEL, GPO, 0);
    gpio_init(LED_BLU, GPO, 0);
    gpio_init(LED_GRE, GPO, 0);
    gpio_init(LED_WHT, GPO, 0);
}

int Mood_Definition()
{
    if (gpio_check(S1_PIN) == 0)
        return 1;
    else if (gpio_check(S2_PIN) == 0)
        return 2;
    else if (gpio_check(S3_PIN) == 0)
        return 3;
    else if (gpio_check(S4_PIN) == 0)
        return 4;
    else
        return 0;
}

int gpio_check(PTXn_e ptxn)
{
    if (gpio_get(ptxn) == 0)
    {
        DELAY_MS(50);
        if (gpio_get(ptxn) == 0)
        {
            return 0;
        }
    }
    return 1;
}

void Light_Init()
{
    gpio_init(R1_PIN, GPI, FAR);
    gpio_init(R2_PIN, GPI, FAR);
    gpio_init(R3_PIN, GPI, FAR);
    gpio_init(R4_PIN, GPI, FAR);
    gpio_init(R5_PIN, GPI, FAR);
}

void Image_Fix_Config()
{
    float top_fix = 10;
    float end_fix = 20;
    for (int i = 0; i < 60; i++)
        img_fix[i] = (int)(i * (end_fix - top_fix) / 60 + top_fix);
}
