/**
  ****************************(C) COPYRIGHT 2025 Polarbear****************************
  * @file       remote_control.c/h
  * @brief      遥控器处理，遥控器是通过类似SBUS的协议传输，利用DMA传输方式节约CPU
  *             资源，利用串口空闲中断来拉起处理函数，同时提供一些掉线重启DMA，串口
  *             的方式保证热插拔的稳定性。
  * @note       该任务是通过串口中断启动，不是freeRTOS任务
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. done
  *  V1.0.0     Nov-11-2019     RM              1. support development board tpye c
  *  V2.0.0     Feb-17-2025     Penguin         1. support RC AT9S PRO
  *                                             2. support RC HT8A
  *                                             3. support normal sbus RC in struct Sbus_t
  *  V2.0.1     Feb-25-2025     Penguin         1. support RC ET08A
  *
  @verbatim
  ==============================================================================
  使用At9sPro遥控器时请设置5通为SwE，6通为SwG

  注：使用非DT7遥控器时，需要先检查通道值数据是否正常（一般遥控器都带有通道值数据偏移功能，将通道值中值移动到正确数值后再使用）
      AT9S PRO 遥控器中值为 1000
      HT8A 遥控器中值为 992
      ET08A 遥控器中值为 1024
  
  ET08A 遥控器设置指南：
    1. 设置 主菜单->系统设置->摇杆模式 为模式2
    2. 设置 主菜单->通用功能->通道设置 5通道为 [辅助1 SB --] 6通道为 [辅助2 SC --]
  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2025 Polarbear****************************
  */

#ifndef REMOTE_CONTROL_H
#define REMOTE_CONTROL_H
#include <stdbool.h>

#include "attribute_typedef.h"
#include "bsp_rc.h"
#include "macro_typedef.h"
#include "struct_typedef.h"

#define SBUS_RX_BUF_NUM 50u
#define SBUS_RC_FRAME_LENGTH 25u

// clang-format off
// AT9S PRO 遥控器通道值范围
#define AT9S_PRO_RC_CH_VALUE_MIN         ((uint16_t)200)
#define AT9S_PRO_RC_CH_VALUE_OFFSET      ((uint16_t)1000)
#define AT9S_PRO_RC_CH_VALUE_MAX         ((uint16_t)1800)

#define AT9S_PRO_RC_CONNECTED_FLAG       ((uint8_t)12)

// ET08A 遥控器通道值范围
#define ET08A_RC_CH_VALUE_MIN         ((uint16_t)353)
#define ET08A_RC_CH_VALUE_OFFSET      ((uint16_t)1024)
#define ET08A_RC_CH_VALUE_MAX         ((uint16_t)1694)

#define ET08A_RC_CONNECTED_FLAG       ((uint8_t)0)

// 自定义控制器连接符
#define ESP32_TRACKER_CONNECTED_FLAG       ((uint8_t)0x01)

/* ----------------------- Data Struct ------------------------------------- */


typedef struct
{
    uint16_t ch[16];
    uint8_t connect_flag;
} SBUS_t;

typedef enum {
    RC_TYPE_UNKNOW,
    RC_TYPE_AT9S_PRO,
    RC_TYPE_HT8A,
    RC_TYPE_ET08A,
    RC_TYPE_ESP32_TRACKER,
} RC_Type_e;

/* ----------------------- Internal Data ----------------------------------- */

extern void remote_control_init(void);
extern const SBUS_t * get_sbus_point(void);
extern void slove_RC_lost(void);
extern void slove_data_error(void);

/******************************************************************/
/* API                                                            */
/******************************************************************/

extern inline bool GetSbusOffline(void);
extern inline uint16_t GetSbusCh(uint8_t ch);
extern inline RC_Type_e GetRcType(void);

#endif
/*------------------------------ End of File ------------------------------*/
