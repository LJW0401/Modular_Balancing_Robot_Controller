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
  *                                             3. support normal sbus RC in struct SBUS_t
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
    2. 设置 主菜单->通用功能->通道设置 5通道为 [辅助1 SB --] 6通道为 [辅助2 SC --] 8通道为 [辅助3 LD --]
  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2025 Polarbear****************************
  */

#include "remote_control.h"

#include "bsp_usart.h"
#include "communication.h"
#include "detect_task.h"
#include "main.h"
#include "robot_param.h"
#include "string.h"

// 遥控器掉线时间阈值
#define RC_LOST_TIME 100  // ms
// 非dt7遥控器连续断线上线次数（超过认为断连）
#define SBUS_MAX_LOST_NUN 10

//遥控器出错数据上限
#define RC_CHANNAL_ERROR_VALUE 700

extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;

// 处理遥控器数据
static void SolveSbusData(volatile const uint8_t * sbus_buf, SBUS_t * sbus);

// SBUS数据
static SBUS_t SBUS = {.connect_flag = 0xFF};

//接收原始数据，为25个字节，给了50个字节长度，防止DMA传输越界
static uint8_t sbus_rx_buf[2][SBUS_RX_BUF_NUM];

// 上一次接收数据的时间
static uint32_t last_receive_time = 0;

// 遥控器类型
static RC_Type_e rc_type;

/**
  * @brief          遥控器初始化
  * @param[in]      none
  * @retval         none
  */
void remote_control_init(void) { RC_Init(sbus_rx_buf[0], sbus_rx_buf[1], SBUS_RX_BUF_NUM); }

/**
  * @brief          获取SBUS数据指针
  * @param[in]      none
  * @retval         SBUS数据指针
  */
const SBUS_t * get_sbus_point(void) { return &SBUS; }

void slove_RC_lost(void) { RC_restart(SBUS_RX_BUF_NUM); }
void slove_data_error(void) { RC_restart(SBUS_RX_BUF_NUM); }

//串口中断
void USART3_IRQHandler(void)
{
    if (huart3.Instance->SR & UART_FLAG_RXNE)  //接收到数据
    {
        __HAL_UART_CLEAR_PEFLAG(&huart3);
    } else if (USART3->SR & UART_FLAG_IDLE) {
        static uint16_t this_time_rx_len = 0;

        __HAL_UART_CLEAR_PEFLAG(&huart3);

        uint32_t now = HAL_GetTick();

        if ((hdma_usart3_rx.Instance->CR & DMA_SxCR_CT) == RESET) {
            /* Current memory buffer used is Memory 0 */

            //disable DMA
            //失效DMA
            __HAL_DMA_DISABLE(&hdma_usart3_rx);

            //get receive data length, length = set_data_length - remain_length
            //获取接收数据长度,长度 = 设定长度 - 剩余长度
            this_time_rx_len = SBUS_RX_BUF_NUM - hdma_usart3_rx.Instance->NDTR;

            //reset set_data_lenght
            //重新设定数据长度
            hdma_usart3_rx.Instance->NDTR = SBUS_RX_BUF_NUM;

            //set memory buffer 1
            //设定缓冲区1
            hdma_usart3_rx.Instance->CR |= DMA_SxCR_CT;

            //enable DMA
            //使能DMA
            __HAL_DMA_ENABLE(&hdma_usart3_rx);

            if (this_time_rx_len == SBUS_RC_FRAME_LENGTH) {
                SolveSbusData(sbus_rx_buf[1], &SBUS);
            }
        } else {
            /* Current memory buffer used is Memory 1 */
            //disable DMA
            //失效DMA
            __HAL_DMA_DISABLE(&hdma_usart3_rx);

            //get receive data length, length = set_data_length - remain_length
            //获取接收数据长度,长度 = 设定长度 - 剩余长度
            this_time_rx_len = SBUS_RX_BUF_NUM - hdma_usart3_rx.Instance->NDTR;

            //reset set_data_lenght
            //重新设定数据长度
            hdma_usart3_rx.Instance->NDTR = SBUS_RX_BUF_NUM;

            //set memory buffer 0
            //设定缓冲区0
            DMA1_Stream1->CR &= ~(DMA_SxCR_CT);

            //enable DMA
            //使能DMA
            __HAL_DMA_ENABLE(&hdma_usart3_rx);

            if (this_time_rx_len == SBUS_RC_FRAME_LENGTH) {
                SolveSbusData(sbus_rx_buf[1], &SBUS);
            }
        }
    }
}

