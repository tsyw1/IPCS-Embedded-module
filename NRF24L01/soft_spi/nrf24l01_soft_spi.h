/**
  ******************************************************************************
  * @file    nrf24l01_soft_spi.h
  * @brief   NRF24L01无线模块软件SPI驱动头文件
  * @version V1.0.0
  * @date    2025-01-10
  ******************************************************************************
  */

#ifndef __NRF24L01_SOFT_SPI_H
#define __NRF24L01_SOFT_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ========================= 用户配置参数区 ========================= */
/**
  * @brief  NRF24L01参数配置
  * @note   用户可根据实际需求修改以下参数
  */
#define TX_ADR_WIDTH    5     // 发送地址宽度（字节）
#define RX_ADR_WIDTH    5     // 接收地址宽度（字节）
#define TX_PLOAD_WIDTH  32    // 发送数据宽度（字节）
#define RX_PLOAD_WIDTH  32    // 接收数据宽度（字节）

/* 默认配置参数 */
#define NRF_CHANNEL_RX  0x14  // 接收信道（0-127）
#define NRF_CHANNEL_TX  0x14  // 发送信道（0-127）
#define NRF_SPEED      0x06   // 无线速率：0x06-1Mbps，0x0E-2Mbps

/* ========================= 寄存器定义 ========================= */
/* NRF24L01指令 */
#define NRF_READ_REG    0x00  // 读配置寄存器，低5位为寄存器地址
#define NRF_WRITE_REG   0x20  // 写配置寄存器，低5位为寄存器地址
#define RD_RX_PLOAD     0x61  // 读RX有效数据，1~32字节
#define WR_TX_PLOAD     0xA0  // 写TX有效数据，1~32字节
#define FLUSH_TX        0xE1  // 清除TX FIFO寄存器
#define FLUSH_RX        0xE2  // 清除RX FIFO寄存器
#define REUSE_TX_PL     0xE3  // 重新使用上一包数据
#define NOP             0xFF  // 空操作，可以用来读状态寄存器

/* NRF24L01寄存器地址 */
#define CONFIG          0x00  // 配置寄存器
#define EN_AA           0x01  // 使能自动应答功能
#define EN_RXADDR       0x02  // 接收地址允许
#define SETUP_AW        0x03  // 设置地址宽度
#define SETUP_RETR      0x04  // 建立自动重发
#define RF_CH           0x05  // RF通道
#define RF_SETUP        0x06  // RF寄存器
#define STATUS          0x07  // 状态寄存器
#define OBSERVE_TX      0x08  // 发送检测寄存器
#define CD              0x09  // 载波检测寄存器
#define RX_ADDR_P0      0x0A  // 数据通道0接收地址
#define RX_ADDR_P1      0x0B  // 数据通道1接收地址
#define RX_ADDR_P2      0x0C  // 数据通道2接收地址
#define RX_ADDR_P3      0x0D  // 数据通道3接收地址
#define RX_ADDR_P4      0x0E  // 数据通道4接收地址
#define RX_ADDR_P5      0x0F  // 数据通道5接收地址
#define TX_ADDR         0x10  // 发送地址
#define RX_PW_P0        0x11  // 接收数据通道0有效数据宽度
#define RX_PW_P1        0x12  // 接收数据通道1有效数据宽度
#define RX_PW_P2        0x13  // 接收数据通道2有效数据宽度
#define RX_PW_P3        0x14  // 接收数据通道3有效数据宽度
#define RX_PW_P4        0x15  // 接收数据通道4有效数据宽度
#define RX_PW_P5        0x16  // 接收数据通道5有效数据宽度
#define NRF_FIFO_STATUS 0x17  // FIFO状态寄存器

/* 状态寄存器位定义 */
#define MAX_TX          0x10  // 达到最大发送次数中断
#define TX_OK           0x20  // TX发送完成中断
#define RX_OK           0x40  // 接收到数据中断

/* ========================= 数据类型定义 ========================= */
/**
  * @brief  NRF24L01通信状态枚举
  */
typedef enum {
    NRF_OK = 0,           // 操作成功
    NRF_ERROR,            // 操作失败
    NRF_TIMEOUT,          // 操作超时
    NRF_NOT_FOUND         // 设备未找到
} NrfStatus;

/**
  * @brief  NRF24L01工作模式枚举
  */
typedef enum {
    NRF_MODE_TX = 0,      // 发送模式
    NRF_MODE_RX           // 接收模式
} NrfMode;

/**
  * @brief  NRF24L01配置结构体
  */
