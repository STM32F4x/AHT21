#ifndef ERROR_CODES_H
#define ERROR_CODES_H

// 定义全局异常常量
typedef enum
{
    RET_CODE_SUCCESS = 0,                           // 成功
    RET_CODE_ERROR_AHT21_INSTANCE_NULL = -1,        // aht21_instance null
    RET_CODE_ERROR_IIC_INSTANCE_NULL = -2,          // iic_instance null
    RET_CODE_ERROR_TIMEBASE_NULL = -3,              // timebase null
    RET_CODE_ERROR_RTOS_YEILD_NULL = -4,            // rtos_yeild null
    RET_CODE_ERROR_TEPM_HUMI_MODLE_ADDR_ERROT = -5, //  TEPM_HUMI_MODLE_ADDR_ERROT
    RET_CODE_HAS_BEEN_INSTED = -6,                  // has been insted
    RET_CODE_HAS_BEEN_INITED = -7,                  // has been inited
    RET_CODE_ERROR_PARAM_NULL = -8,                 // 参数空指针
    RET_CODE_QUEUE_EVENT_NULL = -9,                      // queue event null
    RET_CODE_XSEMAPHORETAKE_FAIL = -10,                // xSemaphoreTake fail
    RET_CODE_XTASKCREATE_FAIL = -11,                // 参数空指针
    RET_CODE_ERROR_PARAM_NULL = -12,                // 参数空指针
    RET_CODE_ERROR_PARAM_NULL = -13,                // 参数空指针
    RET_CODE_ERROR_PARAM_NULL = -14,                // 参数空指针
    RET_CODE_ERROR_PARAM_NULL = -15,                // 参数空指针

} ret_code_t;

#endif // ERROR_CODES_H
