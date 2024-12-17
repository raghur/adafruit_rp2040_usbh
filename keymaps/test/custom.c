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
#include "sm_td/sm_td.h"
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
const uint16_t PROGMEM l0_fd_combo[] = {KC_F, KC_D, COMBO_END};
const uint16_t PROGMEM l1_fd_combo[] = {CKC_F, CKC_D, COMBO_END};
const uint16_t PROGMEM l0_lk_combo[] = {KC_L, KC_K, COMBO_END};
const uint16_t PROGMEM l1_lk_combo[] = {KC_L, CKC_K, COMBO_END};
combo_t key_combos[] = {
    COMBO(jk_combo, KC_ESC),
    COMBO(l0_fd_combo, QK_LEAD),
    COMBO(l1_fd_combo, QK_LEAD),
    COMBO(l0_lk_combo, QK_LEAD),
    COMBO(l1_lk_combo, QK_LEAD),
    /* COMBO(ui_combo, QK_LEAD), */
};
#endif

#ifdef TAP_DANCE_ENABLE
tap_dance_action_t tap_dance_actions[] = {
    [TD_F9] = ACTION_TAP_DANCE_LAYER_MOVE(KC_F9, LYR_RGB),
    [TD_F10] = ACTION_TAP_DANCE_LAYER_MOVE(KC_F10, LYR_EXTRAKEYS),
    [TD_F11] = ACTION_TAP_DANCE_LAYER_MOVE(KC_F11, LYR_HRM),
    [TD_F12] = ACTION_TAP_DANCE_LAYER_MOVE(KC_F12, LYR_DEFAULT),
};
#endif



static uint8_t prev_mod_state;
static layer_state_t prev_layer_state;

void init_quantum_painter(void);
void updateLayerDisplay(layer_state_t layer, bool force);
void updateModDisplay(void);
void updateLeaderDisplay(bool isActive);
void qp_sleep(void);
void qp_wakeup(void);
#ifndef QUANTUM_PAINTER_ENABLE
void init_quantum_painter() {}
void updateLayerDisplay(layer_state_t layer, bool force) {}
void updateLeaderDisplay(bool isActive){}
void qp_sleep(){}
void qp_wakeup(){}
#endif

#ifdef QUANTUM_PAINTER_ENABLE
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define I2C_ADDRESS 0x3c
#define LINENO(v,lh) (v*lh)
static painter_device_t display;
static painter_font_handle_t my_font;
static int line_height;
static uint8_t logoIsDisplayed = 0;
void displayLogo(void);
void hideLogo(void);

void qp_sleep(){ qp_power(display, false); }
void qp_wakeup() { qp_power(display, true); }
void displayLogo() {
    if (logoIsDisplayed) return;
    logoIsDisplayed = 1;
    static const char *text = "QMK!";
    int16_t width = qp_textwidth(my_font, text);
    qp_drawtext(display, (DISPLAY_WIDTH - width)/2, (DISPLAY_HEIGHT - line_height) / 2, my_font, text);
    const char *version = PRODUCT;
    char *ptr = strchr(version, '.');
    if (ptr)
        qp_drawtext(display, 0, DISPLAY_HEIGHT - line_height, my_font, ++ptr);
    else
        qp_drawtext(display, 0, DISPLAY_HEIGHT - line_height, my_font, version);
}

void hideLogo() {
    if (!logoIsDisplayed) return;
    logoIsDisplayed = 0;
    const char *emptyline = "                             ";
    int16_t width = qp_textwidth(my_font, emptyline);
    qp_drawtext(display, (DISPLAY_WIDTH - width)/2, (DISPLAY_HEIGHT - line_height)/2, my_font, emptyline);
    qp_drawtext(display, 0, DISPLAY_HEIGHT - line_height, my_font, emptyline);
}
void init_quantum_painter(void) {
    display = qp_sh1106_make_i2c_device(DISPLAY_WIDTH, DISPLAY_HEIGHT, I2C_ADDRESS);
    qp_init(display, QP_ROTATION);
    my_font = qp_load_font_mem(font_monaspace);
    line_height = my_font->line_height;
    displayLogo();
    qp_drawtext(display, 0, LINENO(0, line_height), my_font, "DEFAULT  ");
}

void updateLayerDisplay(layer_state_t layer_state, bool force) {
    if (force || layer_state != prev_layer_state) {
        prev_layer_state = layer_state;
        switch (get_highest_layer(layer_state)) {
            case LYR_DEFAULT:
                qp_drawtext(display, 0, LINENO(0, line_height),my_font, "DEFAULT  ");
                break;
            case LYR_HRM:
                qp_drawtext(display, 0, LINENO(0, line_height),my_font, "HRM MODS ");
                break;
            case LYR_EXTRAKEYS:
                qp_drawtext(display, 0, LINENO(0, line_height),my_font, "EXTRAKEYS");
                break;
            case LYR_RGB:
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
    if (leader_sequence_one_key(TD(TD_F9))) {
        layer_invert(LYR_RGB);
    }
    if (leader_sequence_one_key(TD(TD_F10))) {
        layer_invert(LYR_EXTRAKEYS);
    }
    if (leader_sequence_one_key(TD(TD_F11))) {
        layer_invert(LYR_HRM);
    }
    if (leader_sequence_one_key(TD(TD_F12))) {
        layer_clear();
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
    init_quantum_painter();
    keyboard_post_init_user();
}

void keyboard_post_init_user(void) {

    #ifdef CONSOLE_ENABLE
        // Customise these values to desired behaviour
        debug_enable=true;
        /* debug_keyboard=true; */
        //debug_mouse=true;
    #endif
    rgblight_layers_init();
    /* updateLayerDisplay(layer_state, true); */
    uprintf("completed post_init_user");
}

layer_state_t layer_state_set_user(layer_state_t state) {
    updateLayerDisplay(state, false);
    return state;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_smtd(keycode, record)) {
        return false;
    }
    return true;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    rgblight_layers_update_mods();
    updateModDisplay();
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

void on_smtd_action(uint16_t keycode, smtd_action action, uint8_t tap_count) {
    switch (keycode) {
        SMTD_MT(CKC_D, KC_D, KC_LEFT_ALT)
        SMTD_MT(CKC_F, KC_F, KC_LEFT_CTRL)

        SMTD_MT(CKC_J, KC_J, KC_RIGHT_CTRL)
        SMTD_MT(CKC_K, KC_K, KC_RIGHT_ALT)
    }
}
