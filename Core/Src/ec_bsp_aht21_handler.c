#include "ec_bsp_aht21_handler.h"
#include "FreeRTOS.h"
#include "task.h"
/**
 * @brief 初始化 AHT21 温湿度传感器模块的实例
 *
 * 该函数使用提供的配置结构来设置 AHT21 传感器处理程序实例，
 * 确保处理程序和传感器实例被正确初始化并准备好使用。
 *
 * @param bsp_AHT21_handler_arg_struct *bsp_AHT21_handler_arg_instance
 *          指向 AHT21 处理程序参数结构的指针。该结构应包含初始化 AHT21 传感器
 *          所需的配置参数。
 *
 * @param bsp_aht21_handler_t *aht21_handler_instance
 *          指向 AHT21 处理程序实例的指针。该结构用于管理 AHT21 传感器的操作，
 *          包括数据采集和与传感器的通信。
 *
 * @param bsp_aht21_t *aht21_instance
 *          指向 AHT21 传感器实例的指针。该结构表示物理传感器，并用于存储传感器
 *          特定的信息和状态。
 *
 * @return int8_t
 *          如果初始化成功返回 0，如果初始化过程中出现故障则返回负值错误代码。
 */

/**
 * @param bsp_AHT21_handler_arg_struct  bsp_AHT21_handler_arg_struct 实例
 * @attention 这个接口提供给OS 来进行Handler初始化
 * @return 0 表示成功，其他值表示失败
 */
void temp_humi_handler_thread(bsp_AHT21_handler_arg_struct *bsp_AHT21_handler_arg_instance)
{
    // 驱动实例声明
    bsp_aht21_t *aht21_instance;
    // 结构体声明
    bsp_aht21_handler_t aht21_handler_instance = {
        .insted = false,
        .inited = false,
        .init_lock = NULL,
        .get_temp_humi_data_lock = NULL,
        .temp = NULL,
        .humi = NULL,
        .lifetimes_temp = NULL,
        .lifetimes_humi = NULL,
        .queue_event = NULL,
        .aht21_instance = NULL,
        .thread_os = NULL
        };
        aht21_handler_instance.aht21_instance = aht21_instance;
    // 调用handler构造函数
    int8_t code = aht21_handler_inst(bsp_AHT21_handler_arg_instance, &aht21_handler_instance, &aht21_instance);
    if (code!= RET_CODE_SUCCESS)
    {
        // 构造失败 执行逆初始化和解构函数
        aht21_handler_deinit(&aht21_handler_instance, &aht21_instance);
        aht21_handler_deinst(&aht21_handler_instance, &aht21_instance);
        return code;
    }
    // 创建任务
    for (;;)
    {
        while (event)
    }
}

static int8_t aht21_handler_inst(bsp_AHT21_handler_arg_struct *bsp_AHT21_handler_arg_instance,
                                 bsp_aht21_handler_t *bsp_aht21_handler_instance,
                                 bsp_aht21_t *aht21_instance)
{
    // 检验是否已完成构造
    if (bsp_aht21_handler_instance->inited != false)
    {
        // 已完成初始化
        return RET_CODE_HAS_BEEN_INITED;
    }
    // 入参校验
    if (NULL == bsp_AHT21_handler_arg_instance ||
        NULL == bsp_AHT21_handler_arg_instance->iic_driver_interface_table ||
        NULL == bsp_AHT21_handler_arg_instance->rtos_yeild ||
        NULL == bsp_AHT21_handler_arg_instance->timebase)
    {
        return RET_CODE_ERROR_PARAM_NULL;
    }
    // 进行handler初始化 完成模块驱动实例挂载
    int8_t code = aht21_handler_init(&bsp_AHT21_handler_arg_instance, &bsp_aht21_handler_instance);
    if (code != 0)
    {
        // init failure
        return code;
    }
    bsp_aht21_handler_instance->thread_os = true;
    // inst success
    return RET_CODE_SUCCESS;
}

/**
 * @brief 初始化AHT21 Handler
 *
 * 这个函数初始化AHT21传感器实例以及相关的操作系统资源。
 *
 * @param bsp_AHT21_handler_arg_struct  bsp_AHT21_handler_arg_instance 实例
 * @return 参考ret_codes.h
 *
 */
