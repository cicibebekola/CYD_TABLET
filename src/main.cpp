#include <Arduino.h>
#include <SPI.h>
#include <lvgl.h>

// Include project headers
#include "config.h"
#include "display.h"
#include "touch.h"
#include "lvgl_init.h"

// Forward declarations
extern void ui_create();

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  Serial.println("ESP32 LVGL Project Starting...");
  
  // Initialize display
  display_init();
  display_set_backlight(180);  // Set backlight to maximum brightness
  Serial.println("Display initialized");
  
  // Initialize touch
  touch_init();
  Serial.println("Touch initialized");
  
  // Uncomment to run calibration
  // touch_calibrate();
  
  // Initialize LVGL
  lvgl_init_system();
  lvgl_init_display();
  lvgl_init_input();
  lvgl_init_timer();
  Serial.println("LVGL initialized");
  
  // Create UI
  ui_create();
  Serial.println("UI created");
  
  Serial.println("Setup complete");
}

void loop() {
  // Handle LVGL tasks
  lvgl_task_handler();
   // Check for screen timeout
  check_screen_timeout();

  // Small delay to prevent watchdog triggers
  delay(5);
}