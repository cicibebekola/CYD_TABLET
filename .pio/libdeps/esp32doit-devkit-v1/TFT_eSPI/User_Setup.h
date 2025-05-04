#define USER_SETUP_INFO "User_Setup"

// ST7789 driver
#define ST7789_DRIVER
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// Define the pins according to your specifications
#define TFT_CS   15   // LCD screen selection control signal (IO15), low level effective
#define TFT_DC   2    // LCD command/data selection control signal (IO2)
                      // High Level: data, low Level: command
#define TFT_RST  -1   // EN is shared with ESP32 reset (low level reset), so we don't need to control it

#define TFT_MOSI 13   // SPI bus write data signal (IO13)
#define TFT_SCLK 14   // SPI bus clock signal (IO14)
#define TFT_MISO 12   // SPI bus read data signal (IO12), not used by ST7789

#define TFT_BL   27   // LCD backlight control signal (IO27), high level backlight on

// SPI configuration
#define SPI_FREQUENCY  27000000
#define SPI_READ_FREQUENCY  20000000

// Optional settings
//#define LOAD_GLCD
//#define LOAD_FONT2
//#define LOAD_FONT4
//#define LOAD_FONT6
//#define LOAD_FONT7
//#define LOAD_FONT8
//#define LOAD_GFXFF
//#define SMOOTH_FONT

// Touch screen configuration (if using)
//#define TOUCH_CS 33   // Resistive touch screen chip selection (IO33), low level effective
// Note: Touch shares SPI pins with LCD (IO12, IO13, IO14)