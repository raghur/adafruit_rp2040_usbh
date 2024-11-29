EXTRAKEY_ENABLE=yes
TAP_DANCE_ENABLE=yes
OLED_ENABLE=yes
LEADER_ENABLE=yes
RGBLIGHT_ENABLE=no
WS2812_DRIVER=vendor

INTROSPECTION_KEYMAP_C = custom.c

COMMAND_ENABLE=yes
MAGIC_ENABLE=yes
COMBO_ENABLE=yes
SPACE_CADET_ENABLE=no

DYNAMIC_MACRO_ENABLE=yes
MOUSEKEY_ENABLE=yes

# too much space
AUDIO_ENABLE=no
# deprecated in favor of MAGIC

#linux doesn't recognize...
PROGRAMMABLE_BUTTON_ENABLE = no
# only for tiny kbds without dedicated esc
GRAVE_ESC_ENABLE=no

CONSOLE_ENABLE=yes
UNICODE_ENABLE=no
KEY_OVERRIDE_ENABLE=no
TERMINAL_ENABLE=no
