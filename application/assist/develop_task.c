// 开发新功能时可以使用本任务进行功能测试

#include "develop_task.h"

#include "FreeRTOS.h"
#include "ps2.h"
#include "ps2_typedef.h"
#include "remote_control.h"
#include "signal_generator.h"
#include "stm32f4xx_hal.h"
#include "task.h"
#include "tim.h"
#include "usb_debug.h"
#include "chassis_balance.h"
#include "IMU.h"
#include "CAN_communication.h"

const Sbus_t * SBUS;
const RC_ctrl_t * RC_CTRL;

extern Ps2_s ps2;
extern Chassis_s CHASSIS;

// Ps2Button_t ps2_btn_select = {.last = false, .now = false};
// Ps2Buttons_t ps2_btns = {0};

void develop_task(void const * pvParameters)
{
    // 空闲一段时间
    vTaskDelay(500);

    SBUS = get_sbus_point();
    RC_CTRL = get_remote_control_point();
    
    SupCap_s p_sup_cap;
    SupCapInit(&p_sup_cap,1);

    while (1) {
        GetSupCapMeasure(&p_sup_cap);

        ModifyDebugDataPackage(0,CHASSIS.fdb.leg[0].joint.Phi1, "L_phi1");
        ModifyDebugDataPackage(1,CHASSIS.fdb.leg[0].joint.Phi4, "L_phi4");
        ModifyDebugDataPackage(2,CHASSIS.fdb.leg[0].rod.L0, "L_L0");
        ModifyDebugDataPackage(3,CHASSIS.fdb.leg[0].rod.Theta, "L_Theta");
        ModifyDebugDataPackage(4,CHASSIS.fdb.leg[1].joint.Phi1, "R_phi1");
        ModifyDebugDataPackage(5,CHASSIS.fdb.leg[1].joint.Phi4, "R_phi4");
        ModifyDebugDataPackage(6,CHASSIS.fdb.leg[1].rod.L0, "R_L0");
        ModifyDebugDataPackage(7,CHASSIS.fdb.leg[1].rod.Theta, "R_Theta");
        ModifyDebugDataPackage(8,CHASSIS.joint_motor[0].fdb.pos, "p0");
        ModifyDebugDataPackage(9,CHASSIS.joint_motor[1].fdb.pos, "p1");

        vTaskDelay(1);
    }
}
