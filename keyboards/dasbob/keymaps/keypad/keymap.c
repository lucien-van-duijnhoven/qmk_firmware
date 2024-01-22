#include QMK_KEYBOARD_H

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

// Tap dance functions
td_state_t cur_dance(tap_dance_state_t *state);

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_TAP_INTERRUPTED,
    TD_SINGLE_HOLD,
    TD_SINGLE_HOLD_INTERRUPTED,
} td_state_t;

td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
      if (state->interrupted && !state->pressed) return TD_SINGLE_TAP_INTERRUPTED;
      else if (state->interrupted && state->pressed) return TD_SINGLE_HOLD_INTERRUPTED;
      else if (state->pressed) return TD_SINGLE_HOLD;
      else return TD_SINGLE_TAP;
    }
}

enum {
    SHIFT,
    CONTROL,
    ALT,
};



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x5_3(
        KC_F21, KC_F22, KC_F23, KC_F24, KC_ESC,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,

        KC_F17, KC_F18, KC_F19, KC_F20, KC_ESC,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,

        KC_F13, KC_F14, KC_F15, KC_F16, KC_ESC,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,

        OSM(MOD_LSFT), OSM(MOD_LCTL), OSM(MOD_LALT), 
        KC_NO, KC_NO, KC_NO
    ),
};

