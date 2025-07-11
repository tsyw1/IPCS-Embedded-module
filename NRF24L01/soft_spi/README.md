# NRF24L01软件SPI驱动模块

## 模块简介

本模块实现了NRF24L01 2.4GHz无线收发模块的软件SPI驱动，适用于没有硬件SPI接口或硬件SPI资源紧张的嵌入式系统。

**主要特性：**
- 纯软件实现SPI时序，不依赖硬件SPI
- 支持1Mbps和2Mbps通信速率
- 支持自动应答和自动重发
- 最大32字节数据包传输
- 硬件无关设计，易于移植
- 提供完整的发送/接收接口


## API函数接口

### 1. 初始化函数
```c
NrfStatus nrf24l01_init(NrfConfig *config);
```
**说明：** 初始化NRF24L01模块，检测设备是否存在

**参数：**
- `config`: 配置参数指针，为NULL时使用默认配置

**返回：** 
- `NRF_OK`: 初始化成功
- `NRF_NOT_FOUND`: 设备未找到

### 2. 模式设置函数
```c
void nrf24l01_set_mode(NrfMode mode);
```
**说明：** 设置工作模式

**参数：**
- `mode`: 工作模式
  - `NRF_MODE_TX`: 发送模式
  - `NRF_MODE_RX`: 接收模式

### 3. 数据发送函数
```c
NrfStatus nrf24l01_send_packet(uint8_t *data, uint8_t len);
```
**说明：** 发送数据包

**参数：**
- `data`: 数据缓冲区
- `len`: 数据长度（1-32字节）

**返回：**
- `NRF_OK`: 发送成功
- `NRF_ERROR`: 发送失败
- `NRF_TIMEOUT`: 发送超时

### 4. 数据接收函数
```c
uint8_t nrf24l01_receive_packet(uint8_t *data, uint8_t len);
```
**说明：** 接收数据包

**参数：**
- `data`: 接收缓冲区
- `len`: 缓冲区大小

**返回：** 实际接收的数据长度（0表示无数据）

### 5. 用户实现接口

用户需要在 `.c` 文件中实现以下函数：

- `user_nrf_gpio_init()`: GPIO初始化
- `user_nrf_ce_write()`: CE引脚控制
- `user_nrf_cs_write()`: CS引脚控制
- `user_nrf_sck_write()`: SCK引脚控制
- `user_nrf_mosi_write()`: MOSI引脚控制
- `user_nrf_miso_read()`: MISO引脚读取
- `user_nrf_irq_read()`: IRQ引脚读取
- `user_delay_us()`: 微秒延时
- `user_delay_ms()`: 毫秒延时

## 使用示例

### 1. 初始化配置

```c
int main(void)
{
    // 硬件初始化
    HAL_Init();
    SystemClock_Config();
    
    // 使用默认配置初始化
    if (nrf24l01_init(NULL) != NRF_OK) {
        printf("NRF24L01 not found!\n");
        while(1);
    }
    
    // 或使用自定义配置
    NrfConfig config = {
        .channel = 0x50,  // 使用信道80
        .speed = 0x0E,    // 2Mbps
        .tx_addr = {0x11, 0x22, 0x33, 0x44, 0x55},
        .rx_addr = {0x11, 0x22, 0x33, 0x44, 0x55}
    };
    nrf24l01_init(&config);
    
    while(1) {
        // 主循环
    }
}
```

### 2. 实现GPIO控制函数

```c
// STM32 HAL库示例
void user_nrf_gpio_init(void)
{
    // GPIO已在CubeMX中配置，这里可以添加额外的初始化
}

void user_nrf_ce_write(uint8_t level)
{
    HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, 
                      level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void user_nrf_cs_write(uint8_t level)
{
    HAL_GPIO_WritePin(NRF_CS_GPIO_Port, NRF_CS_Pin, 
                      level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void user_nrf_sck_write(uint8_t level)
{
    HAL_GPIO_WritePin(NRF_SCK_GPIO_Port, NRF_SCK_Pin, 
                      level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void user_nrf_mosi_write(uint8_t level)
{
    HAL_GPIO_WritePin(NRF_MOSI_GPIO_Port, NRF_MOSI_Pin, 
                      level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

uint8_t user_nrf_miso_read(void)
{
    return HAL_GPIO_ReadPin(NRF_MISO_GPIO_Port, NRF_MISO_Pin);
}

uint8_t user_nrf_irq_read(void)
{
    return HAL_GPIO_ReadPin(NRF_IRQ_GPIO_Port, NRF_IRQ_Pin);
}
```

### 3. 实现延时函数

