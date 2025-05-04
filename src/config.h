#pragma once

// Pin definitions
#define TOUCH_CS  33  // Touch chip select pin - IO33
#define TOUCH_IRQ 36  // Touch interrupt pin - IO36
#define TFT_BL    27  // TFT backlight pin

// SPI pins for touch
#define TOUCH_SPI_SCK  14
#define TOUCH_SPI_MISO 12
#define TOUCH_SPI_MOSI 13

// Display dimensions
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320

// Touch calibration values
#define X_MIN 350
#define X_MAX 3950
#define Y_MIN 350
#define Y_MAX 3850

// RGB LED pins
#define LED_PIN_RED 22    // Red LED (common anode, low level on)
#define LED_PIN_GREEN 16  // Green LED (common anode, low level on)
#define LED_PIN_BLUE 17   // Blue LED (common anode, low level on)

// LVGL settings
#define LVGL_BUFFER_ROWS 20       // Number of rows in the buffer
#define LVGL_REFRESH_TIME 5       // LVGL refresh time in ms

#define SCREEN_TIMEOUT_MS 30000 // 30 seconds timeout
static uint32_t last_activity_time = 0;
static bool screen_on = true;
