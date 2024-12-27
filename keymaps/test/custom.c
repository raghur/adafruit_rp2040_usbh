/* const key_override_t brightness_up_override = ko_make_with_layers_negmods_and_options(MOD_MASK_CA, KC_MPLY, */
/*                                             KC_BRIU, ~0, MOD_MASK_SHIFT, ko_option_no_reregister_trigger); */

/* const key_override_t brightness_down_override = ko_make_basic(MOD_MASK_CSA, KC_MPLY, KC_BRID); */
/* const key_override_t **key_overrides = (const key_override_t *[]){ */
/*     &brightness_up_override, */
/*     &brightness_down_override, */
/*     NULL */
/* }; */
#include "monaspace.qff.h"
#include "print.h"
#define MODS_SHIFT(v)  (v & MOD_MASK_SHIFT)
#define MODS_CTRL(v)   (v & MOD_MASK_CTRL)
#define MODS_ALT(v)    (v & MOD_MASK_ALT)
#define MODS_GUI(v)    (v & MOD_MASK_GUI)

#ifdef AUDIO_ENABLE

float leader_timedout[][2] = SONG(CAPS_LOCK_OFF_SOUND);
float leader_succeed_song[][2] = SONG(QWERTY_SOUND);
#endif



#ifdef COMBO_ENABLE
const uint16_t PROGMEM jk_combo[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM fd_combo[] = {KC_F, KC_D, COMBO_END};
const uint16_t PROGMEM lk_combo[] = {KC_L, KC_K, COMBO_END};
combo_t key_combos[] = {
    COMBO(jk_combo, KC_ESC),
    COMBO(fd_combo, QK_LEAD),
    COMBO(lk_combo, QK_LEAD),
};
#endif

#ifdef TAP_DANCE_ENABLE
tap_dance_action_t tap_dance_actions[] = {
    [TD_F10] = ACTION_TAP_DANCE_LAYER_MOVE(KC_F10, LYR_RGB),
    [TD_F11] = ACTION_TAP_DANCE_LAYER_MOVE(KC_F11, LYR_EXTRAKEYS),
    [TD_F12] = ACTION_TAP_DANCE_LAYER_MOVE(KC_F12, LYR_DEFAULT),
};
#endif



static uint8_t prev_mod_state;
static layer_state_t prev_layer_state;

void qp_connect(void);
void qp_welcome(void);
void updateLayerDisplay(layer_state_t layer, bool force);
void updateModDisplay(void);
void updateLeaderDisplay(bool isActive);
void qp_sleep(void);
void qp_wakeup(void);
void reset_idle_timer(void);
void backlight_sleep(void);
void backlight_wake(void);
uint32_t backlight_idle_sleep_checker(uint32_t trigger_time, void *cb_arg);
#ifndef QUANTUM_PAINTER_ENABLE
void qp_connect() {}
void qp_welcome() {}
void updateLayerDisplay(layer_state_t layer, bool force) {}
void updateLeaderDisplay(bool isActive){}
void qp_sleep(){}
void qp_wakeup(){}
#endif

static deferred_token bl_idle_token;
#ifdef BACKLIGHT_ENABLE
static uint8_t bl_level = BACKLIGHT_LEVELS;
static uint8_t idle_periods = 0;
#define CHECK_PERIOD 3000

void backlight_sleep(){
    bl_level = get_backlight_level();
    backlight_disable();
}
void backlight_wake(){
    if (is_backlight_enabled()) return;
    backlight_enable();
    backlight_level(bl_level);
}
void reset_idle_timer() {
    idle_periods = 0;
    backlight_wake();
}
uint32_t backlight_idle_sleep_checker(uint32_t trigger_time, void *cb_arg) {
    if (!is_backlight_enabled()) return CHECK_PERIOD;
    idle_periods++;
    if ((CHECK_PERIOD * idle_periods) >= QUANTUM_PAINTER_DISPLAY_TIMEOUT) {
        backlight_sleep();
        return CHECK_PERIOD;
    }
    return CHECK_PERIOD;
}
#else
#define CHECK_PERIOD 0
void backlight_wake() { }
void backlight_sleep() { }
void reset_idle_timer() { }
uint32_t backlight_idle_sleep_checker(uint32_t trigger_time, void *cb_arg ){ return 0;}
#endif

#ifdef QUANTUM_PAINTER_ENABLE
static painter_device_t display;
static painter_font_handle_t my_font;
static int line_height;
static uint8_t logoIsDisplayed = 0;


#define LINENO(v,lh) (v*lh)
void displayLogo(void);
void hideLogo(void);
void qp_connect() {
    #ifdef QP_1106i2c
    display = qp_sh1106_make_i2c_device(DISPLAY_WIDTH, DISPLAY_HEIGHT, I2C_ADDRESS);
    #endif

    #ifdef QP_7735spi
    display = qp_st7735_make_spi_device(DISPLAY_WIDTH, DISPLAY_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN, 2, 0);
    bl_idle_token = defer_exec(CHECK_PERIOD, backlight_idle_sleep_checker, NULL);
    #endif
}
void qp_welcome(void) {
    backlight_wake();
    qp_init(display, QP_ROTATION_0);
    qp_rect(display, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, 0, 0, 1);
    qp_clear(display);
    my_font = qp_load_font_mem(font_monaspace);
    static const char *text = "QMK!";
    int16_t width = qp_textwidth(my_font, text);
    line_height = my_font->line_height;
    /* qp_drawtext(display, (DISPLAY_WIDTH - width)/2, (DISPLAY_HEIGHT - line_height)/2, my_font, text); */
    qp_drawtext_recolor(display, (DISPLAY_WIDTH - width)/2, (DISPLAY_HEIGHT - line_height)/2, my_font, text, 120, 75,100, 230,75,0);
    qp_drawtext_recolor(display, 0, LINENO(0, line_height), my_font, "DEFAULT  ", 230, 75,100, 230, 75, 0);
    logoIsDisplayed = 1;
}

void qp_sleep(){
    qp_power(display, false);
}
void qp_wakeup() {
    qp_power(display, true);
}
void displayLogo() {
    static const char *text = "QMK!";
    int16_t width = qp_textwidth(my_font, text);
    qp_drawtext(display, (DISPLAY_WIDTH - width)/2, (DISPLAY_HEIGHT - line_height)/2, my_font, text);
}

void hideLogo() {
    if (logoIsDisplayed) {
        logoIsDisplayed = 0;
        static const char *text = "    ";
        int16_t width = qp_textwidth(my_font, text);
        qp_drawtext(display, (DISPLAY_WIDTH - width)/2, (DISPLAY_HEIGHT - line_height)/2, my_font, text);
    }
}

void updateLayerDisplay(layer_state_t layer_state, bool force) {
    if (force || layer_state != prev_layer_state) {
        prev_layer_state = layer_state;
        switch (get_highest_layer(layer_state)) {
            case 0:
                qp_drawtext(display, 0, LINENO(0, line_height),my_font, "DEFAULT  ");
                break;
            case 1:
                qp_drawtext(display, 0, LINENO(0, line_height),my_font, "EXTRAKEYS");
                break;
            case 2:
                qp_drawtext(display, 0, LINENO(0, line_height),my_font, "CONTROL  ");
                break;
            default:
                // Or use the write_ln shortcut over adding '\n' to the end of your string
                // oled_write_ln_P(PSTR("NA"), false);
                break;
        }
    }
}

void updateModDisplay() {
    hideLogo();
    int mod_state = get_mods();
    char buffer[20];
    if (mod_state != prev_mod_state) {
        prev_mod_state = mod_state;
        char* SHIFT = (mod_state & MOD_MASK_SHIFT) ? "SHF": "   ";
        char* CTRL = (mod_state & MOD_MASK_CTRL) ? "CTL": "   ";
        char* ALT = (mod_state & MOD_MASK_ALT) ? "ALT": "   ";
        char* GUI = (mod_state & MOD_MASK_GUI) ? "GUI": "   ";
        sprintf(buffer, "%s %s %s %s", SHIFT, CTRL, ALT, GUI);
        qp_drawtext(display, 0, LINENO(1, line_height), my_font, buffer);
    }
}
void updateLeaderDisplay(bool isActive) {
    char *text = isActive ? "LEADER...": "         ";
    qp_drawtext(display, 0, LINENO(2, line_height), my_font, text);
}
#endif

void rgblight_layers_init(void);
void rgblight_layers_update_mods(void);
void rgblight_default_layer_set(layer_state_t state);

#ifndef RGBLIGHT_ENABLE
void rgblight_layers_init() {}
void rgblight_layers_update_mods() {}
void rgblight_default_layer_set(layer_state_t state) {}
#endif

#ifdef RGBLIGHT_ENABLE

const rgblight_segment_t PROGMEM extra_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {8, 4, HSV_BLUE}       // Light 4 LEDs, starting with LED 6
);
const rgblight_segment_t PROGMEM rgb_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {8, 4, HSV_RED}       // Light 4 LEDs, starting with LED 6
);

