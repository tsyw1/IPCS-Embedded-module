/**
  ******************************************************************************
  * @file    data_communication_pkg.h
  * @brief   通用数据通信协议库头文件
  * @version V2.0.0
  * @date    2025-01-10
  ******************************************************************************
  */

#ifndef __DATA_COMMUNICATION_PKG_H
#define __DATA_COMMUNICATION_PKG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ========================= 用户配置参数区 ========================= */
/**
  * @brief  协议参数配置
  * @note   用户可根据实际需求修改以下参数
  */
#define FRAME_HEADER      0xAA55      // 帧头（2字节）
#define FRAME_END         0x55AA      // 帧尾（2字节）
#define MAX_DATA_LENGTH   256         // 最大数据载荷长度（字节）
#define USE_CRC16         1           // 是否启用CRC16校验（0-禁用，1-启用）

/* ========================= 数据类型定义 ========================= */
/**
  * @brief  数据包状态枚举
  */
typedef enum {
    PKG_OK = 0,           // 数据包正确
    PKG_HEADER_ERR,       // 帧头错误
    PKG_LENGTH_ERR,       // 长度错误
    PKG_CRC_ERR,          // CRC校验错误
    PKG_END_ERR           // 帧尾错误
} PkgStatus;

/* ========================= API函数接口 ========================= */
/**
  * @brief  初始化通信协议模块
  * @param  无
  * @retval 无
  */
void data_comm_init(void);

/**
  * @brief  打包并发送数据
  * @param  cmd  : 命令字节（1字节）
  * @param  data : 数据载荷指针
  * @param  len  : 数据载荷长度（0~MAX_DATA_LENGTH）
  * @retval 实际发送的字节数
  */
uint16_t data_comm_send(uint8_t cmd, uint8_t *data, uint16_t len);

/**
  * @brief  解析接收到的单个字节
  * @param  byte : 接收到的字节
  * @retval 无
  * @note   将接收到的每个字节传入此函数，内部自动进行协议解析
  */
void data_comm_parse_byte(uint8_t byte);

/* ========================= 用户实现接口 ========================= */
/**
  * @brief  数据发送函数（用户必须实现）
  * @param  data : 待发送数据缓冲区
  * @param  len  : 数据长度
  * @retval 无
  * @note   用户需根据实际硬件（UART/SPI/I2C等）实现此函数
  */
void user_transmit(uint8_t *data, uint16_t len);

/**
  * @brief  数据包接收回调函数（用户必须实现）
  * @param  cmd  : 接收到的命令字节
  * @param  data : 数据载荷缓冲区
  * @param  len  : 数据载荷长度
  * @retval 无
  * @note   当接收到完整数据包时，此函数会被自动调用
  */
void user_packet_handler(uint8_t cmd, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __DATA_COMMUNICATION_PKG_H */