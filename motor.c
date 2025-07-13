#include "motor.h"
#include "math.h"
Motor_t motor;

/* 电机初始化函数
 * 参数：
 *   motor         - 电机结构体指针
 *   htim          - PWM定时器句柄
 *   channel_A     - 通道A（PWMA）
 *   channel_B     - 通道B（PWMB）
 *   dir_port_A    - A相方向控制端口
 *   dir_port_B    - B相方向控制端口
 *   AIN_1,AIN_2   - A相控制引脚
 *   BIN_1,BIN_2   - B相控制引脚
 * 返回值：0-成功，-1-失败
 */
int8_t  Motor_Init(Motor_t* motor, 
									TIM_HandleTypeDef* htim, 
									uint32_t channel_A,uint32_t channel_B,
									GPIO_TypeDef* dir_port_A,
									GPIO_TypeDef* dir_port_B,
									uint16_t AIN_1,
									uint16_t AIN_2,
									uint16_t BIN_1,
									uint16_t BIN_2)

{							
	if (motor == NULL || htim == NULL || dir_port_A == NULL || dir_port_B == NULL ) return -1; // 参数有效性检查
		motor->htim = htim;           // 绑定PWM定时器
		motor->channel_A = channel_A;     // 绑定A通道
		motor->channel_B = channel_B;     // 绑定B通道
		motor->dir_port_A = dir_port_A;   // 绑定A相方向端口
		motor->dir_port_B = dir_port_B;   // 绑定B相方向端口
		motor->AIN_1 = AIN_1;     // 绑定A相控制引脚1
		motor->AIN_2 = AIN_2;     // 绑定A相控制引脚2
		motor->BIN_1 = BIN_1;     // 绑定B相控制引脚1
		motor->BIN_2 = BIN_2;     // 绑定B相控制引脚2
		HAL_GPIO_WritePin(dir_port_A, AIN_1, GPIO_PIN_RESET); // 默认A相IN1低电平
		HAL_GPIO_WritePin(dir_port_A, AIN_2, GPIO_PIN_RESET); // 默认A相IN2低电平			
		HAL_GPIO_WritePin(dir_port_B, BIN_1, GPIO_PIN_RESET); // 默认B相IN1低电平
		HAL_GPIO_WritePin(dir_port_B, BIN_2, GPIO_PIN_RESET); // 默认B相IN2低电平
		HAL_TIM_PWM_Start(htim, channel_A);
    HAL_TIM_PWM_Start(htim, channel_B); // 启动A通道PWM和B通道PWM
			
	return 0;
}

/* 设置A电机速度
 * 参数：
 *   speed - 速度（范围-100.0~100.0，正为正转，负为反转）
 * 返回值：0
 */
int8_t Speed_Set_A(Motor_t* motor,float speed){
	if (speed > 100.0) speed = 100.0;        // 限制最大速度
  if (speed < -100.0) speed = -100.0;      // 限制最小速度
	if (speed > 0) {
        HAL_GPIO_WritePin(motor->dir_port_A, motor->AIN_1, GPIO_PIN_RESET);   // 正转
				HAL_GPIO_WritePin(motor->dir_port_A, motor->AIN_2, GPIO_PIN_SET);
	}
	else if(speed < 0) {
				HAL_GPIO_WritePin(motor->dir_port_A, motor->AIN_1, GPIO_PIN_SET);   // 反转
				HAL_GPIO_WritePin(motor->dir_port_A, motor->AIN_2, GPIO_PIN_RESET);
		
	}
	else {
					__HAL_TIM_SET_COMPARE(motor->htim, motor->channel_A, 0);// 速度为0时关闭PWM
		return 0;
	}
	// 设置PWM占空比，绝对值映射到0~1000
	uint32_t PWM = fabs(speed*10);
	__HAL_TIM_SET_COMPARE(motor->htim, motor->channel_A,PWM);
	return 0;
}

/* 设置B电机速度
 * 参数：
 *   speed - 速度（范围-100.0~100.0，正为正转，负为反转）
 * 返回值：0
 */
int8_t Speed_Set_B(Motor_t* motor,float speed){
	if (speed > 100.0) speed = 100.0;        // 限制最大速度
  if (speed < -100.0) speed = -100.0;      // 限制最小速度
	if (speed > 0) {
        HAL_GPIO_WritePin(motor->dir_port_B, motor->BIN_1, GPIO_PIN_RESET);   // 正转
				HAL_GPIO_WritePin(motor->dir_port_B, motor->BIN_2, GPIO_PIN_SET);
	}
	else if(speed < 0) {
				HAL_GPIO_WritePin(motor->dir_port_B, motor->BIN_1, GPIO_PIN_SET);   // 反转
				HAL_GPIO_WritePin(motor->dir_port_B, motor->BIN_2, GPIO_PIN_RESET);
		
	}
	else {
					__HAL_TIM_SET_COMPARE(motor->htim, motor->channel_B, 0);// 速度为0时关闭PWM
		return 0;
	}
	// 设置PWM占空比，绝对值映射到0~1000
	uint32_t PWM = fabs(speed*10);
	__HAL_TIM_SET_COMPARE(motor->htim, motor->channel_B,PWM);
	return 0;
}
/* 电机刹车（高阻模式）
 * 使电机停止，PWM输出高电平
 */
void motor_Stop(Motor_t* motor){
	HAL_GPIO_WritePin(motor->dir_port_A, motor->AIN_1, GPIO_PIN_RESET);  
	HAL_GPIO_WritePin(motor->dir_port_A, motor->AIN_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(motor->dir_port_B, motor->BIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(motor->dir_port_B, motor->BIN_2, GPIO_PIN_RESET);//IN1,IN2,所有控制引脚低电平
	__HAL_TIM_SET_COMPARE(motor->htim,motor->channel_A,1000);
	__HAL_TIM_SET_COMPARE(motor->htim,motor->channel_B,1000);//PWM输出高电平
}
 /*电机短接刹车（短路制动）
 * 所有控制引脚高电平，PWM输出低电平
 */
void motor_shortBrake(Motor_t* motor){
	HAL_GPIO_WritePin(motor->dir_port_A, motor->AIN_1, GPIO_PIN_SET);  
	HAL_GPIO_WritePin(motor->dir_port_A, motor->AIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(motor->dir_port_B, motor->BIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(motor->dir_port_B, motor->BIN_2, GPIO_PIN_SET);//IN1,IN2,所有控制引脚高电平
	__HAL_TIM_SET_COMPARE(motor->htim,motor->channel_A,0);
	__HAL_TIM_SET_COMPARE(motor->htim,motor->channel_B,0);//PWM输出低电平
}
/* 直行控制
 * speed > 0 前进，speed < 0 后退
 * 参数：speed（-100.0~100.0）
 */
void motor_Direct(Motor_t* motor,float speed){   //直行控制
	Speed_Set_A(motor,speed);
	Speed_Set_B(motor,speed);
}

void motor_Left(Motor_t* motor,float speed){
	float Speed = -speed; 
	Speed_Set_A(motor,speed);    //左轮反转，右轮正转
	Speed_Set_B(motor,Speed);    //左轮正转，右轮反转
}

void motor_Right(Motor_t* motor,float speed){
	float Speed = -speed; 
	Speed_Set_A(motor,Speed);    //左轮正转，右轮反转
	Speed_Set_B(motor,speed);    //左轮反转，右轮正转

}
