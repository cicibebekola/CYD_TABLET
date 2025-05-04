#include "lvgl_init.h"
#include "display.h"
#include "touch.h"

// LVGL display driver
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;

// LVGL display buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCREEN_WIDTH * LVGL_BUFFER_ROWS];

// LVGL timer for ticks
static hw_timer_t * lvglTimer = NULL;

void lvgl_init_system() {
  // Initialize LVGL
  lv_init();
  
  // Initialize display buffer
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * LVGL_BUFFER_ROWS);
}

void lvgl_init_display() {
  // Configure display driver
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = SCREEN_WIDTH;
  disp_drv.ver_res = SCREEN_HEIGHT;
  disp_drv.flush_cb = display_flush_cb;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);
}

void lvgl_init_input() {
  // Configure input device driver
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touch_read_cb;
  lv_indev_drv_register(&indev_drv);
}

void lvgl_init_timer() {
  // Set up timer interrupt for LVGL ticks
  lvglTimer = timerBegin(0, 80, true); // Timer 0, 80 divider for 1MHz, count up
  timerAttachInterrupt(lvglTimer, &lvgl_timer_isr, true);
  timerAlarmWrite(lvglTimer, LVGL_REFRESH_TIME * 1000, true); // Convert ms to µs
  timerAlarmEnable(lvglTimer);
}

void lvgl_task_handler() {
  lv_timer_handler(); // Handle LVGL tasks
}

void IRAM_ATTR lvgl_timer_isr() {
  lv_tick_inc(LVGL_REFRESH_TIME);
}