const rgblight_segment_t PROGMEM my_alt_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {4, 2, HSV_GREEN},
    {14, 2, HSV_GREEN}
);
const rgblight_segment_t PROGMEM my_shift_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, 2, HSV_ORANGE},
    {18, 2, HSV_ORANGE}
);
const rgblight_segment_t PROGMEM my_ctrl_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {2, 2,  HSV_PINK},
    {16, 2,  HSV_PINK}
);
const rgblight_segment_t PROGMEM my_gui_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {6, 2,  HSV_MAGENTA},
    {12, 2,  HSV_MAGENTA}
);
const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    extra_layer,    // Overrides other layers
    rgb_layer, // Overrides other layers
    my_alt_layer,
    my_shift_layer,
    my_ctrl_layer,
    my_gui_layer
);
void rgblight_layers_init() {
    // Enable the LED layers
    rgblight_layers = my_rgb_layers;
    rgblight_set_effect_range(0, RGBLIGHT_LED_COUNT);
}
void rgblight_layers_update_mods() {
    int mods = get_mods() | get_oneshot_mods();
    if ( (mods  & MOD_MASK_SHIFT)
        || (mods & MOD_MASK_ALT)
        || (mods & MOD_MASK_GUI)
        || (mods & MOD_MASK_CTRL)){
        rgblight_set_layer_state(2, MODS_ALT(mods));
        rgblight_set_layer_state(3, MODS_SHIFT(mods));
        rgblight_set_layer_state(4, MODS_CTRL(mods));
        rgblight_set_layer_state(5, MODS_GUI(mods));

    } else {
        rgblight_set_layer_state(2, 0);
        rgblight_set_layer_state(3, 0);
        rgblight_set_layer_state(4, 0);
        rgblight_set_layer_state(5, 0);
    }
}
void rgblight_default_layer_set(layer_state_t state) {
    rgblight_set_layer_state(0, layer_state_cmp(state, LYR_EXTRAKEYS));
    rgblight_set_layer_state(1, layer_state_cmp(state, LYR_RGB));
}
#endif




