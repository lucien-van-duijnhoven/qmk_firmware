#include QMK_KEYBOARD_H
#include "../../features/custom_shift_keys.h"

enum custom_keycodes {
    RESET = SAFE_RANGE,
};

const custom_shift_key_t custom_shift_keys[] = {
    {KC_SLASH, KC_EXLM}, // Shift / is !
    {KC_DOT, KC_UNDS }, // Shift . is _
    {KC_COMM, KC_MINS }, // Shift , is -
    {KC_COLN, KC_SCLN}, // Shift : is ;
    {KC_UP, KC_PGUP}, // Shift Up is Page Up
    {KC_DOWN, KC_PGDN}, // Shift Down is Page Down
    {KC_LEFT, KC_HOME}, // Shift Left is Home
    {KC_RIGHT, KC_END}, // Shift Right is End
};

uint8_t NUM_CUSTOM_SHIFT_KEYS = sizeof(custom_shift_keys) / sizeof(custom_shift_key_t);

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    if (!process_custom_shift_keys(keycode, record)) { return false; } 

    switch (keycode) {
        case RESET:
            if (record->event.pressed) {   
                clear_oneshot_mods();
                reset_oneshot_layer();
                layer_move(0);
            }
            break;
    } 

    return true;
}

enum dasbob_layers {
  _QWERTY,
  _LOWER,
  _RAISE
};

enum {
    SHIFT_ALT,
    CONTROL_SUPER,
    SHIFT_CONTROL,
};

// Tap dance enums
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_TAP_INTERRUPTED,
    TD_SINGLE_HOLD,
    TD_SINGLE_HOLD_INTERRUPTED,
    TD_DOUBLE_TAP,
    TD_DOUBLE_TAP_INTERRUPTED,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_HOLD_INTERRUPTED,
    TD_TRIPLE_TAP,
    TD_TRIPLE_TAP_INTERRUPTED,
    TD_TRIPLE_HOLD,
    TD_TRIPLE_HOLD_INTERRUPTED,
} td_state_t;

// Tap dance state type
typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

// Tap dance functions
td_state_t cur_dance(qk_tap_dance_state_t *state);

void shift_alt_finished(qk_tap_dance_state_t *state, void *user_data);
void shift_alt_reset(qk_tap_dance_state_t *state, void *user_data);

void control_super_finished(qk_tap_dance_state_t *state, void *user_data);
void control_super_reset(qk_tap_dance_state_t *state, void *user_data);

void reset_on_tap_4(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count >= 4) {
        reset_tap_dance(state);
    }
}

td_state_t cur_dance(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
      if (state->interrupted && !state->pressed) return TD_SINGLE_TAP_INTERRUPTED;
      else if (state->interrupted && state->pressed) return TD_SINGLE_HOLD_INTERRUPTED;
      else if (state->pressed) return TD_SINGLE_HOLD;
      else return TD_SINGLE_TAP;
    } else if (state->count == 2) {
      if (state->interrupted && !state->pressed) return TD_DOUBLE_TAP_INTERRUPTED;
      else if (state->interrupted && state->pressed) return TD_DOUBLE_HOLD_INTERRUPTED;
      else if (state->pressed) return TD_DOUBLE_HOLD;
      else return TD_DOUBLE_TAP;
    }

    else if (state->count == 3) {
      if (state->interrupted && !state->pressed) return TD_TRIPLE_TAP_INTERRUPTED;
      else if (state->interrupted && state->pressed) return TD_TRIPLE_HOLD_INTERRUPTED;
      else if (state->pressed) return TD_TRIPLE_HOLD;
      else return TD_TRIPLE_TAP;
    } else return TD_UNKNOWN;
}

