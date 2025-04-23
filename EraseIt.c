 #include <stdlib.h>
 #include <stdio.h>
 #include <ctype.h>

 #include "pico/stdlib.h"
 #include "pico/bootrom.h"
 #include "hardware/timer.h"
 #include "hardware/pwm.h"
 #include "hardware/clocks.h"

 #include "lib/rgb.h"
 #include "lib/joystick.h"
 #include "lib/oledgfx.h"
 #include "lib/push_button.h"
 #include "lib/ws2812b.h"
 #include "lib/mlt8530.h"
 
 // Hardware Configuration
 // ====================
 
 /// @brief I2C port used for OLED communication
 #define I2C_PORT i2c1
 
 /// @brief OLED display pin configuration
 #define OLED_SDA 14      ///< Serial data pin
 #define OLED_SCL 15      ///< Serial clock pin
 #define OLED_ADDR 0x3C   ///< I2C address of OLED
 #define OLED_BAUDRATE 400000  ///< I2C communication speed
 
 /// @brief Joystick pin configuration
 #define JOYSTICK_VRX 27  ///< X-axis analog input
 #define JOYSTICK_VRY 26  ///< Y-axis analog input
 #define JOYSTICK_PB  22  ///< Push button input
 
 /// @brief Buzzer pin configuration
 #define BUZZER_A 10      ///< Primary buzzer pin
 #define BUZZER_B 21      ///< Secondary buzzer pin
 
 /// @brief RGB LED pin configuration
 #define RED_PIN   13     ///< Red LED PWM pin
 #define BLUE_PIN  12     ///< Blue LED PWM pin
 #define GREEN_PIN 11     ///< Green LED PWM pin
 
 // Game Constants
 // =============
 
 /// @brief Button press detection macros
 #define JOYSTICK_SW_PRESSED (gpio == 22)
 #define BUTTON_A_PRESSED (gpio == BUTTON_A)
 #define BUTTON_B_PRESSED (gpio == BUTTON_B)
 
 /// @brief Game state definitions
 #define GAME_STATUS_WAITING 0  ///< Initial waiting state
 #define GAME_STATUS_START   1  ///< Game active state
 #define GAME_STATUS_END     3  ///< Game over state
 
 /// @brief Enable USB boot mode for firmware updates
 #define set_bootsel_mode() reset_usb_boot(0, 0)
 
 // Global Variables
 // ================
 
 /// @brief Countdown timer value (9-0)
 static volatile uint8_t timer_counter = 9;
 
 /// @brief Current game state
 static volatile uint8_t game_status = GAME_STATUS_WAITING;
 
 /// @brief Number of pixels remaining after game ends
 static volatile uint16_t cleared_display_bits = 0;
 
 /// @brief LED control states
 static volatile bool led_red_active = false;
 static volatile bool led_green_active = false;
 static volatile bool led_blue_active = false;
 static volatile bool led_control_override = false; ///< When true, overrides individual LED states
 
 /// @brief Global display and LED matrix references
 static ssd1306_t *ssd_global = NULL;  ///< OLED display object
 static ws2812b_t *ws_global = NULL;   ///< WS2812B LED matrix object
 
 // Predefined colors for WS2812B display
 static const uint8_t COLORS[] = {
     PURPLE, GREEN, BLUE_MARINE, RED, 
     YELLOW, BLUE, GREEN, BLUE, YELLOW
 };
 
 // Function Prototypes
 // ==================
 
 /**
  * @brief Normalizes joystick input to display coordinates
  * @param joystick_vr Raw joystick value (0-4095)
  * @param new_max Maximum value in target range
  * @return Normalized value in display coordinate space
  */
 static uint16_t normalize_joystick_to_display(uint16_t joystick_vr, uint8_t new_max);
 
 /**
  * @brief Adjusts PWM LED value based on joystick position
  * @param pwm_value Raw joystick value (0-4095)
  * @return Adjusted PWM value (0-2048) for LED brightness
  */
 static uint16_t adjust_pwm_led_value(uint16_t pwm_value);
 
 /**
  * @brief GPIO interrupt callback for button presses
  * @param gpio GPIO pin that triggered interrupt
  * @param event Type of interrupt event
  */
 static void gpio_irq_callback(uint gpio, uint32_t event);
 
 /**
  * @brief Timer callback for game countdown
  * @param t Pointer to repeating timer structure
  * @return Always returns true to continue timer
  */
 bool repeating_timer_callback(struct repeating_timer *t);
 
 // Main Application
 // ================
 
 /**
  * @brief Main application entry point
  * @return Exit status (always EXIT_SUCCESS)
  */
 int main()
 {
     // Initialize system clock to 128MHz (required for LED matrix timing)
     set_sys_clock_khz(128000, false);
     stdio_init_all();  // Initialize USB/UART communication
 
     // Hardware objects
     rgb_t rgb;
     joystick_t joy;
     ssd1306_t ssd;
     struct repeating_timer timer;
     
     // Game state variables
     uint16_t adj_led_red_pwm_value, adj_led_blue_pwm_value;
     uint8_t joystick_vrx_norm, joystick_vry_norm;
     uint16_t joystick_vrx, joystick_vry;
     char cleared_bits_buffer[16];
 
     // Initialize hardware components
     rgb_init_all(&rgb, RED_PIN, GREEN_PIN, BLUE_PIN, 1.0, 2048);
     joystick_init_all(&joy, JOYSTICK_VRX, JOYSTICK_VRY, JOYSTICK_PB, 120);
     oledgfx_init_all(&ssd, I2C_PORT, OLED_BAUDRATE, OLED_SDA, OLED_SCL, OLED_ADDR);
     ssd_global = &ssd;  // Store global reference to OLED
 
     // Configure buttons and interrupts
     pb_config(JOYSTICK_PB, true);
     pb_config_btn_a();
     pb_config_btn_b();
     pb_set_irq_callback(&gpio_irq_callback);
     pb_enable_irq(BUTTON_A);
     pb_enable_irq(JOYSTICK_PB);
     pb_enable_irq(BUTTON_B);
 
     // Initialize buzzers
     buzzer_init(BUZZER_A);
     buzzer_init(BUZZER_B);
 
     // Initialize WS2812B LED matrix
     ws_global = init_ws2812b(pio0, WS2812B_PIN);
     ws2812b_turn_off_all(ws_global);
     pwm_set_gpio_level(GREEN_PIN, 128);  // Set initial green LED state
 
     printf("System initialized...\n");
 
     // Main game loop
     while(true) {
         if(game_status == GAME_STATUS_START) {
             // Game active state
             buzzer_beep(BUZZER_A, 100, 2000);
             printf("Game started\n");
             
             // Initialize countdown display
             ws2812b_draw(ws_global, NUMERIC_GLYPHS[timer_counter], COLORS[timer_counter], 1);
             
             // Start 1-second countdown timer
             add_repeating_timer_ms(1000, repeating_timer_callback, NULL, &timer);
             
             // Fill display with random pixels to erase
             oledgfx_random_fill_display(ssd_global);
 
             // Gameplay loop
             while(game_status == GAME_STATUS_START) {
                 // Draw game border
                 oledgfx_draw_border(&ssd, BORDER_LIGHT);
 
                 // Read and process joystick input
                 joystick_vrx = joystick_get_x(&joy);
                 joystick_vry = joystick_get_y(&joy);
         
                 // Normalize joystick values for display coordinates
                 joystick_vrx_norm = normalize_joystick_to_display(joystick_vrx, 127 - CURSOR_SIDE - BORDER_LIGHT);
                 joystick_vry_norm = (63 - CURSOR_SIDE) - normalize_joystick_to_display(joystick_vry, 63 - CURSOR_SIDE - BORDER_LIGHT);
         
                 // Update cursor position and refresh display
                 oledgfx_update_cursor(&ssd, joystick_vrx_norm, joystick_vry_norm);
                 oledgfx_render(&ssd);
     
                 // Update LED brightness based on joystick position
                 adj_led_red_pwm_value = adjust_pwm_led_value(joystick_vrx);
                 adj_led_blue_pwm_value = adjust_pwm_led_value(joystick_vry);
                 pwm_set_gpio_level(BLUE_PIN, adj_led_blue_pwm_value);
                 pwm_set_gpio_level(RED_PIN, adj_led_red_pwm_value);
             }
             
             printf("Game ended\n");
             buzzer_beep(BUZZER_A, 2000, 200);
             cleared_display_bits = oledgfx_count_lit_pixels(ssd_global);
             printf("%u pixels remain\n", cleared_display_bits);
         }
         else if(game_status == GAME_STATUS_WAITING) {
             // Waiting for game start state
             oledgfx_clear_screen(ssd_global);
             oledgfx_draw_border(ssd_global, BORDER_THICK);
             
             // Draw menu screen
             ssd1306_draw_string(ssd_global, "Erase It", 30, 8);
             oledgfx_draw_hline(ssd_global, 16, BORDER_LIGHT);
             ssd1306_draw_string(ssd_global, "A to start", 20, 24);
             ssd1306_draw_string(ssd_global, "SW reestart", 20, 32);
             
             // Display remaining pixels from previous game
             sprintf(cleared_bits_buffer, "%u pel reman", cleared_display_bits);
             ssd1306_draw_string(ssd_global, cleared_bits_buffer, 5, 40);
             ssd1306_send_data(ssd_global);
             
             sleep_ms(50);  // Reduce CPU usage in waiting state
         }
         else if(game_status == GAME_STATUS_END) {
             // Game over state - cleanup
             cancel_repeating_timer(&timer);
             timer_counter = 9;  // Reset countdown
             
             // Turn off LEDs
             pwm_set_gpio_level(BLUE_PIN, 0);
             pwm_set_gpio_level(RED_PIN, 0);
             pwm_set_gpio_level(GREEN_PIN, 128);
             ws2812b_turn_off_all(ws_global);
             
             // Return to waiting state
             game_status = GAME_STATUS_WAITING;
         }
     }
     
     return EXIT_SUCCESS;
 }
 
 // Helper Functions Implementation
 // ===============================
 
 static uint16_t normalize_joystick_to_display(uint16_t joystick_vr, uint8_t new_max)
 {
     return (joystick_vr * new_max) / 4095;
 }
 
 static void gpio_irq_callback(uint gpio, uint32_t event)
 {
     if(pb_is_debounce_delay_over()) {
         if(BUTTON_B_PRESSED) {
             // Enter USB boot mode for firmware updates
             set_bootsel_mode();
         }
         else if(BUTTON_A_PRESSED) {
             // Start game
             game_status = GAME_STATUS_START;
             pwm_set_gpio_level(GREEN_PIN, 0);  // Turn off green LED
         }
         else if(JOYSTICK_SW_PRESSED) {
             // End game
             game_status = GAME_STATUS_END;
         }
     }
 }
 
 static uint16_t adjust_pwm_led_value(uint16_t pwm_value)
 {
     // Convert joystick position to LED brightness
     return (pwm_value >= 2048) ? (pwm_value - 2048) : (2048 - pwm_value);
 }
 
 bool repeating_timer_callback(struct repeating_timer *t) {
     // Update countdown display
     timer_counter--;
     ws2812b_draw(ws_global, NUMERIC_GLYPHS[timer_counter], COLORS[timer_counter], 1);
     
     // End game when countdown reaches 0
     if(timer_counter == 0) {
         cancel_repeating_timer(t);
         game_status = GAME_STATUS_END;
         timer_counter = 9;  // Reset for next game
     }
     
     return true;  // Continue timer
 }