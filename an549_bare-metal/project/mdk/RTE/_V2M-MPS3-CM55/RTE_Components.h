
/*
 * Auto generated Run-Time-Environment Configuration File
 *      *** Do not modify ! ***
 *
 * Project: 'template' 
 * Target:  'V2M-MPS3-CM55' 
 */

#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H


/*
 * Define the Device Header File: 
 */
#define CMSIS_device_header "ARMCM55.h"

/* Arm::Acceleration:Arm-2D:Alpha-Blending:1.0.0-preview */
#define __RTE_ACCELERATION_ARM_2D_ALPHA_BLENDING__
/* Arm::Acceleration:Arm-2D:Core:1.0.0-preview */
#define __RTE_ACCELERATION_ARM_2D__
/* Arm::Acceleration:Arm-2D:Transform:1.0.0-preview */
#define __RTE_ACCELERATION_ARM_2D_TRANSFORM__
/* GorgonMeducer.Performance Counter::Utilities:perf_counter:Core:Library:1.9.4 */
#define __PERF_COUNTER__
/* Keil.ARM Compiler::Compiler:Event Recorder:DAP:1.5.1 */
#define RTE_Compiler_EventRecorder
          #define RTE_Compiler_EventRecorder_DAP
/* Keil.ARM Compiler::Compiler:I/O:STDOUT:EVR:1.2.0 */
#define RTE_Compiler_IO_STDOUT          /* Compiler I/O: STDOUT */
          #define RTE_Compiler_IO_STDOUT_EVR      /* Compiler I/O: STDOUT EVR */
/* LVGL.LVGL::LVGL:lvgl:Benchmark:8.3.0-dev */
/*! \brief enable demo:bencharmk */
#define LV_USE_DEMO_BENCHMARK         1
/* LVGL.LVGL::LVGL:lvgl:Essential:8.3.0-dev */
/*! \brief Enable LVGL */
#define RTE_GRAPHICS_LVGL
/* LVGL.LVGL::LVGL:lvgl:Extra Themes:8.3.0-dev */
/*! \brief use extra themes, widgets and layouts */
#define RTE_GRAPHICS_LVGL_USE_EXTRA_THEMES
/* LVGL.LVGL::LVGL:lvgl:GPU Arm-2D:1.0.3 */
/*! \brief enable Arm-2D support*/
#define LV_USE_GPU_ARM2D 1


#endif /* RTE_COMPONENTS_H */