// SHIFT_ALT
static td_tap_t shift_alt_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void shift_alt_finished(qk_tap_dance_state_t *state, void *user_data) {
    shift_alt_state.state = cur_dance(state);
    switch (shift_alt_state.state) {
        case TD_SINGLE_TAP_INTERRUPTED:
        case TD_SINGLE_TAP: set_oneshot_mods(MOD_BIT(KC_LSFT)); break;
        case TD_SINGLE_HOLD_INTERRUPTED:
        case TD_SINGLE_HOLD: register_code(KC_LSFT); break;
        case TD_DOUBLE_TAP_INTERRUPTED:
        case TD_DOUBLE_TAP: set_oneshot_mods(MOD_BIT(KC_LALT)); break;
        case TD_DOUBLE_HOLD_INTERRUPTED:
        case TD_DOUBLE_HOLD: register_code(KC_LALT); break;
        case TD_TRIPLE_TAP_INTERRUPTED:
        case TD_TRIPLE_TAP: set_oneshot_mods(MOD_BIT(KC_LSFT)|MOD_BIT(KC_LALT)); break;
        case TD_TRIPLE_HOLD_INTERRUPTED:
        case TD_TRIPLE_HOLD: register_code(KC_LSFT); register_code(KC_LALT); break;
        default:
            break;
    }
}

void shift_alt_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (shift_alt_state.state) {
        case TD_SINGLE_HOLD_INTERRUPTED:
        case TD_SINGLE_HOLD: unregister_code(KC_LSFT); break;
        case TD_DOUBLE_HOLD_INTERRUPTED:
        case TD_DOUBLE_HOLD: unregister_code(KC_LALT); break;
        case TD_TRIPLE_HOLD_INTERRUPTED:
        case TD_TRIPLE_HOLD: unregister_code(KC_LSFT); unregister_code(KC_LALT); break;
        default:
            break;
    }
    shift_alt_state.state = TD_NONE;
}

// CONTROL_SUPER
static td_tap_t control_super_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void control_super_finished(qk_tap_dance_state_t *state, void *user_data) {
    control_super_state.state = cur_dance(state);
    switch (control_super_state.state) {
        case TD_SINGLE_TAP_INTERRUPTED:
        case TD_SINGLE_TAP: set_oneshot_mods(MOD_BIT(KC_LCTL)); break;
        case TD_SINGLE_HOLD_INTERRUPTED:
        case TD_SINGLE_HOLD: register_code(KC_LCTL); break;
        case TD_DOUBLE_TAP_INTERRUPTED:
        case TD_DOUBLE_TAP: set_oneshot_mods(MOD_BIT(KC_LGUI)); break;
        case TD_DOUBLE_HOLD_INTERRUPTED:
        case TD_DOUBLE_HOLD: register_code(KC_LGUI); break;
        case TD_TRIPLE_TAP_INTERRUPTED:
        case TD_TRIPLE_TAP: set_oneshot_mods(MOD_BIT(KC_LCTL)|MOD_BIT(KC_LGUI)); break;
        case TD_TRIPLE_HOLD_INTERRUPTED:
        case TD_TRIPLE_HOLD: register_code(KC_LCTL); register_code(KC_LGUI); break;
        default:
            break;
    }
}

void control_super_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (control_super_state.state) {
        case TD_SINGLE_HOLD_INTERRUPTED:
        case TD_SINGLE_HOLD: unregister_code(KC_LCTL); break;
        case TD_DOUBLE_HOLD_INTERRUPTED:
        case TD_DOUBLE_HOLD: unregister_code(KC_LGUI); break;
        case TD_TRIPLE_HOLD_INTERRUPTED:
        case TD_TRIPLE_HOLD: unregister_code(KC_LCTL); unregister_code(KC_LGUI); break;
        default:
            break;
    }
    control_super_state.state = TD_NONE;
}

