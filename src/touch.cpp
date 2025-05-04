#include "touch.h"
#include "display.h"

// Touch controller object
static XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

// Calibration data
static CalibrationData calData;

void touch_init() {
  // Initialize SPI for touch
  SPI.begin(TOUCH_SPI_SCK, TOUCH_SPI_MISO, TOUCH_SPI_MOSI, TOUCH_CS);
  
  // Initialize touch controller
  ts.begin();
  ts.setRotation(0);  // Must match display rotation!
  
  // Initialize EEPROM and load calibration
  EEPROM.begin(sizeof(CalibrationData));
  touch_load_calibration();
}

void touch_calibrate() {
  TFT_eSPI* tft = display_get_tft();
  
  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_WHITE);
  tft->drawString("Touch the RED dots", 10, 10);
  
  // Top-left calibration
  tft->fillCircle(20, 20, 10, TFT_RED);
  while (!ts.touched());
  delay(50); // Debounce
  TS_Point p = ts.getPoint();
  calData.xMin = p.x;
  calData.yMin = p.y;
  
  delay(500);
  while (ts.touched()); // Wait for release
  delay(500);
  
  // Bottom-right calibration
  tft->fillCircle(tft->width()-20, tft->height()-20, 10, TFT_RED);
  while (!ts.touched());
  delay(50); // Debounce
  p = ts.getPoint();
  calData.xMax = p.x;
  calData.yMax = p.y;
  
  delay(500);
  while (ts.touched()); // Wait for release
  
  touch_save_calibration();
  
  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_GREEN);
  tft->drawString("Calibration Complete!", 10, 10);
  delay(1000);
}

void touch_load_calibration() {
  EEPROM.get(0, calData);
  if (calData.xMin == -1 || calData.xMin == 0xFFFF) { // Default values if EEPROM is empty
    calData = {X_MIN, X_MAX, Y_MIN, Y_MAX};
  }
}

void touch_save_calibration() {
  EEPROM.put(0, calData);
  EEPROM.commit();
}

void sleep_screen() {
  display_set_backlight(0);  // Turn off backlight
  screen_on = false;
  Serial.println("Screen sleeping");
}

void wake_screen() {
  display_set_backlight(180);  // Restore backlight
  screen_on = true;
  reset_screen_timeout();  // Reset the timeout
  Serial.println("Screen awake");
}

void reset_screen_timeout() {
  last_activity_time = millis();
  if (!screen_on) {
      wake_screen();
  }
}

void check_screen_timeout() {
  if (screen_on && (millis() - last_activity_time > SCREEN_TIMEOUT_MS)) {
      sleep_screen();
  }
}

void touch_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  static TS_Point last_point;
  if (ts.tirqTouched()) {
    
    if (!screen_on) {
      wake_screen();
      delay(100);  // Allow some time for the screen to wake up
      return;  // Skip further touch processing while waking up
  }

    if (ts.touched()) {
      TS_Point p = ts.getPoint();
      // Only invert X-axis (swap min/max), keep Y-axis normal
      last_point.x = map(p.x, calData.xMin, calData.xMax, 0, SCREEN_WIDTH);   // X inverted
      last_point.y = map(p.y, calData.yMax, calData.yMin, 0, SCREEN_HEIGHT);  // Y normal
      
      // Apply constraints
      last_point.x = constrain(last_point.x, 0, SCREEN_WIDTH);
      last_point.y = constrain(last_point.y, 0, SCREEN_HEIGHT);
      
      data->state = LV_INDEV_STATE_PR;
      Serial.printf("Raw: X=%d, Y=%d | Mapped: X=%d, Y=%d\n", p.x, p.y, last_point.x, last_point.y);
    } else {
      data->state = LV_INDEV_STATE_REL;
    }
  } else {
    data->state = LV_INDEV_STATE_REL;
  }

  // Pass the coordinates to the data structure
  data->point.x = last_point.x;
  data->point.y = last_point.y;
}