static int8_t aht21_handler_init(bsp_AHT21_handler_arg_struct *bsp_AHT21_handler_arg_instance, bsp_aht21_handler_t *bsp_aht21_handler_instance)
{
    // 检查是否初始化
    if (bsp_aht21_handler_instance->inited != false)
    {
        return RET_CODE_HAS_BEEN_INITED;
    }
    // 入参校验
    if (NULL == bsp_AHT21_handler_arg_instance ||
        NULL == bsp_AHT21_handler_arg_instance->iic_driver_interface_table ||
        NULL == bsp_AHT21_handler_arg_instance->rtos_yeild ||
        NULL == bsp_AHT21_handler_arg_instance->timebase)
    {
        // 参数为空
        return RET_CODE_ERROR_PARAM_NULL;
    }

    // 初始化队列
    bsp_aht21_handler_instance->queue_event = xQueueCreate(10, sizeof(int));
    if (NULL == bsp_aht21_handler_instance->queue_event)
    {
        return RET_CODE_QUEUE_EVENT_NULL;
    }

    // 获取信号量
    if (xSemaphoreTake(bsp_aht21_handler_instance->init_lock, portMAX_DELAY) != pdTRUE)
    {
        vSemaphoreDelete(bsp_aht21_handler_instance->init_lock);
        vQueueDelete(bsp_aht21_handler_instance->queue_event);
        return RET_CODE_XSEMAPHORETAKE_FAIL;
    }
    xSemaphoreGive(bsp_aht21_handler_instance->init_lock);
    // 调用AHT21驱动构造函数
    int8_t code = aht21_inst(bsp_aht21_handler_instance->aht21_instance, bsp_AHT21_handler_arg_instance->iic_driver_interface_table, bsp_AHT21_handler_arg_instance->timebase, bsp_AHT21_handler_arg_instance->rtos_yeild);
    // 构造失败
    if (code != 0)
    {
        // 温湿度模块构造失败
        return code;
    }

    // AHT21_driver INIT
    code = aht21_init(bsp_aht21_handler_instance->aht21_instance);
    if (code != 0)
    {
        // init failure
        return code;
    }

    // 初始化成功
    bsp_aht21_handler_instance->inited = true;
    return RET_CODE_SUCCESS;
}

/**
 * @brief 解构AHT21 Handler
 *
 * 这个函数初始化AHT21传感器实例以及相关的操作系统资源。
 *
 * @param handler AHT21 Handler 实例
 * @return 0 表示成功，其他值表示失败
 */
static int8_t aht21_handler_deInst(bsp_aht21_handler_t *handler)
{
    if (NULL != handler)
    {
        //先执行逆初始化
        aht21_handler_deInit(handler);
        //自身属性置空
        handler->insted = false,
        handler->inited = false,
        handler->init_lock = NULL,
        handler->get_temp_humi_data_lock = NULL,
        handler->temp = NULL,
        handler->humi = NULL,
        handler->lifetimes_temp = NULL,
        handler->lifetimes_humi = NULL,
        handler->queue_event = NULL,
        handler->aht21_instance = NULL,
        handler->thread_os = NULL;
        if (NULL != handler->aht21_instance)
        {
            // 逆初始化已包含解构函数
            aht21_deInit(handler->aht21_instance);
        }
    }
    handler=NULL;
    return RET_CODE_SUCCESS;
}

/**
 * @brief 解构AHT21 Handler
 *
 * 这个函数初始化AHT21传感器实例以及相关的操作系统资源。
 *
 * @param handler AHT21 Handler 实例
 * @return 0 表示成功，其他值表示失败
 */
static int8_t aht21_handler_deInit(bsp_aht21_handler_t *bsp_aht21_handler_instance)
{
    if (NULL != bsp_aht21_handler_instance)
    {
        vSemaphoreDelete(bsp_aht21_handler_instance->init_lock);
        vQueueDelete(bsp_aht21_handler_instance->queue_event);
    }
    bsp_aht21_handler_instance=NULL;
    return RET_CODE_SUCCESS;
}