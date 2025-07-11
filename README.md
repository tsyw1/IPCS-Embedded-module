# IPCS-Embedded-Module

## 简介

西南科技大学智能系统与智慧服务创新实践班嵌入式通用模块库，用于全国大学生电子设计大赛。

## 本仓库使用方法

1. 克隆仓库
```bash
git clone https://github.com/nanwanuser/IPCS-Embedded-Module.git
```

2. 将需要的模块文件复制到工程目录

3. 根据模块内的README进行配置和使用

## 模块编写规范(开发人员请注意)

### 1. 模块命名规范

- **文件命名**: `模块名.c` 和 `模块名.h`（尽量对应英文单词，不使用汉语拼音）
- **函数命名**: `模块名_功能动词()`
- **变量命名**: 使用下划线分隔的小写字母

**示例：**
```c
// 文件名
data_communication_pkg.c
data_communication_pkg.h

// 函数名
data_comm_init()
data_comm_send()
data_comm_parse_byte()

// 变量名
pkg_length, data_index
```

### 2. 代码编写规范

#### 参数配置规范(示例)
用户需要适配的参数和频繁调用的变量使用宏定义：

```c
/* ========================= 用户配置参数区 ========================= */
#define FRAME_HEADER      0xAA55      // 帧头（2字节）
#define FRAME_END         0x55AA      // 帧尾（2字节）
#define MAX_DATA_LENGTH   256         // 最大数据载荷长度（字节）
#define USE_CRC16         1           // 是否启用CRC16校验（0-禁用，1-启用）
```

#### 数据类型定义规范(示例)
统一放置在头文件中，比如使用enum和typedef：

```c
/* ========================= 数据类型定义 ========================= */
int xxxxxxxx;
double xxxxxxxxxx;
typedef enum {
    PKG_OK = 0,           // 数据包正确
    PKG_HEADER_ERR,       // 帧头错误
    PKG_LENGTH_ERR,       // 长度错误
    PKG_CRC_ERR,          // CRC校验错误
    PKG_END_ERR           // 帧尾错误
} PkgStatus;
```

#### API函数接口规范(示例)
- 初始化函数：`模块名_init()`
- 功能函数：`模块名_动词()`
- 回调函数：`模块名_callback()` 或 `user_功能()`

```c
/* ========================= API函数接口 ========================= */
void data_comm_init(void);
uint16_t data_comm_send(uint8_t cmd, uint8_t *data, uint16_t len);
void data_comm_parse_byte(uint8_t byte);
```

#### 模块解耦规范(示例)
为适配不同外设，定义用户实现接口：

```c
/* ========================= 用户实现接口 ========================= */
/**
  * @brief  数据发送函数（用户必须实现）
  * @note   用户需根据实际硬件（UARTx/SPIx/I2Cx等）实现此函数
  */
void user_transmit(uint8_t *data, uint16_t len)
{   
    //示例
    //HAL_UART_Transmit(&huart1, data, len, 100);
}

/**
  * @brief  数据包接收回调函数（用户必须实现）
  * @note   当接收到完整数据包时，此函数会被自动调用
  */
void user_packet_handler(uint8_t cmd, uint8_t *data, uint16_t len)
{
    //示例
    //用户实现
}
```

#### 注释规范
使用标准Doxygen格式：

```c
/**
  * @brief  函数简要说明
  * @param  参数名 : 参数说明
  * @retval 返回值说明
  * @note   注意事项或补充说明
  */
```

**代码段注释：**
```c
/* ========================= 私有类型定义 ========================= */
/* ========================= 用户配置参数区 ========================= */
/* ========================= API函数实现 ========================= */
```

## README 编写规范(开发人员请注意)

每个模块的README应包含以下结构：

### 1. 模块简介
- 模块功能描述
- 主要特性列表
- 适用场景

### 2. API函数接口
- 以下仅为举例：
1. 初始化函数说明
2. 核心功能函数说明
3. 用户实现接口说明

### 3. 使用示例
- 以下仅为举例：
1. 初始化示例
2. 用户实现函数示例  
3. 实际应用场景示例

**README模板：**
```markdown
# 模块名称

## 模块简介
功能描述和主要特性

## API函数接口(示例，可能存在其他类型的函数)
### 1. 初始化函数
### 2. 核心功能函数
### 3. 用户实现接口

## 使用示例
### 1. 初始化
### 2. 用户实现函数
### 3. 实际应用
```

## 代码提交步骤(开发人员请注意)
1. Fork本仓库
2. git clone 已经fork的仓库
3. 创建新分支 (`git checkout -b feature/your-module-name`)
4. 提交更改 (`git commit -m 'Add: 模块描述'`)
5. 推送到分支 (`git push origin feature/your-module-name`)
6. 创建Pull Request

## 开源许可证

MIT License