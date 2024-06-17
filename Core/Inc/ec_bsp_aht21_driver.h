/**
 * @file aht21.h
 * @brief AHT21 温湿度传感器驱动头文件
 *
 * 这个文件包含用于控制和操作AHT21温湿度传感器的函数声明和结构体定义。
 * 提供了初始化、启动测量、读取数据和复位等功能。
 * 
 * @version 1.0
 * @date 2024-05-26
 * 
 * 使用这个头文件需要一个I2C通信库，该库必须提供基本的I2C读写功能。
 * 
 * @note
 * - 确保I2C库已经初始化并配置正确。
 * - 所有函数都假定传感器的I2C地址为0x38（默认地址）。
 * - 读取数据函数返回的温湿度值为浮点数，单位分别是摄氏度和百分比。
 * 
 * @par 依赖项
 * - i2c.h : 包含I2C通信函数的头文件。
 * 
 * @par 版本历史
 * - 1.0 初始版本
 * 
 * @par 作者
 * - liyijie
 */

#ifndef __EC_BSP_AHT21_DRIVER_H__
#define __EC_BSP_AHT21_DRIVER_H__

#include "ec_bsp_aht21_reg.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "error_codes.h"

// AHT21 延时时间定义
#define AHT21_INIT_DELAY_MS          40
#define AHT21_MEASUREMENT_DELAY_MS   75
#define AHT21_RESET_DELAY_MS         20

//IIC函数指针结构体
typedef struct{
	int8_t (*pfInit)(void);
	int8_t (*pfDeInit)(void);
	//IIC接口补充
	int8_t (*pfStart)(void);
    int8_t (*pfStop)(void);
    int8_t (*pfWaitAck)(void);
    int8_t (*pfSendByte)(uint8_t byte);
    int8_t (*pfReadByte)(uint8_t *byte);
    int8_t (*pfSendAck)(void);
    int8_t (*pfSendNack)(void);

	int8_t (*pfWriteReg)(uint8_t addr, uint8_t *pdata, uint8_t size);
	int8_t (*pfReadReg)(uint8_t addr, uint8_t *pdata, uint8_t size);
}iic_driver_interface_t;

//定义时基
typedef struct 
{
	uint32_t (*mcu_get_systick_count)(void);
}system_timebase_interface_t;
//提前定义aht21_instance防止报错
typedef struct  bsp_aht21_t bsp_aht21_t;
//AHT21函数指针结构体
struct bsp_aht21_t{

	iic_driver_interface_t	*iic_driver_interface_t;  						// IIC接口
	system_timebase_interface_t   *pftimebase_interface;         			// 时基接口

	int8_t (*pfInst)(                                             // #1 初始化函数
					bsp_aht21_t * 			      aht21_instance, // AHT21的实体实例

					iic_driver_interface_t *      iic_instance,   // IIC的实体实例
					
					system_timebase_interface_t * timebase,       // 时基
					
					void * rtos_yeild                           );// 操作系统切换;
	int8_t (*pfinit)(bsp_aht21_t *aht21_instance);
	int8_t (*pfdeInit)(bsp_aht21_t *aht21_instance);
	int8_t (*pfaht21_read_id)(bsp_aht21_t *aht21_instance);
	int8_t (*pfstartMeasurement)(bsp_aht21_t *aht21_instance);
	int8_t (*pfaht21_read_data)(bsp_aht21_t *aht21_instance,float *temp,float *humi);
	int8_t (*pfsoftReset)(void);
	int8_t (*pfsleep)(void);
	int8_t (*pfwakeup)(void);
	int8_t (*pfyield)(bsp_aht21_t *aht21_instance);
};

/**
 * AHT21传感器初始化函数
 */
int8_t aht21_inst(                                                  // #1 初始化函数
				bsp_aht21_t * 			      aht21_instance,     // AHT21的实体实例
   
				iic_driver_interface_t *      iic_instance,       // IIC的实体实例
				   
				system_timebase_interface_t * timebase,           // 时基
				   
				void * rtos_yeild                           );	  // 操作系统切换

/**
 * AHT21实例解构
 */
int8_t aht21_deInst(bsp_aht21_t *aht21_instance);

/**
 * AHT21传感器逆初始化函数
 */
int8_t aht21_init(bsp_aht21_t *aht21_instance);
/**
 * AHT21传感器逆初始化函数
 */
int8_t aht21_deInit(bsp_aht21_t *aht21_instance);
/**
 * 读取id
 */
int8_t aht21_read_id(bsp_aht21_t *aht21_instance);
/**
 * 读取温度/温度
 */
int8_t aht21_read_data(bsp_aht21_t *aht21_instance,float *temp,float *humi);					      
/**
 * @brief 使AHT21传感器进入软件复位状态
 */
void aht21_softReset(void);
// 进入休眠模式
int8_t aht21_sleep(void);
// 唤醒WT588F
int8_t aht21_wakeup(void);
#endif
