/**
  ******************************************************************************
  * @file    nrf24l01_soft_spi.c
  * @brief   NRF24L01无线模块软件SPI驱动实现
  * @version V1.0.0
  * @date    2025-01-10
  ******************************************************************************
  */

#include "nrf24l01_soft_spi.h"
#include <string.h>

/* ========================= 私有变量 ========================= */
/* 默认地址配置 */
static const uint8_t default_tx_addr[TX_ADR_WIDTH] = {0x20, 0x97, 0x07, 0x28, 0x00};
static const uint8_t default_rx_addr[RX_ADR_WIDTH] = {0x20, 0x97, 0x07, 0x28, 0x00};

/* 当前配置 */
static NrfConfig g_config;

/* ========================= 私有函数 ========================= */
/**
  * @brief  软件SPI读写一个字节
  * @param  data : 要发送的字节
  * @retval 接收到的字节
  */
static uint8_t spi_read_write_byte(uint8_t data)
{
    uint8_t bit;
    
    for (bit = 0; bit < 8; bit++) {
        user_nrf_sck_write(0);
        
        if (data & 0x80) {
            user_nrf_mosi_write(1);
        } else {
            user_nrf_mosi_write(0);
        }
        
        data = data << 1;
        user_delay_us(1);
        
        user_nrf_sck_write(1);
        
        if (user_nrf_miso_read()) {
            data |= 0x01;
        }
        
        user_delay_us(1);
    }
    
    user_nrf_sck_write(0);
    return data;
}

/* ========================= API函数实现 ========================= */
/**
  * @brief  初始化NRF24L01模块
  * @param  config : 配置参数指针（为NULL时使用默认配置）
  * @retval NrfStatus : 初始化状态
  */
NrfStatus nrf24l01_init(NrfConfig *config)
{
    /* GPIO初始化 */
    user_nrf_gpio_init();
    
    /* 设置初始电平 */
    user_nrf_ce_write(0);
    user_nrf_cs_write(1);
    user_nrf_sck_write(0);
    
    /* 等待上电稳定 */
    user_delay_ms(10);
    
    /* 配置参数 */
    if (config != NULL) {
        memcpy(&g_config, config, sizeof(NrfConfig));
    } else {
        /* 使用默认配置 */
        g_config.channel = NRF_CHANNEL_TX;
        g_config.speed = NRF_SPEED;
        memcpy(g_config.tx_addr, default_tx_addr, TX_ADR_WIDTH);
        memcpy(g_config.rx_addr, default_rx_addr, RX_ADR_WIDTH);
    }
    
    /* 检查设备是否存在 */
    if (nrf24l01_check() != NRF_OK) {
        return NRF_NOT_FOUND;
    }
    
    return NRF_OK;
}

/**
  * @brief  检查NRF24L01是否存在
  * @param  无
  * @retval NrfStatus : NRF_OK-存在，NRF_NOT_FOUND-不存在
  */
NrfStatus nrf24l01_check(void)
{
    uint8_t buf[5] = {0xA5, 0xA5, 0xA5, 0xA5, 0xA5};
    uint8_t i;
    
    /* 写入测试数据 */
    nrf24l01_write_buf(NRF_WRITE_REG + TX_ADDR, buf, 5);
    
    /* 读回数据 */
    nrf24l01_read_buf(TX_ADDR, buf, 5);
    
    /* 验证数据 */
    for (i = 0; i < 5; i++) {
        if (buf[i] != 0xA5) {
            return NRF_NOT_FOUND;
        }
    }
    
    return NRF_OK;
}

/**
  * @brief  读取寄存器
  * @param  reg : 寄存器地址
  * @retval uint8_t : 寄存器值
  */
uint8_t nrf24l01_read_reg(uint8_t reg)
{
    uint8_t reg_val;
    
    user_nrf_cs_write(0);
    spi_read_write_byte(reg);
    reg_val = spi_read_write_byte(0xFF);
    user_nrf_cs_write(1);
    
    return reg_val;
}

/**
  * @brief  写入寄存器
  * @param  reg   : 寄存器地址
  * @param  value : 寄存器值
  * @retval uint8_t : 状态寄存器值
  */
uint8_t nrf24l01_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t status;
    
    user_nrf_cs_write(0);
    status = spi_read_write_byte(reg);
    spi_read_write_byte(value);
    user_nrf_cs_write(1);
    
    return status;
}

/**
  * @brief  读取缓冲区
  * @param  reg  : 寄存器地址
  * @param  buf  : 数据缓冲区
  * @param  len  : 数据长度
  * @retval uint8_t : 状态寄存器值
  */
uint8_t nrf24l01_read_buf(uint8_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t status, i;
    
    user_nrf_cs_write(0);
    status = spi_read_write_byte(reg);
    
    for (i = 0; i < len; i++) {
        buf[i] = spi_read_write_byte(0xFF);
    }
    
    user_nrf_cs_write(1);
    
    return status;
}

