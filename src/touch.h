#pragma once

#include <XPT2046_Touchscreen.h>
#include <EEPROM.h>
#include <lvgl.h>
#include "config.h"

// Touch initialization and management functions
void touch_init();
void touch_calibrate();

// LVGL touch read callback
void touch_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);

// Calibration data structure
struct CalibrationData {
  int xMin, xMax, yMin, yMax;
};

// Load/save calibration data
void touch_load_calibration();
void touch_save_calibration();

void check_screen_timeout();
void reset_screen_timeout();
void update_voltage_display();