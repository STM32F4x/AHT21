/**
 * @file aht21.c
 * @brief AHT21 温湿度传感器驱动源文件
 *
 * 这个文件包含用于控制和操作AHT21温湿度传感器的函数实现。
 * 提供了初始化、启动测量、读取数据和复位等功能。
 *
 * @version 1.0
 * @date 2024-05-26
 *
 * @note
 * - 确保I2C库已经初始化并配置正确。
 * - 所有函数都假定传感器的I2C地址为0x38（默认地址）。
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

#include "ec_bsp_aht21_driver.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
/**
 * @brief 构造AHT21传感器 对AHT21实例进行挂载和判空，并在必要时进行逆初始化。
 *
 * @description 构造AHT21传感器
 *
 * @param bsp_aht21_t *aht21_instance,     // AHT21的实体实例
 *       iic_driver_interface_t *      iic_instance,       // IIC的实体实例
 *		 ystem_timebase_interface_t * timebase,           // 时基
 *		 void * rtos_yeild    //系统任务切换
 *
 * @return 0 success
 *
 *		   -1 aht21_instance null
 *         -2 iic_instance null
 *         -3 timebase null
 *         -4 rtos_yeild null
 *         -5 AHT21 ADDR error
 **/
int8_t aht21_inst(bsp_aht21_t *aht21_instance, // AHT21的实体实例

                  iic_driver_interface_t *iic_instance, // IIC的实体实例

                  system_timebase_interface_t *timebase, // 时基

                  void *rtos_yeild) // 操作系统切换
{
    // 对参数进行判空
    if (aht21_instance == NULL)
    {
        return RET_CODE_ERROR_AHT21_INSTANCE_NULL;
    }
    if (iic_instance == NULL)
    {
        return RET_CODE_ERROR_IIC_INSTANCE_NULL;
    }
    if (timebase == NULL)
    {
        return RET_CODE_ERROR_TIMEBASE_NULL;
    }
    if (rtos_yeild == NULL)
    {
        return RET_CODE_ERROR_RTOS_YEILD_NULL;
    }
    // 一些全局变量声明
    static iic_driver_interface_t iic_driver_interface = {NULL, NULL, NULL, NULL};
    static bsp_aht21_t aht21_interface = {NULL, NULL, NULL, NULL, NULL, NULL};
    static system_timebase_interface_t timebase_interface = {NULL};
    static void *rtos_yeild_interface = NULL;
    // 对iic实例进行判空和挂载
    iic_driver_interface.pfDeInit = iic_instance->pfDeInit;
    iic_driver_interface.pfReadReg = iic_instance->pfReadReg;
    iic_driver_interface.pfWriteReg = iic_instance->pfWriteReg;
    iic_driver_interface.pfInit = iic_instance->pfInit;
    if (iic_driver_interface.pfDeInit == NULL ||
        iic_driver_interface.pfInit == NULL ||
        // iic_driver_interface.pfReadReg == NULL ||
        // iic_driver_interface.pfWriteReg == NULL ||
        iic_driver_interface.pfStart == NULL ||
        iic_driver_interface.pfStop == NULL ||
        iic_driver_interface.pfWaitAck == NULL ||
        iic_driver_interface.pfSendByte == NULL ||
        iic_driver_interface.pfReadByte == NULL
    )
    {
        // 解构
        aht21_deInst(aht21_instance);
        return RET_CODE_ERROR_PARAM_NULL;
    }

    // IIC挂载
    aht21_instance->iic_driver_interface_t->pfDeInit = iic_instance->pfDeInit;
    // aht21_instance->iic_driver_interface_t->pfReadReg = iic_instance->pfReadReg;
    // aht21_instance->iic_driver_interface_t->pfWriteReg = iic_instance->pfWriteReg;
    aht21_instance->iic_driver_interface_t->pfInit = iic_instance->pfInit;
    aht21_instance->iic_driver_interface_t->pfStart = iic_instance->pfStart;
    aht21_instance->iic_driver_interface_t->pfStop = iic_instance->pfStop;
    aht21_instance->iic_driver_interface_t->pfWaitAck = iic_instance->pfWaitAck;
    aht21_instance->iic_driver_interface_t->pfSendByte = iic_instance->pfSendByte;
    aht21_instance->iic_driver_interface_t->pfReadByte = iic_instance->pfReadByte;
    aht21_instance->iic_driver_interface_t->pfSendAck = iic_instance->pfSendAck;
    aht21_instance->iic_driver_interface_t->pfSendNack = iic_instance->pfSendNack;

    // 对时基单元进行判空和挂载
    timebase_interface.mcu_get_systick_count = timebase->mcu_get_systick_count;
    if (timebase_interface.mcu_get_systick_count == NULL)
    {
        // 解构
        aht21_deInst(aht21_instance);
        return RET_CODE_ERROR_TIMEBASE_NULL;
    }
    aht21_instance->pftimebase_interface = timebase;
    // 对rtos_yeild进行挂载和判空
    rtos_yeild_interface = rtos_yeild;
    if (rtos_yeild_interface == NULL)
    {
        // 解构
        aht21_deInst(aht21_instance);
        return RET_CODE_ERROR_RTOS_YEILD_NULL;
    }
    aht21_instance->pfyield = rtos_yeild;
    // AHT21实例方法判空与挂载
    aht21_instance->pfinit = aht21_init;
    aht21_instance->pfaht21_read_id = aht21_read_id;
    aht21_instance->pfdeInit = aht21_deInit;
    if (0x38 != aht21_read_id(aht21_instance))
    {
        aht21_deInst(aht21_instance);
        return RET_CODE_ERROR_TEPM_HUMI_MODLE_ADDR_ERROT;
    }
    aht21_instance->pfaht21_read_data = aht21_read_data;
    // 进行判空
    if (aht21_instance->pfdeInit == NULL ||
        aht21_instance->pfinit == NULL ||
        aht21_instance->pfaht21_read_id == NULL ||
        aht21_instance->pfaht21_read_data == NULL)
    {
        // 解构
        aht21_deInst(aht21_instance);
        return RET_CODE_ERROR_PARAM_NULL;
    }
    // 挂载成功
    return RET_CODE_SUCCESS;
}

