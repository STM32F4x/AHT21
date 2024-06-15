/**
 * @file ec_bsp_aht21_handler.h
 * @brief AHT21 温湿度传感器处理程序头文件
 *
 * 这个文件包含用于控制和操作AHT21温湿度传感器与操作系统交互的函数声明和结构体定义。
 * 提供了初始化、启动测量、读取数据和复位等功能。
 *
 * @version 1.0
 * @date 2024-06-06
 *
 * @note
 * - 确保I2C库和操作系统已经初始化并配置正确。
 * - 所有函数都假定传感器的I2C地址为0x38（默认地址）。
 * - 读取数据函数返回的温湿度值为浮点数，单位分别是摄氏度和百分比。
 *
 * @par 依赖项
 * - aht21.h : 包含AHT21传感器驱动的头文件。
 * - os.h : 包含操作系统相关函数的头文件。
 *
 * @par 版本历史
 * - 1.0 初始版本
 *
 * @par 作者
 * - liyijie
 */

#ifndef EC_BSP_AHT21_HANDLER_H
#define EC_BSP_AHT21_HANDLER_H

#include "ec_bsp_aht21_driver.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_codes.h"
// 请求数据类型枚举
typedef enum
{
    TEMP_HUMI_EVENT_TYPE_TEMP = 0,
    TEMP_HUMI_EVENT_TYPE_HUMI,
    TEMP_HUMI_EVENT_TYPE_BOTH,
} temp_humi_t;

// 请求事件结构体
typedef struct
{
    float *temp;
    float *humi;
    uint32_t *lifetime;
    uint32_t *timestamp;
    temp_humi_t type_of_data;
    void (*callback)(float *, float *);
} temp_humi_event_t;

// 提供给RTOS初始化结构体参数
typedef struct
{
    // Core层提供的接口
    iic_driver_interface_t *iic_driver_interface_table; // IIC的实体实例
    system_timebase_interface_t *timebase;              // 时基
    // RTOS提供的接口
    void *rtos_yeild; // 操作系统切换
} bsp_AHT21_handler_arg_struct;

// 提前定义bsp_aht21_handler_t防止报错
typedef struct bsp_aht21_handler_t bsp_aht21_handler_t;
/**
 * @brief AHT21 Handler 结构体
 *
 * 这个结构体包含AHT21传感器实例以及与操作系统交互所需的资源。
 */
struct bsp_aht21_handler_t
{
    // Core层提供的接口
    iic_driver_interface_t *iic_driver_interface_table; // IIC的实体实例
    system_timebase_interface_t *timebase;              // 时基
    // RTOS提供的接口
    temp_humi_event_t *temp_humi_event_instance; // 请求事件实例
    void *rtos_yeild;                            // 操作系统切换
    // BSP提供的接口
    bsp_aht21_t *aht21_instance;                                 // AHT21传感器实例
    
    int8_t (*pfdeInit)(bsp_aht21_handler_t *bsp_aht21_handler_instance); // 逆初始化
    // 自身构造函数
    int8_t (*pfInst)(bsp_AHT21_handler_arg_struct *bsp_AHT21_handler_arg_instance,
                     bsp_aht21_handler_t *aht21_handler_instance,
                     bsp_aht21_t *aht21_instance);
    // 面向rtos接口                                                 // 初始化
    int8_t (*pfaht21_handler_getTemp_humi_Data)(bsp_aht21_handler_t *aht21_handler_instance, temp_humi_event_t *temp_humi_event_instance);

    //#内部交互
    // lock
    void * insted;
    void * inited;
    void * init_lock;
    void * get_temp_humi_data_lock;
    void * temp;
    void * humi;
    void * lifetimes_temp;
    void * lifetimes_humi;
    void * queue_event;
    void * thread_os;
};

/**
 * @brief 构造AHT21 Handler
 *
 * 这个函数初始化AHT21传感器实例以及相关的操作系统资源。
 *
 * @param handler AHT21 Handler 实例
 * @param aht21_instance AHT21传感器实例
 * @param iic_instance I2C驱动接口实例
 * @param timebase 系统时基接口实例
 * @param rtos_yeild 操作系统任务切换函数
 *
 * @return 0 表示成功，其他值表示失败
 */
static int8_t aht21_handler_inst(bsp_AHT21_handler_arg_struct *bsp_AHT21_handler_arg_instance,
                                 bsp_aht21_handler_t *aht21_handler_instance,
                                 bsp_aht21_t *aht21_instance);

/**
 * @brief 解构AHT21 Handler
 *
 * 这个函数初始化AHT21传感器实例以及相关的操作系统资源。
 *
 * @param handler AHT21 Handler 实例
 * @return 0 表示成功，其他值表示失败
 */
static int8_t aht21_handler_deInst(bsp_aht21_handler_t *handler);

/**
 * @brief 初始化AHT21 Handler
 *
 * 这个函数初始化AHT21传感器实例以及相关的操作系统资源。
 *
 * @param handler AHT21 Handler 实例
 * @return 0 表示成功，其他值表示失败
 */
static int8_t aht21_handler_init(bsp_AHT21_handler_arg_struct *bsp_AHT21_handler_arg_instance, bsp_aht21_handler_t *bsp_aht21_handler_instance);

/**
 * @brief 逆初始化AHT21 Handler
 *
 * 这个函数初始化AHT21传感器实例以及相关的操作系统资源。
 *
 * @param handler AHT21 Handler 实例
 * @return 0 表示成功，其他值表示失败
 */
static int8_t aht21_handler_deInit(bsp_aht21_handler_t *handler);

/**
 * @brief 启动AHT21测量
 *
 * 这个函数获取AHT21传感器的温湿度测量。
 *
 * @param aht21_handler_instance AHT21 Handler 实例
 * @param temp_humi_event_instance temp_humi_event_t实例
 * @param humi 指向保存湿度数据的浮点数指针
 *
 * @return 0 表示成功，其他值表示失败
 */
static int8_t aht21_handler_getTemp_humi_Data(bsp_aht21_handler_t *aht21_handler_instance, temp_humi_event_t *temp_humi_event_instance);

/**
 * @param aht21_handler_instance AHT21 Handler 实例
 * @attention 这个接口用来构造温湿度模块实例
 * @return 0 表示成功，其他值表示失败
 */
static int8_t inst_BSP_driver_instance(bsp_aht21_handler_t *aht21_handler_instance); // 尝试构造驱动实例

/**
 * @param bsp_AHT21_handler_arg_struct  bsp_AHT21_handler_arg_struct 实例
 * @attention 这个接口提供给OS 来进行Handler初始化
 * @return 0 表示成功，其他值表示失败
 */
void temp_humi_handler_thread(bsp_AHT21_handler_arg_struct *bsp_AHT21_handler_arg_instance); // 被操作系统调用的

int8_t temp_humi_event_handler_send(temp_humi_event_t *event);

#endif