// SHIFT_CONTROL
static td_tap_t shift_control_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void shift_control_finished(qk_tap_dance_state_t *state, void *user_data) {
    shift_control_state.state = cur_dance(state);
    switch (shift_control_state.state) {
        case TD_SINGLE_TAP_INTERRUPTED:
        case TD_SINGLE_TAP: set_oneshot_mods(MOD_BIT(KC_LSFT)); break;
        case TD_SINGLE_HOLD_INTERRUPTED:
        case TD_SINGLE_HOLD: register_code(KC_LSFT); break;
        case TD_DOUBLE_TAP_INTERRUPTED:
        case TD_DOUBLE_TAP: set_oneshot_mods(MOD_BIT(KC_LCTL)); break;
        case TD_DOUBLE_HOLD_INTERRUPTED:
        case TD_DOUBLE_HOLD: register_code(KC_LCTL); break;
        case TD_TRIPLE_TAP_INTERRUPTED:
        case TD_TRIPLE_TAP: set_oneshot_mods(MOD_BIT(KC_LSFT)|MOD_BIT(KC_LCTL)); break;
        case TD_TRIPLE_HOLD_INTERRUPTED:
        case TD_TRIPLE_HOLD: register_code(KC_LSFT); register_code(KC_LCTL); break;
        default:
            break;
    }
}

void shift_control_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (shift_control_state.state) {
        case TD_SINGLE_HOLD_INTERRUPTED:
        case TD_SINGLE_HOLD: unregister_code(KC_LSFT); break;
        case TD_DOUBLE_HOLD_INTERRUPTED:
        case TD_DOUBLE_HOLD: unregister_code(KC_LCTL); break;
        case TD_TRIPLE_HOLD_INTERRUPTED:
        case TD_TRIPLE_HOLD: unregister_code(KC_LSFT); unregister_code(KC_LCTL); break;
        default:
            break;
    }
    shift_control_state.state = TD_NONE;
}

