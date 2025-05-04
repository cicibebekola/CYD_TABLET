#include <lvgl.h>
#include "display.h"

// TFT Display object
static TFT_eSPI tft = TFT_eSPI();

void display_init() {
  // Initialize display
  tft.init();
  tft.setRotation(0);  // Match your physical screen orientation
  tft.fillScreen(TFT_BLACK);
  
}

void display_set_backlight(uint8_t brightness) {
  analogWrite(TFT_BL, brightness);        // Write brightness (0-255)
}

TFT_eSPI* display_get_tft() {
  return &tft;
}

void display_flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)color_p, w * h, true);
  tft.endWrite();
  
  lv_disp_flush_ready(disp);
}