/**
 * @brief  ATH21初始化 进行方法挂载 必要时进行逆初始化
 *
 * @param  aht21_instance *aht21_instance  aht21实例
 * @return 0 success
 *         -1 fail
 */
int8_t aht21_init(bsp_aht21_t *aht21_instance)
{
    // iic初始化
    aht21_instance->iic_driver_interface_t->pfInit();
    int32_t count = aht21_instance->pftimebase_interface->mcu_get_systick_count();
    if ((aht21_instance->pftimebase_interface->mcu_get_systick_count() - count) < 70)
    {
        // 让出cpu
        aht21_instance->pfyield(aht21_instance);
    }
    // AHT21初始化
    uint8_t readBuffer;
    // 获取状态
    iic_Start();                            // 生成iic启动信号
    iic_Send_Byte(AHT21_ADDR, 0x71);        // 发送0x71获取状态
    iic_wait_ack(AHT21_ADDR);               // 等待iic应答
    readBuffer = iic_Read_Byte(AHT21_ADDR); // 接收AHT21状态
    iic_Stop();                             // iic停止
    if ((readBuffer & 0x80) == 0x00)
    {
        iic_Start();                     // 生成iic启动信号
        iic_Send_Byte(AHT21_ADDR, 0xBE); // 发送0xBE进行初始化
        iic_nack(AHT21_ADDR);            // 发送非应答信号
        iic_Stop();                      // iic停止
    }
    return readBuffer;
}

/**
 * AHT21传感器逆初始化函数
 */
int8_t aht21_deInit(bsp_aht21_t *aht21_instance)
{
    // iic逆初始化
    aht21_instance->iic_driver_interface_t->pfDeInit();
    // AHT21下电
    __HAL_RCC_GPIOB_CLK_DISABLE();
    // aht21_instance 解构
    aht21_deInst(aht21_instance);
    // 逆初始化成功
    return RET_CODE_SUCCESS;
}

