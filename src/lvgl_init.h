#pragma once

#include <lvgl.h>
#include "config.h"

// LVGL initialization functions
void lvgl_init_system();
void lvgl_init_display();
void lvgl_init_input();
void lvgl_init_timer();

// LVGL timer handler
void lvgl_task_handler();

// LVGL timer ISR
void IRAM_ATTR lvgl_timer_isr();
