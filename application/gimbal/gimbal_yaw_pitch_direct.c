/**
  ****************************(C) COPYRIGHT 2024 Polarbear****************************
  * @file       gimbal_yaw_pitch.c/h
  * @brief      yaw_pitch云台控制器。
  * @note       包括初始化，目标量更新、状态量更新、控制量计算与直接控制量的发送
  * @history
  *  Version    Date            Modification
  *   V1.0.0    2025-12-3       1. 完成云台所有基本控制
  @verbatim
  ==============================================================================
  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2024 Polarbear****************************
**/

#include "gimbal_yaw_pitch_direct.h"

#include "remote_control.h"
#include "signal_generator.h"
#include "string.h"

#if (GIMBAL_TYPE == GIMBAL_YAW_PITCH_DIRECT)

#define TUNE_MODE true

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MID(a, b, c) (MAX(MIN(MAX(a, b), c), MIN(a, b)))

#define PITCH_MAX (M_PI_2 * 0.9f)

Gimbal_s GIMBAL;

Motor_s * motor_array[2];

/*--------------------------------Internal functions---------------------------------------*/

/*-------------------- Init --------------------*/

/**
 * @brief          初始化
 * @param[in]      none
 * @retval         none
 */
void GimbalInit(void)
{
    GIMBAL.mode = GIMBAL_SAFE;
    GIMBAL.last_mode = GIMBAL_SAFE;

    // 电机初始化
    MotorInit(
        &GIMBAL.m_pit, MOTOR_GIMBAL_PIT_ID, MOTOR_GIMBAL_PIT_CAN, MOTOR_GIMBAL_PIT_TYPE,
        MOTOR_GIMBAL_PIT_DIRECTION, MOTOR_GIMBAL_PIT_REDUCTION_RATIO, MOTOR_GIMBAL_PIT_MODE);
    MotorInit(
        &GIMBAL.m_yaw, MOTOR_GIMBAL_YAW_ID, MOTOR_GIMBAL_YAW_CAN, MOTOR_GIMBAL_YAW_TYPE,
        MOTOR_GIMBAL_YAW_DIRECTION, MOTOR_GIMBAL_YAW_REDUCTION_RATIO, MOTOR_GIMBAL_YAW_MODE);

    motor_array[0] = &GIMBAL.m_pit;
    motor_array[1] = &GIMBAL.m_yaw;

    // 状态量初始化
    memset(&GIMBAL.fdb, 0, sizeof(GIMBAL.fdb));
    // 目标量初始化
    memset(&GIMBAL.ref, 0, sizeof(GIMBAL.ref));
    // 控制量初始化
    memset(&GIMBAL.cmd, 0, sizeof(GIMBAL.cmd));

    //限制值初始化
    GIMBAL.limit.middle.motor.pit = GIMBAL_MIDDLE_LIMIT_PIT_POS;
    if (GIMBAL_UPPER_LIMIT_PIT_POS > GIMBAL_MIDDLE_LIMIT_PIT_POS) {
        GIMBAL.limit.upper.motor.pit = GIMBAL_UPPER_LIMIT_PIT_POS;
    } else {
        GIMBAL.limit.upper.motor.pit = GIMBAL_UPPER_LIMIT_PIT_POS + 2.0f * M_PI;
    }
    if (GIMBAL_LOWER_LIMIT_PIT_POS < GIMBAL_MIDDLE_LIMIT_PIT_POS) {
        GIMBAL.limit.lower.motor.pit = GIMBAL_LOWER_LIMIT_PIT_POS;
    } else {
        GIMBAL.limit.lower.motor.pit = GIMBAL_LOWER_LIMIT_PIT_POS - 2.0f * M_PI;
    }

    // PID参数初始化
    float pit_pos_pid[3] = {GIMBAL_PID_PIT_POS_KP, GIMBAL_PID_PIT_POS_KI, GIMBAL_PID_PIT_POS_KD};
    PID_init(
        &GIMBAL.pid.pit.pos, PID_POSITION, pit_pos_pid, GIMBAL_PID_PIT_POS_MAX_OUT,
        GIMBAL_PID_PIT_POS_MAX_IOUT);
    float pit_vel_pid[3] = {GIMBAL_PID_PIT_VEL_KP, GIMBAL_PID_PIT_VEL_KI, GIMBAL_PID_PIT_VEL_KD};
    PID_init(
        &GIMBAL.pid.pit.vel, PID_POSITION, pit_vel_pid, GIMBAL_PID_PIT_VEL_MAX_OUT,
        GIMBAL_PID_PIT_VEL_MAX_IOUT);

    float yaw_pid[3] = {GIMBAL_PID_YAW_POS_KP, GIMBAL_PID_YAW_POS_KI, GIMBAL_PID_YAW_POS_KD};
    PID_init(
        &GIMBAL.pid.yaw.pos, PID_POSITION, yaw_pid, GIMBAL_PID_YAW_POS_MAX_OUT,
        GIMBAL_PID_YAW_POS_MAX_IOUT);
    float yaw_vel_pid[3] = {GIMBAL_PID_YAW_VEL_KP, GIMBAL_PID_YAW_VEL_KI, GIMBAL_PID_YAW_VEL_KD};
    PID_init(
        &GIMBAL.pid.yaw.vel, PID_POSITION, yaw_vel_pid, GIMBAL_PID_YAW_VEL_MAX_OUT,
        GIMBAL_PID_YAW_VEL_MAX_IOUT);
}
/*-------------------- Set mode --------------------*/

/**
 * @brief          设置模式
 * @param[in]      none
 * @retval         none
 */
