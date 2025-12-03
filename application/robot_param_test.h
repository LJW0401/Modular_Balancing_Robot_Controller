/**
  * @file       robot_param_balanced_infantry_gimbal.h
  * @brief      这里是平衡步兵机器人云台部分参数配置文件，包括物理参数、PID参数等
  */

#ifndef INCLUDED_ROBOT_PARAM_H
#define INCLUDED_ROBOT_PARAM_H
#include "robot_typedef.h"

#define GIMBAL_TYPE GIMBAL_YAW_PITCH_DIRECT  // 选择云台类型

// clang-format off
#define __SELF_BOARD_ID C_BOARD_DEFAULT  // 本板ID
#define __GYRO_BIAS_YAW  0.003096855f           // 陀螺仪零飘，单位rad/s

#define __CONTROL_LINK_RC  CL_RC_DIRECT   // 控制链路选择：RC遥控器
#define __CONTROL_LINK_KM  CL_KM_RC      // 控制链路选择：键鼠数据

/*******************************************************************************/
/* Gimbal                                                                      */
/*******************************************************************************/
// 云台ROL电机参数
// #define MOTOR_GIMBAL_ROL_ID               ((uint8_t)2)
// #define MOTOR_GIMBAL_ROL_CAN              ((uint8_t)1)
// #define MOTOR_GIMBAL_ROL_TYPE             ((MotorType_e)DJI_M6020)
// #define MOTOR_GIMBAL_ROL_DIRECTION        (1)
// #define MOTOR_GIMBAL_ROL_REDUCTION_RATIO  (1)
// #define MOTOR_GIMBAL_ROL_MODE             (DJI_VOLTAGE_MODE)

// 云台PIT电机参数
#define MOTOR_GIMBAL_PIT_ID               ((uint8_t)2)
#define MOTOR_GIMBAL_PIT_CAN              ((uint8_t)1)
#define MOTOR_GIMBAL_PIT_TYPE             ((MotorType_e)DJI_M6020)
#define MOTOR_GIMBAL_PIT_DIRECTION        (1)
#define MOTOR_GIMBAL_PIT_REDUCTION_RATIO  (1)
#define MOTOR_GIMBAL_PIT_MODE             (DJI_VOLTAGE_MODE)

// 云台YAW电机参数
#define MOTOR_GIMBAL_YAW_ID               ((uint8_t)1)
#define MOTOR_GIMBAL_YAW_CAN              ((uint8_t)1)
#define MOTOR_GIMBAL_YAW_TYPE             ((MotorType_e)DJI_M6020)
#define MOTOR_GIMBAL_YAW_DIRECTION        (1)
#define MOTOR_GIMBAL_YAW_REDUCTION_RATIO  (1)
#define MOTOR_GIMBAL_YAW_MODE             (DJI_VOLTAGE_MODE)

// 云台位置限制参数
#define GIMBAL_UPPER_LIMIT_PIT_POS    (-1.609145879f)    // 云台PIT上限位置(rad)
#define GIMBAL_MIDDLE_LIMIT_PIT_POS   (-2.063708757f)    // 云台PIT中间位置(rad)
#define GIMBAL_LOWER_LIMIT_PIT_POS    (-2.806417942f)   // 云台PIT下限位置(rad)

// 云台PID参数
// PIT位置环
#define GIMBAL_PID_PIT_POS_KP      50.0f
#define GIMBAL_PID_PIT_POS_KI      0.0f
#define GIMBAL_PID_PIT_POS_KD      0.0f
#define GIMBAL_PID_PIT_POS_MAX_IOUT  0.0f
#define GIMBAL_PID_PIT_POS_MAX_OUT   20.0f
// PIT速度环
#define GIMBAL_PID_PIT_VEL_KP      5000.0f
#define GIMBAL_PID_PIT_VEL_KI      5.0f
#define GIMBAL_PID_PIT_VEL_KD      0.0f
#define GIMBAL_PID_PIT_VEL_MAX_IOUT  1000.0f
#define GIMBAL_PID_PIT_VEL_MAX_OUT   25000.0f

// YAW位置环
#define GIMBAL_PID_YAW_POS_KP      30.0f
#define GIMBAL_PID_YAW_POS_KI      0.0f
#define GIMBAL_PID_YAW_POS_KD      0.0f
#define GIMBAL_PID_YAW_POS_MAX_IOUT  0.0f
#define GIMBAL_PID_YAW_POS_MAX_OUT   20.0f
// YAW速度环
#define GIMBAL_PID_YAW_VEL_KP      10000.0f
#define GIMBAL_PID_YAW_VEL_KI      100.0f
#define GIMBAL_PID_YAW_VEL_KD      0.0f
#define GIMBAL_PID_YAW_VEL_MAX_IOUT  5000.0f
#define GIMBAL_PID_YAW_VEL_MAX_OUT   25000.0f

// clang-format on
#endif /* INCLUDED_ROBOT_PARAM_H */
/*------------------------------ End of File ------------------------------*/