/**
 * @brief  ATH21 解构
 *
 * @param  *ath21_instance  aht21实例
 *
 * @return 0 success
 *
 */
int8_t aht21_deInst(bsp_aht21_t *aht21_instance)
{
    if (aht21_instance != NULL)
    {
        // AHT21实例IIC逆初始化
        if (aht21_instance->iic_driver_interface_t != NULL)
        {
            aht21_instance->iic_driver_interface_t->pfDeInit = NULL;
            aht21_instance->iic_driver_interface_t->pfReadReg = NULL;
            aht21_instance->iic_driver_interface_t->pfWriteReg = NULL;
            aht21_instance->iic_driver_interface_t->pfInit = NULL;
            aht21_instance->iic_driver_interface_t->pfStart = NULL;
            aht21_instance->iic_driver_interface_t->pfStop = NULL;
            aht21_instance->iic_driver_interface_t->pfWaitAck = NULL;
            aht21_instance->iic_driver_interface_t->pfSendByte = NULL;
            aht21_instance->iic_driver_interface_t->pfReadByte = NULL;
            aht21_instance->iic_driver_interface_t->pfSendAck = NULL;
            aht21_instance->iic_driver_interface_t->pfSendNack = NULL;
            aht21_instance->iic_driver_interface_t = NULL;
        }
        // AHT21实例时基单元逆初始化
        if (aht21_instance->pftimebase_interface != NULL)
        {
            aht21_instance->pftimebase_interface->mcu_get_systick_count = NULL;
        }
        // AHT21实例逆初始化
        aht21_instance->pfdeInit = NULL;
        aht21_instance->pfinit = NULL;
        aht21_instance->pfaht21_read_id = NULL;
        aht21_instance->pfaht21_read_data = NULL;
        aht21_instance->pfyield = NULL;
        aht21_instance->pfInst = NULL;
        aht21_instance = NULL;
    }
    return 0;
}

/**
 * @brief  读取AHT21 id
 * @param  aht21_instance *aht21_instance  aht2
 */
int8_t aht21_read_id(bsp_aht21_t *aht21_instance)
{
    return AHT21_ADDR;
}

/**
 * 读取温度/湿度
 */
int8_t aht21_read_data(bsp_aht21_t *aht21_instance, float *temp, float *humi)
{
    uint8_t readBuffer[6];
    //发送信号
    uint8_t send_arry[3] =  {0xAC,0x33,0x00};
    aht21_instance->iic_driver_interface_t->pfWriteReg(AHT21_ADDR,send_arry,3);
    // iic_Start(); // 生成iic启动信号
    // iic_Send_Byte(AHT21_ADDR, 0xAC);
    // iic_Send_Byte(AHT21_ADDR, 0x33);
    // iic_Send_Byte(AHT21_ADDR, 0x00);
    // 等待75ms时间测量
    int32_t count = aht21_instance->pftimebase_interface->mcu_get_systick_count();
    if ((aht21_instance->pftimebase_interface->mcu_get_systick_count()) - count < 75)
    {
        // 让出cpu
        aht21_instance->pfyield(aht21_instance);
    }
    iic_wait_ack(AHT21_ADDR); // 等待iic应答
    for (int i = 0; i < 6; i++)
    {
        readBuffer[i] = iic_Read_Byte(AHT21_ADDR); // 接收AHT21测量值
    }
    iic_Stop(); // 发送iic停止信号
    // 判断0字节第七位是否为0 若为0则为刚刚测量完成的数据
    if ((readBuffer[0] & 0x80) == 0x00)
    {
        uint32_t data = 0;
        data = ((uint32_t)readBuffer[3] >> 4) + ((uint32_t)readBuffer[2] << 4) + ((uint32_t)readBuffer[1] << 12);
        *temp = data * 200.0f / (1 << 20) - 50;
        *humi = data * 100.0f / (1 << 20);
        return RET_CODE_SUCCESS; // 测量成功
    }
    return -1; // 待修改
}

int8_t iic_ReadReg(void *,uint8_t addr,uint8_t *data,uint8_t size)
{
    iic_driver_interface_t
}