static void SolveSbusData(volatile const uint8_t * sbus_buf, SBUS_t * sbus)
{
    // clang-format off
    sbus->ch[0] =((sbus_buf[2]<<8)   + (sbus_buf[1]))     & 0x07ff;                    \
    sbus->ch[1] =((sbus_buf[3]<<5)   + (sbus_buf[2]>>3))  & 0x07ff;                    \
    sbus->ch[2] =((sbus_buf[5]<<10)  + (sbus_buf[4]<<2) + (sbus_buf[3]>>6)) & 0x07ff;  \
    sbus->ch[3] =((sbus_buf[6]<<7)   + (sbus_buf[5]>>1))  & 0x07ff;                    \
    sbus->ch[4] =((sbus_buf[7]<<4)   + (sbus_buf[6]>>4))  & 0x07ff;                    \
    sbus->ch[5] =((sbus_buf[9]<<9)   + (sbus_buf[8]<<1) + (sbus_buf[7]>>7)) & 0x07ff;  \
    sbus->ch[6] =((sbus_buf[10]<<6)  + (sbus_buf[9]>>2))  & 0x07ff;                    \
    sbus->ch[7] =((sbus_buf[11]<<3)  + (sbus_buf[10]>>5)) & 0x07ff;                    \
    sbus->ch[8] =((sbus_buf[13]<<8)  + (sbus_buf[12]))    & 0x07ff;                    \
    sbus->ch[9] =((sbus_buf[14]<<5)  + (sbus_buf[13]>>3)) & 0x07ff;                    \
    sbus->ch[10]=((sbus_buf[16]<<10) + (sbus_buf[15]<<2) + (sbus_buf[14]>>6)) & 0x07ff;\
    sbus->ch[11]=((sbus_buf[17]<<7)  + (sbus_buf[16]>>1)) & 0x07ff;                    \
    sbus->ch[12]=((sbus_buf[18]<<4)  + (sbus_buf[17]>>4)) & 0x07ff;                    \
    sbus->ch[13]=((sbus_buf[20]<<9)  + (sbus_buf[19]<<1) + (sbus_buf[18]>>7)) & 0x07ff;\
    sbus->ch[14]=((sbus_buf[21]<<6)  + (sbus_buf[20]>>2)) & 0x07ff;                    \
    sbus->ch[15]=((sbus_buf[22]<<3)  + (sbus_buf[21]>>5)) & 0x07ff;                    \
    sbus->connect_flag = sbus_buf[23];
    // clang-format on

    // 记录数据接收时间
    last_receive_time = HAL_GetTick();
}

/******************************************************************/
/* API                                                            */
/*----------------------------------------------------------------*/
/* function:      GetRcOffline                                    */
/*                GetDt7RcCh                                      */
/*                GetDt7RcSw                                      */
/*                GetDt7MouseSpeed                                */
/*                GetDt7Mouse                                     */
/*                GetDt7Keyboard                                  */
/******************************************************************/

/**
  * @brief          获取遥控器是否离线。
  * @retval         true:离线，false:在线
  */
inline bool GetSbusOffline(void)
{
#if __CONTROL_LINK_RC == CL_RC_DIRECT
    return ((HAL_GetTick() - last_receive_time < RC_LOST_TIME) && (HAL_GetTick() > RC_LOST_TIME)) ||
           (!SBUS.connect_flag);
#elif __CONTROL_LINK_RC == CL_RC_UART2
    return GetUartRcOffline();
#else
    return true;
#endif
}

/**
  * @brief          获取SBUS通道值。
  * @param[in]      ch 通道id [0,15]
  * @retval         SBUS通道值，范围为 [0, 2048]
  */
inline uint16_t GetSbusCh(uint8_t ch) { return SBUS.ch[ch]; }

/**
  * @brief          获取遥控器型号。
  * @retval         遥控器型号
  */
inline RC_Type_e GetRcType(void) { return rc_type; }

/*------------------------------ End of File ------------------------------*/
