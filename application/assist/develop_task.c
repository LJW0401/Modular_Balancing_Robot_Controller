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

const SBUS_t * SBUS;

extern Ps2_s ps2;

// Ps2Button_t ps2_btn_select = {.last = false, .now = false};
// Ps2Buttons_t ps2_btns = {0};

void develop_task(void const * pvParameters)
{
    // 空闲一段时间
    vTaskDelay(500);

    SBUS = get_sbus_point();
    
    SupCap_s p_sup_cap;
    SupCapInit(&p_sup_cap,1);

    while (1) {
        // ModifyDebugDataPackage(0,, "");
        // ModifyDebugDataPackage(1,, "");
        // ModifyDebugDataPackage(2,, "");
        // ModifyDebugDataPackage(3,, "");
        // ModifyDebugDataPackage(4,, "");
        // ModifyDebugDataPackage(5,, "");
        // ModifyDebugDataPackage(6,, "");
        // ModifyDebugDataPackage(7,, "");
        // ModifyDebugDataPackage(8,, "");
        // ModifyDebugDataPackage(9,, "");

        vTaskDelay(1);
    }
}