```c
void user_delay_us(uint32_t us)
{
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 1000000 * us);
    while (delay--) {
        __NOP();
    }
}

void user_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}
```

### 4. 发送端示例

```c
// 发送传感器数据
void transmit_sensor_data(void)
{
    uint8_t tx_buffer[32];
    float temperature = 25.6;
    float humidity = 65.3;
    
    // 设置为发送模式
    nrf24l01_set_mode(NRF_MODE_TX);
    
    // 准备数据
    memcpy(tx_buffer, &temperature, sizeof(float));
    memcpy(tx_buffer + 4, &humidity, sizeof(float));
    
    // 发送数据
    if (nrf24l01_send_packet(tx_buffer, 8) == NRF_OK) {
        printf("Data sent successfully\n");
    } else {
        printf("Send failed\n");
    }
}

// 发送控制命令
void send_control_command(uint8_t cmd, uint16_t value)
{
    uint8_t tx_buffer[3];
    
    nrf24l01_set_mode(NRF_MODE_TX);
    
    tx_buffer[0] = cmd;
    tx_buffer[1] = (value >> 8) & 0xFF;
    tx_buffer[2] = value & 0xFF;
    
    nrf24l01_send_packet(tx_buffer, 3);
}
```

### 5. 接收端示例

```c
// 接收数据处理
void receive_data_task(void)
{
    uint8_t rx_buffer[32];
    uint8_t rx_len;
    
    // 设置为接收模式
    nrf24l01_set_mode(NRF_MODE_RX);
    
    while(1) {
        // 检查是否有数据
        rx_len = nrf24l01_receive_packet(rx_buffer, sizeof(rx_buffer));
        
        if (rx_len > 0) {
            printf("Received %d bytes\n", rx_len);
            
            // 根据数据长度判断数据类型
            if (rx_len == 8) {
                // 传感器数据
                float temp, humi;
                memcpy(&temp, rx_buffer, sizeof(float));
                memcpy(&humi, rx_buffer + 4, sizeof(float));
                printf("Temperature: %.1f°C, Humidity: %.1f%%\n", temp, humi);
            } else if (rx_len == 3) {
                // 控制命令
                uint8_t cmd = rx_buffer[0];
                uint16_t value = (rx_buffer[1] << 8) | rx_buffer[2];
                printf("Command: 0x%02X, Value: %d\n", cmd, value);
                
                // 执行命令
                process_command(cmd, value);
            }
        }
        
        HAL_Delay(10);
    }
}
```

### 6. 双向通信示例

```c
// 主机端 - 发送命令并等待响应
void master_communication(void)
{
    uint8_t tx_buffer[32] = {0x01, 0x02, 0x03};  // 查询命令
    uint8_t rx_buffer[32];
    uint8_t rx_len;
    uint32_t timeout;
    
    // 发送查询命令
    nrf24l01_set_mode(NRF_MODE_TX);
    if (nrf24l01_send_packet(tx_buffer, 3) != NRF_OK) {
        printf("Send failed\n");
        return;
    }
    
    // 切换到接收模式等待响应
    nrf24l01_set_mode(NRF_MODE_RX);
    
    timeout = 0;
    while (timeout < 100) {  // 等待100ms
        rx_len = nrf24l01_receive_packet(rx_buffer, sizeof(rx_buffer));
        if (rx_len > 0) {
            printf("Response received: ");
            for (int i = 0; i < rx_len; i++) {
                printf("%02X ", rx_buffer[i]);
            }
            printf("\n");
            break;
        }
        HAL_Delay(1);
        timeout++;
    }
    
    if (timeout >= 100) {
        printf("Response timeout\n");
    }
}

// 从机端 - 接收命令并响应
void slave_communication(void)
{
    uint8_t rx_buffer[32];
    uint8_t tx_buffer[32];
    uint8_t rx_len;
    
    // 默认接收模式
    nrf24l01_set_mode(NRF_MODE_RX);
    
    while(1) {
        rx_len = nrf24l01_receive_packet(rx_buffer, sizeof(rx_buffer));
        
        if (rx_len > 0) {
            // 处理接收到的命令
            if (rx_buffer[0] == 0x01) {  // 查询命令
                // 准备响应数据
                tx_buffer[0] = 0x81;  // 响应标识
                tx_buffer[1] = 0xAA;  // 数据1
                tx_buffer[2] = 0xBB;  // 数据2
                
                // 切换到发送模式
                nrf24l01_set_mode(NRF_MODE_TX);
                nrf24l01_send_packet(tx_buffer, 3);
                
                // 返回接收模式
                nrf24l01_set_mode(NRF_MODE_RX);
            }
        }
        
        HAL_Delay(1);
    }
}
```
