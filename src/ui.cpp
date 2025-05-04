#include <lvgl.h>
#include "config.h"
#include <Arduino.h>
#include "display.h"
#include "touch.h"
#include "symbol.h"

// Spotify colors
#define SPOTIFY_BLACK lv_color_hex(0x121212)
#define SPOTIFY_GREEN lv_color_hex(0x1DB954)
#define SPOTIFY_WHITE lv_color_hex(0xFFFFFF)
#define SPOTIFY_LIGHT_GREY lv_color_hex(0xB3B3B3)
#define SPOTIFY_DARK_GREY lv_color_hex(0x282828)

// UI elements
static lv_obj_t* brightness_panel;
static lv_obj_t* led_slider;
static lv_obj_t* voltagedisplay;
static lv_obj_t* brightness_slider;
static lv_obj_t* led_icon;           // Flashlight icon
static lv_obj_t* brightness_icon;    // Brightness icon
static lv_obj_t* led_value_label;    // Label to show LED value
static lv_obj_t* brightness_value_label; // Label to show brightness value
static bool panel_visible = false;

// Store current values to restore after sleep
static uint8_t current_led_brightness = 0;
static uint8_t current_screen_brightness = 255;
static float current_voltage = 0.0;

// Timer for voltage updates
static lv_timer_t* voltage_update_timer = NULL;

// Forward declaration
void update_ui_values();

void set_led_brightness(uint8_t brightness) {
    current_led_brightness = brightness;
    uint8_t pwm_value = 255 - brightness;
    analogWrite(LED_PIN_RED, pwm_value);
    analogWrite(LED_PIN_GREEN, pwm_value);
    analogWrite(LED_PIN_BLUE, pwm_value);
    
    // Update the LED value label if it exists
    if (led_value_label) {
        lv_label_set_text_fmt(led_value_label, "%d%%", (brightness * 100) / 255);
    }
}

void update_voltage_display() {
    if (!voltagedisplay || !panel_visible || !screen_on) return;
    
    int raw = analogRead(34);
    current_voltage = (raw / 4095.0) * 3.3;
    
    // Force UI update with the new voltage value
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%.2fV", current_voltage);
    lv_label_set_text(voltagedisplay, buffer);
    
    // Request a redraw to ensure the change is visible
    lv_obj_invalidate(voltagedisplay);
}

void led_slider_event_cb(lv_event_t* e) {
    reset_screen_timeout(); // Reset timeout on interaction
    lv_obj_t* slider = lv_event_get_target(e);
    set_led_brightness(lv_slider_get_value(slider));
}

void brightness_slider_event_cb(lv_event_t* e) {
    reset_screen_timeout(); // Reset timeout on interaction
    lv_obj_t* slider = lv_event_get_target(e);
    current_screen_brightness = lv_slider_get_value(slider);
    display_set_backlight(current_screen_brightness);
    
    // Update the brightness value label
    if (brightness_value_label) {
        lv_label_set_text_fmt(brightness_value_label, "%d%%", (current_screen_brightness * 100) / 255);
    }
}


void create_led_icon(lv_obj_t* parent) {
    // Create image object instead of label
    led_icon = lv_img_create(parent);
    //lv_img_set_src(led_icon, &flashlight);  // Use your custom flashlight icon
    
    // Adjust the alignment coordinates to compensate for the icon size
    // Shift by half the width and height to center the icon where the label was
    lv_obj_align(led_icon, LV_ALIGN_TOP_LEFT, 10 - 16, 65 - 16);
    
    // Size is automatically set by the image descriptor (32x32)
   
    // Create label to show LED brightness value
    led_value_label = lv_label_create(parent);
    lv_label_set_text(led_value_label, "0%");
    lv_obj_set_style_text_color(led_value_label, SPOTIFY_WHITE, 0);
    lv_obj_set_style_text_font(led_value_label, &lv_font_montserrat_12, 0);
    lv_obj_align(led_value_label, LV_ALIGN_TOP_RIGHT, -1, 57);
}

