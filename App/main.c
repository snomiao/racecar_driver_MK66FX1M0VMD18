#include "common.h"
#include "include.h"
#include "queue.h"

Queue_T gU1RxQ = {0};
Queue_T *cmdQ = &gU1RxQ;

#undef VECTOR_069
#define VECTOR_069 UART4_IRQHandler

int flag_print_speed_ctl = 1;
int flag_print_parsing = 0;
int flag_print_uart4 = 0;

// middle signal at 0.15ms in 20ms with TowerPro MG995
// ref: https://components101.com/motors/mg995-servo-motor
// PWM:
// freq: 50
// left: 0.5ms in 20ms duty
// middle: 1.5ms in 20ms duty
// right: 2.5ms in 20ms duty
float v_servo = 7.5;

// middle signal at 0.15ms in 20ms
float v_motor = 7.5;

extern void UART4_IRQHandler(); //串口1 中断接收函数

// float map_float(float x, float in_min, float in_max, float out_min, float out_max)
// {
//     return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
// }k

long map_long(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// void update_servo(float duty)
// {
//     //调整舵机占空比
//     float duty = map_float(range, 0, 1, 0, 17.48);
//     ftm_pwm_duty(FTM2, FTM_CH1, duty);
// }
// void update_motor(float duty){
//     //调整电调占空比
//     float duty = map_float(range, 0, 10000, 0, 100);
//     ftm_pwm_duty(FTM1, FTM_CH0, duty);
// }

// reply
void reply_command(void)
{
    uint8 tmp = 0, sum = 0;
    uart_putchar(UART4, 0xAA);

    uint16 cmd_motor = (uint16)v_motor * 200;
    uint16 cmd_servo = (uint16)v_servo * 200;

    tmp = cmd_motor >> 0;
    sum = tmp;
    uart_putchar(UART4, tmp);

    tmp = cmd_motor >> 8;
    sum += tmp;
    uart_putchar(UART4, tmp);

    tmp = cmd_servo >> 0;
    sum += tmp;
    uart_putchar(UART4, tmp);

    tmp = cmd_servo >> 8;
    sum += tmp;
    uart_putchar(UART4, tmp);

    uart_putchar(UART4, sum);
    uart_putchar(UART4, 0x55);
}

void Parse_Command(void)
{
    uint8 head = 0, tail = 0, check = 0, sum = 0;
    uint8 tmpl = 0, tmph = 0;
    uint16 cmd_servo = 0, cmd_motor = 0;
    // AA 0 DC C2 55 0

    // if(flag_){
    //     printf("%02X\n", ch);
    // }
    dequeue(cmdQ, &head);
    if (head != 0xAA)
        return;
    dequeue(cmdQ, &tmpl);
    dequeue(cmdQ, &tmph);
    cmd_motor = ((uint16)tmph << 8) + tmpl;
    sum = tmph + tmpl;

    dequeue(cmdQ, &tmpl);
    dequeue(cmdQ, &tmph);
    cmd_servo = ((uint16)tmph << 8) + tmpl;
    sum += tmpl;
    sum += tmph;

    dequeue(cmdQ, &check);
    if (check != sum)
        return;

    dequeue(cmdQ, &tail);
    if (tail != 0x55)
        return;

    if (flag_print_speed_ctl)
        printf("speed_ctl: ");
    if (500 <= cmd_servo && cmd_servo <= 2500)
    {
        v_servo = (float)cmd_servo / (float)200;
        ftm_pwm_duty(FTM2, FTM_CH1, v_servo);
        if (flag_print_speed_ctl)
            printf("servo: %d\t", cmd_servo);
    } // TPM2C0V=2*cmd_servo;	//设置舵机PWM

    if (500 <= cmd_motor && cmd_motor <= 2500)
    {
        v_motor = (float)cmd_motor / (float)200;
        ftm_pwm_duty(FTM2, FTM_CH0, v_motor);
        if (flag_print_speed_ctl)
            printf("motor: %d\t", cmd_motor);
    }
    if (flag_print_speed_ctl)
        printf("\n");
    // delay(1);
    // SCI_Reply();
}

void UART4_IRQHandler(void)
{
    // uint8 ch;

    DisableInterrupts; //关总中断

    char ch;
    uart_querychar(UART4, &ch);
    printf("ch: %c\n", ch);

    EnableInterrupts; //开总中断
}

unsigned int fps = 0;
int32_t speed = 0;

unsigned int heartbeat = 0;
int tick_tah = 0;

void PIT2_IRQHandler2()
{
    speed = ftm_quad_get(FTM1); //获取FTM 正交解码 的脉冲数(负数表示反方向)
    ftm_quad_clean(FTM1);

    // update_servo(v_servo);
    // update_motor(v_motor);

    uint8 ch;
    // uint8 chs[20];

    fps++;
    PIT_Flag_Clear(PIT2);
}

void PIT0_IRQHandler2()
{
    tick_tah = heartbeat % 2;

    printf("seconds: %d \t fps: %d \t speed: %d\n", heartbeat, fps, speed);

    ftm_pwm_duty(FTM2, FTM_CH1, v_servo);
    ftm_pwm_duty(FTM2, FTM_CH0, v_motor);

    // uart_putstr(UART4, tick_tah ? "A" : "B");
    // uart_putstr(UART4, "1234\n");

    // v_servo = tick_tah?85: 75;
    // v_servo += 1; // tick_tah ? 20 : 80;
    // if(v_servo > 99) v_servo = 0;
    // v_motor += 5; // tick_tah ? 20 : 80;
    // if(v_motor > 99) v_motor = 0;
    printf("servo: %f \t motor: %f \t .....: \n", v_servo, v_motor);

    heartbeat++;
    fps = 0;
    PIT_Flag_Clear(PIT0);
}
void init_all()
{
    DisableInterrupts;

    // 心跳程序
    pit_init_ms(PIT0, 1000);
    set_vector_handler(PIT0_VECTORn, PIT0_IRQHandler2); //设置 PIT0 的中断服务函数为 PIT0_IRQHandler
    enable_irq(PIT0_IRQn);

    // 速度控制程序
    pit_init_us(PIT2, 1000);
    set_vector_handler(PIT2_VECTORn, PIT2_IRQHandler2);
    enable_irq(PIT2_IRQn);

    // 舵机
    ftm_pwm_init(FTM2, FTM_CH1, 50, 50, FTM2_CH1_PIN);

    // 电调
    ftm_pwm_init(FTM2, FTM_CH0, 50, 50, FTM2_CH0_PIN);

    // 编码器
    ftm_quad_init(FTM1, FTM_PS_1, FTM1_QDPHA_PIN, FTM1_QDPHB_PIN);

    // 串口初始化
    // UART_IRQ_DIS(UART4);                          //串口1   关接收中断
    // uart_rx_irq_dis (UART4);                          //开串口接收中断
    uart_init(UART4, 38400);

    // 不知为何接收中断不能用，目前采用轮询方式
    uart_rx_irq_en(UART4); //开串口接收中断
    set_vector_handler(UART4_RX_TX_VECTORn, UART4_IRQHandler);

    // uart_tx_irq_en (UART4);                          //开串口发送中断
    // uart_txc_irq_en (UART4);                         //开串口发送完成中断

    // uart_rx_irq_dis(UART4);                          //关串口接收中断
    // uart_tx_irq_dis(UART4);                          //关串口发送中断
    // uart_txc_irq_dis(UART4);                         //关串口发送完成中断

    // 完成
    printf("\nConfig OK\n");
    EnableInterrupts;
}

void main(void)
{
    init_all();
    printf("\nrunning..\n");
    //uint16 a = 1500;
    //float b = 200;
    // printf("%f", a/b);
    while (1)
    {
        uint8 ch;
        while (uart_querychar(UART4, &ch))
        {
            enqueue(&gU1RxQ, ch);
            if (flag_print_uart4)
            {
                printf("%02X\n", ch);
            }
        }

        while (get_queue_size(&gU1RxQ) >= 7)
        {
            if (flag_print_parsing)
            {
                printf("parsing cmd...\n");
            }
            Parse_Command();
        }
    }
}
