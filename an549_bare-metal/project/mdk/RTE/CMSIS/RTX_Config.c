/*
 * Copyright (c) 2013-2023 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * -----------------------------------------------------------------------------
 *
 * $Revision:   V5.2.0
 *
 * Project:     CMSIS-RTOS RTX
 * Title:       RTX Configuration
 *
 * -----------------------------------------------------------------------------
 */
 
#include "cmsis_compiler.h"
#include "rtx_os.h"


#if defined(RTE_Acceleration_Arm_2D_Helper_Disp_Adapter0)

#include <stdio.h>

#include "arm_2d_helper.h"
#include "arm_2d_disp_adapters.h"

#include "arm_2d_scene_console.h"

extern
int32_t GLCD_DrawBitmap(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t *bitmap);

void Disp0_DrawBitmap(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t *bitmap)
{
    GLCD_DrawBitmap(x, y, width, height, bitmap);
}



ARM_NOINIT
static user_scene_console_t DEFAULT_CONSOLE;

static 
uintptr_t s_hWaitConsole = 0;

static void __console_wait_for_flush_sync(void)
{
    arm_2d_port_wait_for_semaphore(s_hWaitConsole);
}

static void __console_signal_flush_sync(void)
{
    arm_2d_port_set_semaphore(s_hWaitConsole);
}

int $Sub$$stdout_putchar(int ch)
{
    
    while (!console_box_putchar(&DEFAULT_CONSOLE.tConsole, ch)) {
        __console_wait_for_flush_sync();
    }
    
    return ch;
}

int $Sub$$fflush(FILE *fn)
{
    extern int $Super$$fflush(FILE *fn);
    
    int result = $Super$$fflush(fn);
    if (fn == &__stdout) {
        __console_wait_for_flush_sync();
    }
    return result;
}

void console_init(void)
{
    static bool s_bInitalized = false;

    arm_irq_safe {
        if (s_bInitalized) {
            arm_exit_irq_safe;
        }
        s_bInitalized = true;
        
        /* RTX5 requires irq is enabled */
        __enable_irq();
        
        arm_2d_init();
        
        disp_adapter0_init();
        arm_2d_scene_console_init(&DISP0_ADAPTER, &DEFAULT_CONSOLE);
        
        
        s_hWaitConsole = arm_2d_port_new_semaphore();
    }
}

#else
void console_init(void)
{}
#endif



 
// OS Idle Thread
__WEAK __NO_RETURN void osRtxIdleThread (void *argument) {
  (void)argument;

#if defined(RTE_Acceleration_Arm_2D_Helper_Disp_Adapter0)

    console_init();

    
    while(1) {
 
        if (arm_fsm_rt_cpl == disp_adapter0_task()) {
            __console_signal_flush_sync();
        }
        
    }

#else
  for (;;) {}
#endif
}
 
// OS Error Callback function
__WEAK uint32_t osRtxErrorNotify (uint32_t code, void *object_id) {
  (void)object_id;

  switch (code) {
    case osRtxErrorStackOverflow:
      // Stack overflow detected for thread (thread_id=object_id)
      break;
    case osRtxErrorISRQueueOverflow:
      // ISR Queue overflow detected when inserting object (object_id)
      break;
    case osRtxErrorTimerQueueOverflow:
      // User Timer Callback Queue overflow detected for timer (timer_id=object_id)
      break;
    case osRtxErrorClibSpace:
      // Standard C/C++ library libspace not available: increase OS_THREAD_LIBSPACE_NUM
      break;
    case osRtxErrorClibMutex:
      // Standard C/C++ library mutex initialization failed
      break;
    case osRtxErrorSVC:
      // Invalid SVC function called (function=object_id)
      break;
    default:
      // Reserved
      break;
  }
  for (;;) {}
//return 0U;
}