// Screen brightness icon using your custom sun bitmap
void create_brightness_icon(lv_obj_t* parent) {
    // Create image object instead of label
    brightness_icon = lv_img_create(parent);
    lv_img_set_src(brightness_icon, &sun_map);  // Use your custom sun icon with rays
    
    // Adjust the alignment coordinates to compensate for the icon size
    // Shift by half the width and height to center the icon where the label was
    lv_obj_align(brightness_icon, LV_ALIGN_TOP_LEFT, 10 - 16, 25 - 16);
    
    // Size is automatically set by the image descriptor (32x32)
   
    // Create label to show brightness value
    brightness_value_label = lv_label_create(parent);
    lv_label_set_text(brightness_value_label, "100%");
    lv_obj_set_style_text_color(brightness_value_label, SPOTIFY_WHITE, 0);
    lv_obj_set_style_text_font(brightness_value_label, &lv_font_montserrat_12, 0);
    lv_obj_align(brightness_value_label, LV_ALIGN_TOP_RIGHT, -1, 17);
}


void create_led_slider(lv_obj_t* parent) {
    led_slider = lv_slider_create(parent);
    lv_obj_set_size(led_slider, LV_PCT(67), 10);
    lv_obj_align(led_slider, LV_ALIGN_TOP_MID, -7, 60);
    lv_slider_set_range(led_slider, 0, 255);
    lv_slider_set_value(led_slider, current_led_brightness, LV_ANIM_OFF);
    lv_obj_add_event_cb(led_slider, led_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Slider styling
    lv_obj_set_style_bg_color(led_slider, SPOTIFY_LIGHT_GREY, LV_PART_MAIN);
    lv_obj_set_style_bg_color(led_slider, SPOTIFY_GREEN, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(led_slider, SPOTIFY_WHITE, LV_PART_KNOB);
}

void create_brightness_slider(lv_obj_t* parent) {
    brightness_slider = lv_slider_create(parent);
    lv_obj_set_size(brightness_slider, LV_PCT(67), 10);
    lv_obj_align(brightness_slider, LV_ALIGN_TOP_MID, -7, 20);
    lv_slider_set_range(brightness_slider, 10, 255);
    lv_slider_set_value(brightness_slider, current_screen_brightness, LV_ANIM_OFF);
    lv_obj_add_event_cb(brightness_slider, brightness_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Slider styling
    lv_obj_set_style_bg_color(brightness_slider, SPOTIFY_LIGHT_GREY, LV_PART_MAIN);
    lv_obj_set_style_bg_color(brightness_slider, SPOTIFY_GREEN, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(brightness_slider, SPOTIFY_WHITE, LV_PART_KNOB);
}

void create_voltage_display(lv_obj_t* parent) {
    voltagedisplay = lv_label_create(parent);
    lv_label_set_text(voltagedisplay, "0.00V");  // Capitalized V for voltage
    
    // Position at the absolute top-right corner of the panel
    lv_obj_align(voltagedisplay, LV_ALIGN_TOP_RIGHT, -1, 1);
    
    // Style the label
    lv_obj_set_style_text_color(voltagedisplay, SPOTIFY_WHITE, 0);
    lv_obj_set_style_text_font(voltagedisplay, &lv_font_montserrat_12, 0);
    
    // Make sure label is visible and has enough space
    lv_obj_set_width(voltagedisplay, 60);
    lv_obj_clear_flag(voltagedisplay, LV_OBJ_FLAG_OVERFLOW_VISIBLE); // Ensure text stays within bounds
    
    // Remove any padding that might push it away from the edge
    lv_obj_set_style_pad_all(voltagedisplay, 0, 0);
    
    // Set long mode to make sure text is fully visible
    lv_label_set_long_mode(voltagedisplay, LV_LABEL_LONG_CLIP);
    
    // Set text alignment to right
    lv_obj_set_style_text_align(voltagedisplay, LV_TEXT_ALIGN_RIGHT, 0);
}
// Function to update all UI values after waking from sleep
void update_ui_values() {
    if (!screen_on) return;
    
    // Update sliders with stored values
    if (brightness_slider) {
        lv_slider_set_value(brightness_slider, current_screen_brightness, LV_ANIM_OFF);
    }
    
    if (led_slider) {
        lv_slider_set_value(led_slider, current_led_brightness, LV_ANIM_OFF);
    }
    
    // Update value labels
    if (brightness_value_label) {
        lv_label_set_text_fmt(brightness_value_label, "%d%%", (current_screen_brightness * 100) / 255);
    }
    
    if (led_value_label) {
        lv_label_set_text_fmt(led_value_label, "%d%%", (current_led_brightness * 100) / 255);
    }
    
    // Update voltage display if panel is visible
    if (panel_visible) {
        update_voltage_display();
    }
}
// Function to manage the voltage update timer
void toggle_voltage_timer(bool enable) {
    if (enable) {
        // Delete any existing timer to avoid duplicates
        if (voltage_update_timer) {
            lv_timer_del(voltage_update_timer);
            voltage_update_timer = NULL;
        }
        
        // Create a new timer with shorter interval for more responsive updates
        voltage_update_timer = lv_timer_create([](lv_timer_t* timer) {
            update_voltage_display();
        }, 500, NULL);
        
        // Do an immediate update to show voltage right away
        update_voltage_display();
        
        // Force a refresh of the UI
        lv_refr_now(NULL);
    } 
    else if (!enable && voltage_update_timer) {
        // Delete timer when panel is hidden
        lv_timer_del(voltage_update_timer);
        voltage_update_timer = NULL;
    }
}

static void brightness_gesture_cb(lv_event_t* e) {
    reset_screen_timeout(); // Reset timeout on interaction
    
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir != LV_DIR_TOP && dir != LV_DIR_BOTTOM) return;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, brightness_panel);
    lv_anim_set_time(&a, 200);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);

    if (dir == LV_DIR_BOTTOM && !panel_visible) {
        // Set flag first before animation
        panel_visible = true;
        
        // Start voltage updates immediately
        toggle_voltage_timer(true);
        
        // Then start animation
        lv_anim_set_values(&a, -100, 0);
        lv_anim_start(&a);
        
        // Add animation end callback to ensure values are updated after animation completes
        lv_anim_set_ready_cb(&a, [](lv_anim_t* a) {
            update_ui_values();
            // Force another voltage update after animation
            update_voltage_display();
            lv_refr_now(NULL);
        });
    } else if (dir == LV_DIR_TOP && panel_visible) {
        lv_anim_set_values(&a, 0, -100);
        lv_anim_start(&a);
        panel_visible = false;
        toggle_voltage_timer(false); // Stop voltage updates
    }
}

void create_pull_panel(lv_obj_t* parent) {
    brightness_panel = lv_obj_create(parent);
    lv_obj_set_size(brightness_panel, LV_PCT(100), 100);
    lv_obj_set_style_bg_color(brightness_panel, SPOTIFY_DARK_GREY, 0);
    lv_obj_set_style_radius(brightness_panel, 0, 0);
    lv_obj_set_style_border_width(brightness_panel, 0, 0);
    lv_obj_set_style_pad_all(brightness_panel, 2, 0);
    lv_obj_align(brightness_panel, LV_ALIGN_TOP_MID, 0, -100);

    // Create voltage display
    create_voltage_display(brightness_panel);
    
    // Create brightness control with icon
    create_brightness_icon(brightness_panel);
    create_brightness_slider(brightness_panel);
    
    // Create LED control with icon
    create_led_icon(brightness_panel);
    create_led_slider(brightness_panel);
}
// Handle waking from sleep
void on_wake_from_sleep() {
    update_ui_values();
    
    // Make sure voltage timer is running if panel is visible
    if (panel_visible) {
        toggle_voltage_timer(true);
    }
}

void ui_create() {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, SPOTIFY_BLACK, 0);

    create_pull_panel(scr);
    lv_obj_add_event_cb(scr, brightness_gesture_cb, LV_EVENT_GESTURE, NULL);
    
    // No longer create a timer here - it will be created dynamically when needed
    
    // Initialize voltage display if panel is visible (which it shouldn't be at start)
    if (panel_visible) {
        update_voltage_display();
    }
}

// Call this function in your code when the device wakes from sleep
// Add this to wherever you handle screen wakeup in your main code

void wake_up_handler() {
    screen_on = true;
    display_set_backlight(current_screen_brightness);
    on_wake_from_sleep();
}