/**
  * @brief  写入缓冲区
  * @param  reg  : 寄存器地址
  * @param  buf  : 数据缓冲区
  * @param  len  : 数据长度
  * @retval uint8_t : 状态寄存器值
  */
uint8_t nrf24l01_write_buf(uint8_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t status, i;
    
    user_nrf_cs_write(0);
    status = spi_read_write_byte(reg);
    
    for (i = 0; i < len; i++) {
        spi_read_write_byte(buf[i]);
    }
    
    user_nrf_cs_write(1);
    
    return status;
}

/**
  * @brief  设置工作模式
  * @param  mode : 工作模式（NRF_MODE_TX/NRF_MODE_RX）
  * @retval 无
  */
void nrf24l01_set_mode(NrfMode mode)
{
    user_nrf_ce_write(0);
    
    if (mode == NRF_MODE_RX) {
        /* 接收模式配置 */
        nrf24l01_write_reg(NRF_WRITE_REG + EN_AA, 0x01);              // 使能通道0自动应答
        nrf24l01_write_reg(NRF_WRITE_REG + EN_RXADDR, 0x01);          // 使能通道0接收地址
        nrf24l01_write_reg(NRF_WRITE_REG + SETUP_RETR, 0x1A);         // 自动重发15次
        nrf24l01_write_reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); // 设置通道0数据宽度
        nrf24l01_write_reg(NRF_WRITE_REG + RF_CH, g_config.channel);  // 设置RF通信频率
        nrf24l01_write_reg(NRF_WRITE_REG + RF_SETUP, g_config.speed + 1); // 接收模式+1打开LNA
        nrf24l01_write_reg(NRF_WRITE_REG + CONFIG, 0x0F);             // 配置接收模式参数
        
        nrf24l01_write_buf(NRF_WRITE_REG + RX_ADDR_P0, g_config.rx_addr, RX_ADR_WIDTH);
        nrf24l01_write_buf(NRF_WRITE_REG + TX_ADDR, g_config.tx_addr, TX_ADR_WIDTH);
    } else {
        /* 发送模式配置 */
        nrf24l01_write_buf(NRF_WRITE_REG + TX_ADDR, g_config.tx_addr, TX_ADR_WIDTH);
        nrf24l01_write_buf(NRF_WRITE_REG + RX_ADDR_P0, g_config.rx_addr, RX_ADR_WIDTH);
        
        nrf24l01_write_reg(NRF_WRITE_REG + EN_AA, 0x01);              // 使能通道0自动应答
        nrf24l01_write_reg(NRF_WRITE_REG + EN_RXADDR, 0x01);          // 使能通道0接收地址
        nrf24l01_write_reg(NRF_WRITE_REG + SETUP_RETR, 0x1A);         // 自动重发15次
        nrf24l01_write_reg(NRF_WRITE_REG + RF_CH, g_config.channel);  // 设置RF通信频率
        nrf24l01_write_reg(NRF_WRITE_REG + RF_SETUP, g_config.speed); // 设置速率和功率
        nrf24l01_write_reg(NRF_WRITE_REG + CONFIG, 0x0E);             // 配置发送模式参数
    }
    
    user_nrf_ce_write(1);
    user_delay_ms(1);
}

/**
  * @brief  发送数据包
  * @param  data : 数据缓冲区
  * @param  len  : 数据长度（1-32字节）
  * @retval NrfStatus : 发送状态
  */
NrfStatus nrf24l01_send_packet(uint8_t *data, uint8_t len)
{
    uint8_t sta;
    uint32_t timeout = 0;
    
    /* 参数检查 */
    if (data == NULL || len == 0 || len > TX_PLOAD_WIDTH) {
        return NRF_ERROR;
    }
    
    /* 清空发送FIFO */
    nrf24l01_write_reg(FLUSH_TX, 0xFF);
    
    /* 清除所有中断标识 */
    nrf24l01_write_reg(NRF_WRITE_REG + STATUS, 0x70);
    
    /* 写入数据 */
    user_nrf_ce_write(0);
    nrf24l01_write_buf(WR_TX_PLOAD, data, len);
    user_nrf_ce_write(1);
    
    /* 等待发送完成 */
    while (user_nrf_irq_read() != 0) {
        timeout++;
        if (timeout > 100000) {
            return NRF_TIMEOUT;
        }
        user_delay_us(1);
    }
    
    /* 读取状态 */
    sta = nrf24l01_read_reg(STATUS);
    
    /* 清除中断标志 */
    nrf24l01_write_reg(NRF_WRITE_REG + STATUS, sta);
    
    if (sta & MAX_TX) {
        /* 达到最大重发次数 */
        nrf24l01_write_reg(FLUSH_TX, 0xFF);
        return NRF_ERROR;
    }
    
    if (sta & TX_OK) {
        /* 发送成功 */
        return NRF_OK;
    }
    
    return NRF_ERROR;
}