// Tap dance registration
qk_tap_dance_action_t tap_dance_actions[] = {
    [SHIFT_ALT] = ACTION_TAP_DANCE_FN_ADVANCED(reset_on_tap_4, shift_alt_finished, shift_alt_reset),
    [CONTROL_SUPER] = ACTION_TAP_DANCE_FN_ADVANCED(reset_on_tap_4, control_super_finished, control_super_reset),
    [SHIFT_CONTROL] = ACTION_TAP_DANCE_FN_ADVANCED(reset_on_tap_4, shift_control_finished, shift_control_reset)
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
     /*
      * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
      * │ Q │ W │ E │ R │ T │       │ Y │ U │ I │ O │ P │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │ A │ S │ D │ F │ G │       │ H │ J │ K │ L │ ; │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │ Z │ X │ C │ V │ B │       │ N │ M │ , │ . │ / │
      * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
      *           ┌───┐                   ┌───┐
      *           │DEL├───┐           ┌───┤ESC│
      *           └───┤SPC├───┐   ┌───┤BSP├───┘
      *               └───│LOW│   │RSE├───┘
      *                   └───┘   └───┘
      */


    // [_QWERTY] = LAYOUT_split_3x5_3(
    //     KC_Q,    KC_W,              KC_E,            KC_R,            KC_T,               KC_Y,         KC_U,              KC_I,              KC_O,           KC_P,
    //     KC_A,    RALT_T(KC_S),      LCTL_T(KC_D),    LSFT_T(KC_F),    KC_G,               KC_H,         LSFT_T(KC_J),      LCTL_T(KC_K),      RALT_T(KC_L),   KC_SCLN,
    //     KC_Z,    KC_X,              KC_C,            KC_V,            KC_B,               KC_N,         KC_M,              KC_COMM,           KC_DOT,         KC_SLSH,
    //                                 KC_DEL,          KC_SPC,          MO(1),              MO(2),        KC_BSPC,           KC_ESC
    // ),

    // [_LOWER] = LAYOUT_split_3x5_3(
    //    KC_1,     KC_2,              KC_3,            KC_4,            KC_5,               KC_6,         KC_7,             KC_8,            KC_9,              KC_0,
    //    CK_RST,  RALT_T(XXXXXXX),   LCTL_T(XXXXXXX), LSFT_T(XXXXXXX), XXXXXXX,           KC_MINS,      LSFT_T(KC_EQL),   LCTL_T(KC_GRV),  RALT_T(KC_QUOT),    KC_BSLS,
    //    CK_TOGG,  CK_UP,           CK_DOWN,         LCTL(KC_C),      LCTL(KC_V),         KC_TAB,      XXXXXXX,          XXXXXXX,         XXXXXXX,           XXXXXXX,
    //                                 KC_TRNS,         KC_TRNS,         KC_TAB,             KC_ENT,       KC_TRNS,          KC_TRNS
    // ),

    // [_RAISE] = LAYOUT_split_3x5_3(
    //     KC_ESC,   KC_UP,           XXXXXXX,         XXXXXXX,         CK_TOGG,             KC_NUM_LOCK,  KC_KP_7,          KC_KP_8,         KC_KP_9,           KC_KP_MINUS,
    //     KC_LEFT,  RALT_T(KC_DOWN), LCTL_T(KC_RGHT), LSFT_T(XXXXXXX), KC_LGUI,             XXXXXXX,      LSFT_T(KC_KP_4),  LCTL_T(KC_KP_5), RALT_T(KC_KP_6),   KC_KP_PLUS,
    //     KC_MPRV,  KC_MSTP,         KC_MPLY,         KC_MNXT,         KC_PSCR,             XXXXXXX,      KC_KP_1,          KC_KP_2,         KC_KP_3,           KC_KP_0,
    //                                KC_TRNS,         KC_TRNS,         KC_TAB,              KC_ENT,       KC_TRNS,          KC_TRNS
    // ),

    [0] = LAYOUT_split_3x5_3(
        KC_Q, KC_W, KC_E, KC_R, KC_T,
        KC_Y, KC_U, KC_I, KC_O,KC_BSPC,

        KC_A, KC_S, KC_D,  KC_F, KC_G,
        KC_H,KC_J,KC_K,KC_L,KC_P,

        KC_Z, KC_X, KC_C, KC_V, KC_B,
        KC_N,KC_M,KC_COMM,KC_DOT,KC_SLASH,

        KC_SPACE, OSL(2), TD(CONTROL_SUPER),
        RESET, TD(SHIFT_ALT), OSL(1)
    ),

    [1] = LAYOUT_split_3x5_3(
        KC_ESCAPE, KC_PLUS, KC_EQUAL, KC_MINS, KC_ASTERISK,
        KC_UNDS,KC_DOUBLE_QUOTE,KC_QUOTE,KC_GRAVE,KC_BSPC,

        KC_TAB, KC_LBRC, KC_LCBR, KC_LPRN, KC_LT,
        KC_GT,KC_RPRN,KC_RCBR,KC_RBRC,KC_ENTER,

        KC_P1, KC_P2, KC_P3, KC_P4, KC_P5,    
        KC_P6,KC_P7,KC_P8,KC_P9,KC_P0,

        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
    ),


    [2] = LAYOUT_split_3x5_3(
        KC_ESC, KC_NO, KC_NUM, KC_TILD,  KC_F11,   
        KC_F12,KC_CIRC,KC_PERCENT,KC_DEL,KC_BSPC,

        KC_PIPE, KC_AMPERSAND, KC_QUESTION, KC_EXLM, KC_HASH,                        
        KC_BACKSLASH,KC_COLN,KC_SCLN,KC_AT,KC_DOLLAR,

        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5,
        KC_F6,KC_F7,KC_F8,KC_F9,KC_F10,

        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
    ),

    // [3] = LAYOUT_split_3x5_3(
    //     KC_ESC, KC_INS, KC_UP, KC_PAUS, KC_ASTG,
    //     KC_BSPC, KC_F24, KC_F23, KC_F22, KC_F21, 

    //     KC_PSCR, KC_LEFT, KC_DOWN, KC_RIGHT, KC_CAPS,
    //     KC_ENTER, KC_F20, KC_F19, KC_F18, KC_F17, 

    //     KC_DEL, KC_HELP, KC_EXEC, KC_FIND, KC_NUM,
    //     KC_NO, KC_F16, KC_F15, KC_F14, KC_F13, 

    //     LLOCK, KC_TRANSPARENT,
    //     RESET, KC_TRANSPARENT
    // )
};

