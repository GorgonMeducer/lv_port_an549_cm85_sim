/****************************************************************************
*  Copyright 2021 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/

/*============================ INCLUDES ======================================*/
#include "app_cfg.h"
#include "platform.h"
#include "cmsis_compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "RTE_Components.h"

#if defined(RTE_CMSIS_RTOS2)
#include "cmsis_os2.h"
#endif

#if defined(RTE_GRAPHICS_LVGL)
#   include "demos/lv_demos.h"
#   include "lv_port_disp_template.h"
#   include "lv_port_indev_template.h"
#endif

#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wcompound-token-split-by-macro"
#endif

/*============================ MACROS ========================================*/
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

// <o>Arm-2D APP Stack Size <1024-32767>
// <i> Specify the arm-2d application thread stack size
// <i> Default: 2048
#ifndef APP_STACK_SIZE
#   define APP_STACK_SIZE       (3072ul)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if defined(RTE_GRAPHICS_LVGL)
static void test(void)
{
    lv_obj_t * btn = lv_btn_create(lv_scr_act());                   /*Add a button to the current screen*/
    lv_obj_set_pos(btn, 10, 10);                                    /*Set its position*/
    lv_obj_set_size(btn, 100, 50);                                  /*Set its size*/
    //lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL); /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);                        /*Add a label to the button*/
    lv_label_set_text(label, "Button");                             /*Set the labels text*/
    lv_obj_center(label);                                           /*Align the label to the center*/
}


int main(void)
{
    printf("Hello LVGL!!\r\n");
    
    __cycleof__("Draw strings on LCD") {
        __LL_LCD_PRINT_BANNER("Hello LVGL!!");
    }

    lv_init();
    
    lv_port_disp_init();
    lv_port_indev_init();

#if LV_USE_DEMO_BENCHMARK

    __LL_LCD_PRINT(24, 0, "Running LVGL Benchmark...");
    __LL_LCD_PRINT(25, 0, "Please stand by...");
    __LL_LCD_PRINT(28, 0, "NOTE: You will NOT see anything until the end.");

    lv_demo_benchmark();
    
    //lv_demo_benchmark_run_scene(LV_DEMO_BENCHMARK_MODE_RENDER_AND_DRIVER, 26*2-1);      // run scene no 31
#elif LV_USE_DEMO_RENDER
    lv_demo_render(LV_DEMO_RENDER_SCENE_IMAGE_NORMAL, 128);
#elif LV_USE_DEMO_WIDGETS
    lv_demo_widgets();
#elif LV_USE_DEMO_MUSIC
    lv_demo_music();
#else
    test();
#endif
    
    while(1) {
        //lv_timer_handler_run_in_period(1);
        lv_timer_handler();
    }
    
}

#else

 __NO_RETURN
void app_2d_main_thread (void *argument) 
{
    
    init_task_cycle_counter();
    
    printf("Hello Template!!");

    __cycleof__("Draw strings on LCD") {
        __LL_LCD_PRINT_BANNER("Hello LVGL!!");
        printf("SystemCoreClock: %d", SystemCoreClock);
    }
    
    fflush(&__stdout);

    uint64_t dwInstructions = perfc_pmu_get_instruction_count();
    uint32_t dwInsCalib = perfc_pmu_get_instruction_count() - dwInstructions;
    int64_t lCycles = 0;
    
    dwInstructions = perfc_pmu_get_instruction_count();
    
    __cycleof__("Coremark", { lCycles = __cycle_count__; }) {
#ifdef __PERF_COUNTER_COREMARK__
    coremark_main();
#endif
    }
    dwInstructions = perfc_pmu_get_instruction_count() - dwInstructions - dwInsCalib;
    
    printf( "\r\n"
            "No. Instructions: %lld\r\n"
            "Cycle Used: %lld\r\n"
            "Cycles per Instructions: %3.3f \r\n", 

            dwInstructions,
            lCycles,
            (double)lCycles / (double)dwInstructions);
    

    while(1) {
        osDelay(1000ul);
//        printf("[%016llx]", get_system_ticks());
//        printf("%lld\r\n", get_system_ms());
    }

    //osThreadExit();
}

extern void console_init(void);

int main(void)
{

    /* Initialize CMSIS-RTOS2 */
    osKernelInitialize ();
    console_init();

    static uint64_t thread1_stk_1[APP_STACK_SIZE / sizeof(uint64_t)];
     
    const osThreadAttr_t thread1_attr = {
      .stack_mem  = &thread1_stk_1[0],
      .stack_size = sizeof(thread1_stk_1)
    };

    /* Create application main thread */
    osThreadNew(app_2d_main_thread, NULL, &thread1_attr);

    /* Start thread execution */
    osKernelStart();
    
    while(1) {
    }

}
#endif