/**
  * @brief  接收数据包
  * @param  data : 数据缓冲区
  * @param  len  : 缓冲区长度
  * @retval uint8_t : 实际接收的数据长度（0表示无数据）
  */
uint8_t nrf24l01_receive_packet(uint8_t *data, uint8_t len)
{
    uint8_t sta;
    uint8_t rx_len = 0;
    
    /* 参数检查 */
    if (data == NULL || len == 0) {
        return 0;
    }
    
    /* 读取状态 */
    sta = nrf24l01_read_reg(STATUS);
    
    if (sta & RX_OK) {
        /* 有数据接收 */
        user_nrf_ce_write(0);
        
        /* 确定接收长度 */
        rx_len = (len > RX_PLOAD_WIDTH) ? RX_PLOAD_WIDTH : len;
        
        /* 读取数据 */
        nrf24l01_read_buf(RD_RX_PLOAD, data, rx_len);
        
        /* 清除RX FIFO */
        nrf24l01_write_reg(FLUSH_RX, 0xFF);
        
        /* 清除中断标志 */
        nrf24l01_write_reg(NRF_WRITE_REG + STATUS, sta);
        
        user_nrf_ce_write(1);
        
        return rx_len;
    }
    
    return 0;
}

/* ========================= 用户需要实现的函数 ========================= */
/**
  * @brief  GPIO初始化（用户必须实现）
  * @param  无
  * @retval 无
  * @note   此函数需要用户根据实际硬件实现
  *         需要初始化CE、CS、SCK、MOSI、MISO、IRQ引脚
  */
void user_nrf_gpio_init(void)
{
    /* 此函数需要用户根据实际硬件实现 */
}

/**
  * @brief  设置CE引脚电平（用户必须实现）
  * @param  level : 0-低电平，1-高电平
  * @retval 无
  */
void user_nrf_ce_write(uint8_t level)
{
    /* 此函数需要用户根据实际硬件实现 */
    /* 示例：HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, level ? GPIO_PIN_SET : GPIO_PIN_RESET); */
}

/**
  * @brief  设置CS引脚电平（用户必须实现）
  * @param  level : 0-低电平，1-高电平
  * @retval 无
  */
void user_nrf_cs_write(uint8_t level)
{
    /* 此函数需要用户根据实际硬件实现 */
    /* 示例：HAL_GPIO_WritePin(NRF_CS_GPIO_Port, NRF_CS_Pin, level ? GPIO_PIN_SET : GPIO_PIN_RESET); */
}

/**
  * @brief  设置SCK引脚电平（用户必须实现）
  * @param  level : 0-低电平，1-高电平
  * @retval 无
  */
void user_nrf_sck_write(uint8_t level)
{
    /* 此函数需要用户根据实际硬件实现 */
    /* 示例：HAL_GPIO_WritePin(NRF_SCK_GPIO_Port, NRF_SCK_Pin, level ? GPIO_PIN_SET : GPIO_PIN_RESET); */
}

/**
  * @brief  设置MOSI引脚电平（用户必须实现）
  * @param  level : 0-低电平，1-高电平
  * @retval 无
  */
void user_nrf_mosi_write(uint8_t level)
{
    /* 此函数需要用户根据实际硬件实现 */
    /* 示例：HAL_GPIO_WritePin(NRF_MOSI_GPIO_Port, NRF_MOSI_Pin, level ? GPIO_PIN_SET : GPIO_PIN_RESET); */
}

/**
  * @brief  读取MISO引脚电平（用户必须实现）
  * @param  无
  * @retval uint8_t : 0-低电平，1-高电平
  */
uint8_t user_nrf_miso_read(void)
{
    /* 此函数需要用户根据实际硬件实现 */
    /* 示例：return HAL_GPIO_ReadPin(NRF_MISO_GPIO_Port, NRF_MISO_Pin); */
    return 0;
}

/**
  * @brief  读取IRQ引脚电平（用户必须实现）
  * @param  无
  * @retval uint8_t : 0-低电平，1-高电平
  */
uint8_t user_nrf_irq_read(void)
{
    /* 此函数需要用户根据实际硬件实现 */
    /* 示例：return HAL_GPIO_ReadPin(NRF_IRQ_GPIO_Port, NRF_IRQ_Pin); */
    return 0;
}

/**
  * @brief  微秒延时（用户必须实现）
  * @param  us : 延时微秒数
  * @retval 无
  */
void user_delay_us(uint32_t us)
{
    /* 此函数需要用户根据实际硬件实现 */
    /* 示例：
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 1000000 * us);
    while (delay--) {
        __NOP();
    }
    */
}

/**
  * @brief  毫秒延时（用户必须实现）
  * @param  ms : 延时毫秒数
  * @retval 无
  */
void user_delay_ms(uint32_t ms)
{
    /* 此函数需要用户根据实际硬件实现 */
    /* 示例：HAL_Delay(ms); */
}