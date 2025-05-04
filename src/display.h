#pragma once

#include <TFT_eSPI.h>
#include "config.h"

// Display initialization and management functions
void display_init();
void display_set_backlight(uint8_t brightness);

// Get display object
TFT_eSPI* display_get_tft();

// LVGL display flush callback
void display_flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

