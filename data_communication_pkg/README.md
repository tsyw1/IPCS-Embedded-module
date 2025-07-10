# 通用数据通信协议模块

## 模块简介

本模块实现了一个轻量级、可靠的数据通信协议，适用于嵌入式系统中的串口、SPI、I2C等多种通信方式。

**主要特性：**
- 固定帧格式：帧头(2) + 长度(2) + 命令(1) + 数据(n) + CRC16(2) + 帧尾(2)
- 支持可变长度数据传输（最大256字节）
- 可选CRC16校验
- 状态机解析，自动错误恢复

## API函数接口

### 1. 初始化函数
```c
void data_comm_init(void);
```
**说明：** 初始化协议模块，在使用前必须调用

### 2. 数据发送函数
```c
uint16_t data_comm_send(uint8_t cmd, uint8_t *data, uint16_t len);
```
**参数：**
- `cmd`: 命令字节
- `data`: 数据缓冲区指针
- `len`: 数据长度（0~256）

**返回：** 实际发送的字节数

### 3. 数据解析函数
```c
void data_comm_parse_byte(uint8_t byte);
```
**说明：** 解析接收到的字节，在接收中断中调用

### 4. 用户实现函数（需要前往.c文件中进行实现）

```c
// 发送函数 - 根据实际硬件实现
void user_transmit(uint8_t *data, uint16_t len);

// 接收回调 - 处理完整数据包
void user_packet_handler(uint8_t cmd, uint8_t *data, uint16_t len);
```

## 使用示例

### 1. 初始化
```c
int main(void)
{
    // 硬件初始化
    HAL_Init();
    MX_USART1_UART_Init();
    
    // 协议初始化
    data_comm_init();
    
    while(1) {
        // 主循环
    }
}
```

### 2. 实现硬件发送函数
```c
void user_transmit(uint8_t *data, uint16_t len)
{
    // 串口发送示例
    HAL_UART_Transmit(&huart1, data, len, 100);
    
    // SPI发送示例
    // HAL_SPI_Transmit(&hspi1, data, len, 100);
    
    // I2C发送示例
    // HAL_I2C_Master_Transmit(&hi2c1, DEVICE_ADDR, data, len, 100);
}
```

### 3. 接收中断处理
```c
// 串口接收中断回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        static uint8_t rx_byte;
        // 解析接收到的字节
        data_comm_parse_byte(rx_byte);
        // 继续接收下一字节
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}
```

### 4. 数据包处理
```c
void user_packet_handler(uint8_t cmd, uint8_t *data, uint16_t len)
{
    switch(cmd) {
        case 0x01:  // 传感器数据（温度+湿度）
            if (len == 4) {
                int16_t temp = (data[0] << 8) | data[1];
                int16_t humi = (data[2] << 8) | data[3];
                printf("Temperature: %.1f°C, Humidity: %.1f%%\n", 
                       temp / 10.0, humi / 10.0);
            }
            break;
            
        case 0x02:  // LED控制
            if (len == 1) {
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 
                                  data[0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
                printf("LED: %s\n", data[0] ? "ON" : "OFF");
            }
            break;
            
        case 0x03:  // 电机控制
            if (len == 4) {
                uint16_t speed = (data[0] << 8) | data[1];
                uint16_t duration = (data[2] << 8) | data[3];
                printf("Motor: Speed=%d, Duration=%dms\n", speed, duration);
                // motor_control(speed, duration);
            }
            break;
            
        default:
            printf("Unknown command: 0x%02X\n", cmd);
            break;
    }
}
```

### 5. 发送数据示例
```c
// 发送传感器数据 - 对应接收端的 case 0x01
void send_sensor_data(float temperature, float humidity)
{
    uint8_t buffer[4];
    int16_t temp = (int16_t)(temperature * 10);  // 0.1°C精度
    int16_t humi = (int16_t)(humidity * 10);     // 0.1%精度
    
    buffer[0] = (temp >> 8) & 0xFF;
    buffer[1] = temp & 0xFF;
    buffer[2] = (humi >> 8) & 0xFF;
    buffer[3] = humi & 0xFF;
    
    data_comm_send(0x01, buffer, 4);
}

// 发送LED控制命令 - 对应接收端的 case 0x02
void send_led_command(uint8_t state)
{
    data_comm_send(0x02, &state, 1);  // state: 0=OFF, 1=ON
}

// 发送电机控制命令 - 对应接收端的 case 0x03
void send_motor_command(uint16_t speed, uint16_t duration)
{
    uint8_t buffer[4];
    
    buffer[0] = (speed >> 8) & 0xFF;
    buffer[1] = speed & 0xFF;
    buffer[2] = (duration >> 8) & 0xFF;
    buffer[3] = duration & 0xFF;
    
    data_comm_send(0x03, buffer, 4);
}
```