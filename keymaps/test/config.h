#pragma once
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET // Activates the double-tap behavior
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 200U // Timeout window in ms in which the double tap can occur.
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED GP25 // Specify a optional status led by GPIO number which blinks when entering the bootloader

#define MOUSEKEY_INTERVAL 16
#define MOUSEKEY_MAX_SPEED 7
#define MOUSEKEY_DELAY 0
#define MOUSEKEY_TIME_TO_MAX 40
#define MOUSEKEY_WHEEL_DELAY 0
#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE

#define QP_7735spi
/* #define QP_1106i2c */

#ifdef QP_7735spi
#define SPI_DRIVER SPID1
#define SPI_SCK_PIN GP10
#define SPI_MOSI_PIN GP11
#define TFT_CS_PIN GP9
#define TFT_DC_PIN GP12
#define TFT_RST_PIN GP13
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 160

/// use PWM on GP3 to drive backlight levels on ST7735 TFT LCD display
#define BACKLIGHT_PIN GP29
#define BACKLIGHT_PWM_DRIVER PWMD6
#define BACKLIGHT_PWM_CHANNEL RP2040_PWM_CHANNEL_B
/* #define BACKLIGHT_LEVELS 5 */
/* #define BACKLIGHT_DEFAULT_LEVEL BACKLIGHT_LEVELS */
/* #define BACKLIGHT_ON_STATE 1 */
#endif

#ifdef QP_1106i2c
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define I2C_ADDRESS 0x3c
#endif

#define TAPPING_TOGGLE 2
#define LAYER_STATE_8BIT
#define OLED_DISPLAY_128X64
#define OLED_TIMEOUT 10000
#define OLED_FADE_OUT
#define OLED_FADE_OUT_INTERVAL 2

#define COMBO_TERM 100

/// leader options - backslash mod_tap
#define LEADER_TIMEOUT 300
// time between each leader
#define LEADER_PER_KEY_TIMING
// no timeout after leader initialized
#define LEADER_NO_TIMEOUT

#ifdef AUDIO_ENABLE
#define AUDIO_PIN GP15
#define AUDIO_PWM_DRIVER PWMD7
#define AUDIO_PWM_CHANNEL RP2040_PWM_CHANNEL_B
#define AUDIO_ENABLE_TONE_MULTIPLEXING
#define AUDIO_TONE_MULTIPLEXING_RATE_DEFAULT 10
#define AUDIO_INIT_DELAY
#define STARTUP_SONG SONG(STARTUP_SOUND)
#endif
/* #define AUDIO_CLICKY */

// tap dance and related
#define TAPPING_TERM 300
#define RETRO_TAPPING

#ifdef RGBLIGHT_ENABLE
#define WS2812_PIO_USE_PIO1

// rgb layers
// underglow related
#define RGBLIGHT_LAYERS
#define RGBLIGHT_SLEEP
#define RGBLIGHT_LED_COUNT 1
#define RGBLIGHT_LIMIT_VAL 180
#define RGBLIGHT_MAX_LAYERS 8

// effects
#define RGBLIGHT_EFFECT_ALTERNATING
/* #define RGBLIGHT_EFFECT_BREATHING */
/* #define RGBLIGHT_EFFECT_CHRISTMAS */
/* #define RGBLIGHT_EFFECT_KNIGHT */
/* #define RGBLIGHT_EFFECT_RAINBOW_MOOD */
/* #define RGBLIGHT_EFFECT_RAINBOW_SWIRL */
/* #define RGBLIGHT_EFFECT_RGB_TEST */
/* #define RGBLIGHT_EFFECT_SNAKE */
/* #define RGBLIGHT_EFFECT_STATIC_GRADIENT */
#define RGBLIGHT_EFFECT_TWINKLE

#define RGBLIGHT_LAYERS_OVERRIDE_RGB_OFF
#define RGBLIGHT_LAYERS_RETAIN_VAL

#define WS2812_DI_PIN GP16
/* #define USE_GET_MILLISECOND_TIMER */

//layer blink
#define RGBLIGHT_LAYER_BLINK
#define RGBLIGHT_BLINK_DURATION 500

// custom
#define ENABLE_LAYER_LED
#endif