#ifdef LEADER_ENABLE
uint8_t leaderCSFT = 0;
void toggleCSFT (void) {
    if (!leaderCSFT) {
        register_mods(MOD_LCTL);
        register_mods(MOD_LSFT);
    }else {
        unregister_mods(MOD_LSFT);
        unregister_mods(MOD_LCTL);
    }

    leaderCSFT = !leaderCSFT;
}

void leader_start_user(void) {
    updateLeaderDisplay(true);
}

void leader_end_user(void) {
    if (leader_sequence_timed_out()) {
        #ifdef AUDIO_ENABLE
        PLAY_SONG(leader_timedout);
        #endif
    }

    bool did_succeed = false;
    if (leader_sequence_two_keys(KC_E, KC_E)) {
        SEND_STRING("raghu.rajagopalan@gmail.com");
        did_succeed = true;
    } else if (leader_sequence_two_keys(KC_E, KC_S)) {
        SEND_STRING("raghu.nospam@gmail.com");
        did_succeed = true;
    } else if (leader_sequence_two_keys(KC_E, KC_W)) {
        SEND_STRING("raghu.rajagopalan@rockwellautomation.com");
        did_succeed = true;
    } else if (leader_sequence_two_keys(KC_E, KC_D)) {
        SEND_STRING("rockwellautomation.com");
        did_succeed = true;
    }
    if (leader_sequence_one_key(KC_L)) {
        SEND_STRING("ra-int\\rrajagopala");
        did_succeed = true;
    }
    if (leader_sequence_one_key(KC_F11)) {
        layer_on(LYR_EXTRAKEYS);
    }
    if (leader_sequence_one_key(KC_MINS)) {
        SEND_STRING("->");
        did_succeed = true;
    }
    if (leader_sequence_one_key(KC_EQL)) {
        SEND_STRING("=>");
        did_succeed = true;
    }
    if (leader_sequence_one_key(KC_M)) {
        register_code(KC_LGUI);
        register_code(KC_F12);
        unregister_code(KC_F12);
        unregister_code(KC_LGUI);
        did_succeed = true;
    }
    if (leader_sequence_one_key(KC_V)) {
        register_code(KC_LALT);
        register_code(KC_LCTL);
        register_code(KC_V);
        unregister_code(KC_V);
        unregister_code(KC_LCTL);
        unregister_code(KC_LALT);
        did_succeed = true;
    }
    if (did_succeed) {

#ifdef AUDIO_ENABLE
     PLAY_SONG(leader_succeed_song);
#endif
    }

    updateLeaderDisplay(false);
}

#endif

void keyboard_post_init_kb(void) {
    #ifdef CONSOLE_ENABLE
        // Customise these values to desired behaviour
        debug_enable=true;
        debug_keyboard=true;
        //debug_mouse=true;
    #endif
    keyboard_post_init_user();
    dprintf("started post_init_kb\n");
}

uint32_t qp_start_job(uint32_t trigger_time, void *cb_arg);
uint32_t qp_start_job(uint32_t trigger_time, void *cb_arg) {
    qp_connect();
    qp_welcome();
    return 0;
}
void keyboard_post_init_user(void) {

    dprintf("started post_init_user\n");
    defer_exec(5000, qp_start_job, NULL);
    /* qp_connect(); */
    /* qp_welcome(); */
    rgblight_layers_init();
    /* updateLayerDisplay(layer_state, true); */
    dprintf("completed post_init_user\n");
}

layer_state_t layer_state_set_user(layer_state_t state) {
    updateLayerDisplay(state, false);
    return state;
}
void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    rgblight_layers_update_mods();
    updateModDisplay();
    if (record->event.pressed) {
        reset_idle_timer();
    }
}
layer_state_t default_layer_state_set_user(layer_state_t state) {
    rgblight_default_layer_set(state);
    return state;
}

void suspend_power_down_user(void) {
    // code will run multiple times while keyboard is suspended
    qp_sleep();
}

void suspend_wakeup_init_user(void) {
    // code will run on keyboard wakeup
    qp_wakeup();
}