typedef struct {
    uint8_t channel;      // 通信信道（0-127）
    uint8_t speed;        // 通信速率（0x06:1Mbps, 0x0E:2Mbps）
    uint8_t tx_addr[TX_ADR_WIDTH];  // 发送地址
    uint8_t rx_addr[RX_ADR_WIDTH];  // 接收地址
} NrfConfig;

/* ========================= API函数接口 ========================= */
/**
  * @brief  初始化NRF24L01模块
  * @param  config : 配置参数指针（为NULL时使用默认配置）
  * @retval NrfStatus : 初始化状态
  */
NrfStatus nrf24l01_init(NrfConfig *config);

/**
  * @brief  检查NRF24L01是否存在
  * @param  无
  * @retval NrfStatus : NRF_OK-存在，NRF_NOT_FOUND-不存在
  */
NrfStatus nrf24l01_check(void);

/**
  * @brief  设置工作模式
  * @param  mode : 工作模式（NRF_MODE_TX/NRF_MODE_RX）
  * @retval 无
  */
void nrf24l01_set_mode(NrfMode mode);

/**
  * @brief  发送数据包
  * @param  data : 数据缓冲区
  * @param  len  : 数据长度（1-32字节）
  * @retval NrfStatus : 发送状态
  */
NrfStatus nrf24l01_send_packet(uint8_t *data, uint8_t len);

/**
  * @brief  接收数据包
  * @param  data : 数据缓冲区
  * @param  len  : 缓冲区长度
  * @retval uint8_t : 实际接收的数据长度（0表示无数据）
  */
uint8_t nrf24l01_receive_packet(uint8_t *data, uint8_t len);

/**
  * @brief  读取寄存器
  * @param  reg : 寄存器地址
  * @retval uint8_t : 寄存器值
  */
uint8_t nrf24l01_read_reg(uint8_t reg);

/**
  * @brief  写入寄存器
  * @param  reg   : 寄存器地址
  * @param  value : 寄存器值
  * @retval uint8_t : 状态寄存器值
  */
uint8_t nrf24l01_write_reg(uint8_t reg, uint8_t value);

/**
  * @brief  读取缓冲区
  * @param  reg  : 寄存器地址
  * @param  buf  : 数据缓冲区
  * @param  len  : 数据长度
  * @retval uint8_t : 状态寄存器值
  */
uint8_t nrf24l01_read_buf(uint8_t reg, uint8_t *buf, uint8_t len);

/**
  * @brief  写入缓冲区
  * @param  reg  : 寄存器地址
  * @param  buf  : 数据缓冲区
  * @param  len  : 数据长度
  * @retval uint8_t : 状态寄存器值
  */
uint8_t nrf24l01_write_buf(uint8_t reg, uint8_t *buf, uint8_t len);

/* ========================= 用户实现接口 ========================= */
/**
  * @brief  GPIO初始化（用户必须实现）
  * @param  无
  * @retval 无
  * @note   初始化CE、CS、SCK、MOSI、MISO、IRQ引脚
  */
void user_nrf_gpio_init(void);

/**
  * @brief  设置CE引脚电平（用户必须实现）
  * @param  level : 0-低电平，1-高电平
  * @retval 无
  */
void user_nrf_ce_write(uint8_t level);

/**
  * @brief  设置CS引脚电平（用户必须实现）
  * @param  level : 0-低电平，1-高电平
  * @retval 无
  */
void user_nrf_cs_write(uint8_t level);

/**
  * @brief  设置SCK引脚电平（用户必须实现）
  * @param  level : 0-低电平，1-高电平
  * @retval 无
  */
void user_nrf_sck_write(uint8_t level);

/**
  * @brief  设置MOSI引脚电平（用户必须实现）
  * @param  level : 0-低电平，1-高电平
  * @retval 无
  */
void user_nrf_mosi_write(uint8_t level);

/**
  * @brief  读取MISO引脚电平（用户必须实现）
  * @param  无
  * @retval uint8_t : 0-低电平，1-高电平
  */
uint8_t user_nrf_miso_read(void);

/**
  * @brief  读取IRQ引脚电平（用户必须实现）
  * @param  无
  * @retval uint8_t : 0-低电平，1-高电平
  */
uint8_t user_nrf_irq_read(void);

/**
  * @brief  微秒延时（用户必须实现）
  * @param  us : 延时微秒数
  * @retval 无
  */
void user_delay_us(uint32_t us);

/**
  * @brief  毫秒延时（用户必须实现）
  * @param  ms : 延时毫秒数
  * @retval 无
  */
void user_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* __NRF24L01_SOFT_SPI_H */