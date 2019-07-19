#include "img.h"

extern uint8 imgbuff[CAMERA_SIZE];                             //定义存储接收图像的数组
extern uint8 img[CAMERA_H][CAMERA_W];                           //定义存储解压后的图像数组（用于读取像素值，图像处理）

extern Site_t site;                           //显示图像左上角位置
extern int servo_duty;

extern int HIDE_FLAG;

int actual_x;
int actual_y;
int lost;

int   dir;
int   dis;

int record_mid[60];
int record_left[60];
int record_right[60];
int lost_flag = 0;

int light[6];                       //用于存储红外避障数据

void Image_Handler()
{
    camera_get_img();
    
    img_extract(img, imgbuff, CAMERA_SIZE);
    
    Search_Point();
    
    Data_Handler();
    
}
//寻找信标函数
//反馈值   actual_x    actual_y    lost
void Search_Point()
{
    //int start_flag = -1;
    //int end_flag = -1;
    //int total = 0;
    int x_total = 0;
    int y_total = 0;
    int num = 0;
    int already_find = 0;
  
    //record 数组 重置
    for(int i = 0 ; i < 60; i++){
      record_left[i] = -1;
      record_right[i] = -1;
      record_mid[i] = -1;
    }
    //寻找特征点
    for(int i = 0 ; i < 59; i++)
    {
        for(int j = 0 ; j < 79 ; j++)
        {
            if(img[i][j] == IMG_BLACK && img[i][j+1] == IMG_WHITE)
              record_left[i] = j;
            if(img[i][j] == IMG_WHITE && img[i][j+1] == IMG_BLACK)
              record_right[i] = j+1;   
        }
        if(record_left[i] != -1 && record_right[i] != -1)
          record_mid[i] = (record_left[i] + record_right[i])/2;
    }
    //过滤异常中点
//    for(int i = 0 ; i < 59; i++)
//    {
//      if(start_flag == -1 && record_mid[i] != -1){
//          start_flag = i;
//      }
//      if(start_flag != -1 && record_mid[i] == -1){
//          end_flag = i-1;
//      }
//      if(start_flag != -1 && end_flag != -1){
//          for(int j = start_flag+1; j <= end_flag; j++)
//          {
//              if(ABS(record_mid[j] - record_mid[j-1]) > 3){
//                total = 0;
//                num = 0;
//                break;
//              }
//              else{
//                total = total + record_mid[j];
//                num++;
//              }
//              if(j == end_flag){
//                actual_x = total/num;
//                actual_y = (start_flag + end_flag)/2;
//                total = 0;
//                num = 0;
//                start_flag = -1;
//                end_flag = -1;
//                already_find++;
//              }
//          }
//      }
//      
      already_find = 0;
      for(int i = 0 ; i < 60 ; i++)
      {
        if(record_mid[i] != -1)
        {   
            already_find = 1;
            x_total = x_total + record_mid[i];
            y_total = y_total + i;
            num++;
        }
      }
      actual_x = x_total/num;
      actual_y = y_total/num;
      printf("actual_x:%d\t",actual_x);
      if(!already_find)
        lost_flag++;
      else
        lost_flag = 0;
      if(lost_flag > 5)
        lost = 1;
      else
        lost = 0;
      
      //printf("lost_flag:%d\tlost:%d\n", lost_flag,lost);
    
}

//数据处理函数
//对  lost   actual_x    actual_y    进行分析
//同时接收  红外感应    数据
//反馈    dir     dis
void Data_Handler()
{

  
  
  
  
    
    /*Get_Obstacle();
    if(light[1] || light[2] || light[3] || light[4] || light[5])
    {
        if((light[1] + light[2] + light[3]) > (light[3] + light[4] + light[5]))
          Left_Full_Turn();
        else
          Right_Full_Turn();
    
    }
    else */if(lost)
      Search_Light();
    else
    {
      if(actual_x != 0 && actual_y != 0){
        dir = actual_x;
        
        dis = actual_y;
        
        if(HIDE_FLAG)
        {
            if(dis < 40)
                Normal_Run();
            else
                Right_Full_Turn();
        }
        else
          Normal_Run();
        
      }
    }
}
//三角计算
void Caculate_ControlData()
{



}






void Get_Obstacle()
{
    light[1] = gpio_get(R1_PIN);
    light[2] = gpio_get(R2_PIN);
    light[3] = gpio_get(R3_PIN);
    light[4] = gpio_get(R4_PIN);
    light[5] = gpio_get(R5_PIN);
}