void GimbalSetMode(void)
{
    GIMBAL.last_mode = GIMBAL.mode;
    if (GetSbusOffline()) {
        GIMBAL.mode = GIMBAL_SAFE;
        return;
    }

    if (GetRcType() == RC_TYPE_ET08A) {
        if (GetSbusCh(4) == 0x0400) {
            GIMBAL.mode = GIMBAL_IMU;
        } else {
            GIMBAL.mode = GIMBAL_SAFE;
        }
    }
}
/*-------------------- Observe --------------------*/

/**
 * @brief          更新状态量
 * @param[in]      none
 * @retval         none
 */
void GimbalObserver(void)
{
    // 电机位置更新
    GetMotorMeasure(&GIMBAL.m_pit);
    GetMotorMeasure(&GIMBAL.m_yaw);

    GIMBAL.fdb.pit.m_pos = GIMBAL.m_pit.fdb.pos * GIMBAL.m_pit.direction;
    GIMBAL.fdb.yaw.m_pos = GIMBAL.m_yaw.fdb.pos * GIMBAL.m_pit.direction;

    // IMU角度与速度更新
    GIMBAL.fdb.pit.pos = GetImuAngle(AX_PITCH);
    GIMBAL.fdb.pit.vel = GetImuVelocity(AX_PITCH);

    GIMBAL.fdb.yaw.pos = GetImuAngle(AX_YAW);
    GIMBAL.fdb.yaw.vel = GetImuVelocity(AX_YAW);

    // IMU限制范围更新
    // pitch轴
    float upper_delta = GIMBAL.limit.upper.motor.pit - GIMBAL.fdb.pit.m_pos;
    float lower_delta = GIMBAL.limit.lower.motor.pit - GIMBAL.fdb.pit.m_pos;
    GIMBAL.limit.upper.imu.pit = upper_delta + GIMBAL.fdb.pit.pos;
    GIMBAL.limit.lower.imu.pit = lower_delta + GIMBAL.fdb.pit.pos;

    GIMBAL.limit.upper.imu.pit = MIN(GIMBAL.limit.upper.imu.pit, PITCH_MAX);
    GIMBAL.limit.lower.imu.pit = MAX(GIMBAL.limit.lower.imu.pit, -PITCH_MAX);
}

/*-------------------- Reference --------------------*/

/**
 * @brief          更新目标量
 * @param[in]      none
 * @retval         none
 */
void GimbalReference(void)
{
    float pit_pos, yaw_pos;
#if TUNE_MODE
    pit_pos = GenerateSinWave(0.3, 0, 5);
    yaw_pos = GenerateSinWave(0.3, 0, 5);
#else
    if (GetRcType() == RC_TYPE_ET08A) {
        pit_pos = (GetSbusCh(1) - ET08A_RC_CH_VALUE_OFFSET) /
                  (ET08A_RC_CH_VALUE_MAX - ET08A_RC_CH_VALUE_MIN) * 2;
        yaw_pos = (GetSbusCh(0) - ET08A_RC_CH_VALUE_OFFSET) /
                  (ET08A_RC_CH_VALUE_MAX - ET08A_RC_CH_VALUE_MIN) * 2;
    } else if (GetRcType() == RC_TYPE_ESP32_TRACKER) {
        pit_pos = uint_to_float(GetSbusCh(1), -M_PI_2, M_PI_2, 11);
        yaw_pos = uint_to_float(GetSbusCh(2), -M_PI_2, M_PI_2, 11);
    }
#endif
    GIMBAL.ref.pit.pos =
        MID(theta_format(pit_pos), GIMBAL.limit.lower.imu.pit, GIMBAL.limit.upper.imu.pit);
    GIMBAL.ref.yaw.pos = theta_format(yaw_pos);
}

/*-------------------- Console --------------------*/

/**
 * @brief          计算控制量
 * @param[in]      none
 * @retval         none
 */
void GimbalConsole(void)
{
    if (GIMBAL.mode == GIMBAL_SAFE || GIMBAL.mode == GIMBAL_OFF) {
        GIMBAL.cmd.pit.value = 0;
        GIMBAL.cmd.yaw.value = 0;
    } else if (GIMBAL.mode == GIMBAL_IMU || GIMBAL.mode == GIMBAL_ECD) {
        // roll pid控制
        // pitch pid控制
        GIMBAL.ref.pit.vel = PID_calc(&GIMBAL.pid.pit.pos, GIMBAL.fdb.pit.pos, GIMBAL.ref.pit.pos);
        GIMBAL.cmd.pit.value =
            PID_calc(&GIMBAL.pid.pit.vel, GIMBAL.fdb.pit.vel, GIMBAL.ref.pit.vel);
        // yaw pid控制
        GIMBAL.ref.yaw.vel = PID_calc(&GIMBAL.pid.yaw.pos, GIMBAL.fdb.yaw.pos, GIMBAL.ref.yaw.pos);
        GIMBAL.cmd.yaw.value =
            PID_calc(&GIMBAL.pid.yaw.vel, GIMBAL.fdb.yaw.vel, GIMBAL.ref.yaw.vel);
    }
}

/*-------------------- Cmd --------------------*/

/**
 * @brief          发送控制量
 * @param[in]      none
 * @retval         none
 */
void GimbalSendCmd(void)
{
    // CanCmdDjiMotor(1,0x1FF,gimbal_direct.yaw.set.curr,gimbal_direct.pitch.set.curr,0,0);
    // CanCmdDjiMotor(1, 0x1FF, GIMBAL.cmd.yaw.value, GIMBAL.cmd.pit.value, 0, 0);
    // DjiMultipleControl(1, 2, motor_array);
}

#endif  // GIMBAL_YAW_PITCH
