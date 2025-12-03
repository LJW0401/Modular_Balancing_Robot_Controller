/**
  ****************************(C) COPYRIGHT 2024 Polarbear****************************
  * @file       gimbal_yaw_pitch.c/h
  * @brief      yaw_pitch云台控制器。
  * @note       包括初始化，目标量更新、状态量更新、控制量计算与直接控制量的发送
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     2024-04-01      Penguin         1. done
  *  V1.2.0     2025-02-26      Harry_Wong      1.删除了不需要的变量
  *                                             2.剩余更新详情请见.c文件
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2024 Polarbear****************************
*/

#include "robot_param.h"
#if (GIMBAL_TYPE == GIMBAL_YAW_PITCH_DIRECT)
#ifndef GIMBAL_YAW_PITCH_H
#define GIMBAL_YAW_PITCH_H
#include "CAN_cmd_dji.h"
#include "CAN_receive.h"
#include "IMU.h"  //陀螺仪文件
#include "cmsis_os.h"
#include "detect_task.h"
#include "gimbal.h"
#include "macro_typedef.h"
#include "math.h"
#include "motor.h"
#include "pid.h"
#include "remote_control.h"
#include "robot_param.h"
#include "struct_typedef.h"
#include "supervisory_computer_cmd.h"
#include "usb_debug.h"
#include "user_lib.h"

/**
 * @brief 云台模式
 */
typedef enum {
    GIMBAL_OFF = 0,  // 关闭，给所有电机发送关闭指令
    GIMBAL_SAFE,     // 云台安全，所有控制量置0
    GIMBAL_IMU,      // 云台陀螺仪控制(角度控制)
    GIMBAL_ECD,      // 云台电机角度控制
} GimbalMode_e;

typedef struct
{
    GimbalMode_e mode, last_mode;  // 模式

    /*-------------------- Motors --------------------*/
    Motor_s m_yaw, m_pit;
    /*-------------------- Values --------------------*/
    struct
    {
        struct
        {
            float pos;  // (rad) 云台yaw位置
            float vel;  // (rad/s) 云台yaw速度
        } rol, pit, yaw;
    } fdb;  // 状态量

    struct
    {
        struct
        {
            float pos;  // (rad) 云台yaw位置
            float vel;  // (rad/s) 云台yaw速度
        } rol, pit, yaw;
    } ref;  // 期望量

    struct
    {
        struct
        {
            float pos;    // (rad) 位置
            float vel;    // (rad/s) 速度
            float tor;    // (N*m) 力矩
            float cur;    // (A) 电流
            float value;  // 直接控制量，无单位
        } rol, pit, yaw;
    } cmd;  // 控制量

    struct
    {
        struct
        {
            struct
            {
                float pos
            } rol, pit, yaw;
        } upper, lower, middle;
    } limit;

    struct
    {
        struct
        {
            pid_type_def pos, vel;
        } rol, pit, yaw;
    } pid;
} Gimbal_s;

extern void GimbalInit(void);

extern void GimbalHandleException(void);

extern void GimbalObserver(void);

extern void GimbalReference(void);

extern void GimbalConsole(void);

extern void GimbalSendCmd(void);

#endif  // GIMBAL_YAW_PITCH_H
#endif  // GIMBAL_YAW_PITCH
