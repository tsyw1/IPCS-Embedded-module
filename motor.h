#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"

// 电机控制结构体
typedef struct {
    TIM_HandleTypeDef* htim;  // PWM定时器句柄
    uint32_t channel_A,channel_B;         // PWMA,PWMB通道
    GPIO_TypeDef* dir_port_A;     // 方向控制端口
    uint16_t AIN_1,AIN_2;         // 方向控制引脚
		GPIO_TypeDef* dir_port_B;     // 方向控制端口
    uint16_t BIN_1,BIN_2;         // 方向控制引脚
} Motor_t;

/*电机初始化并初始化
*参数：motor-电机结构指针，htim-PWM定时器句柄，channel-PWMA,B通道，dir_port_A,dir_port_B-方向控制端口，
*AIN_1,AIN_2,BIN_1,BIN_2-方向控制引脚，hdma_tim-DMA句柄，dma_channel-DMA通道
*
*
*/
int8_t  Motor_Init(Motor_t* motor, 
					TIM_HandleTypeDef* htim, 
					uint32_t channel_A,uint32_t channel_B,
					GPIO_TypeDef* dir_port_A,
					GPIO_TypeDef* dir_port_B,
					uint16_t AIN_1,
					uint16_t AIN_2,
					uint16_t BIN_1,
					uint16_t BIN_2);

/*设置电机A的速度
*参数：-100.0 - 100.0
*/
int8_t Speed_Set_A(Motor_t* motor,float speed);

/*设置电机B的速度
*参数：-100.0 - 100.0
*/
int8_t Speed_Set_B(Motor_t* motor,float speed);


//电机自由停止模式，自由停止
void motor_Stop(Motor_t* motor);

//短刹车
void motor_shortBrake(Motor_t* motor);

//直行后退函数，speed > 0 直行，speed < 0 后退
void motor_Direct(Motor_t* motor,float speed);

//左转函数，  speed (0 - 100.0)
void motor_Left(Motor_t* motor,float speed);

//右转函数，  speed (0 - 100.0)
void motor_Right(Motor_t* motor,float speed);

#endif
