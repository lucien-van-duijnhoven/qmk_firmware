#include QMK_KEYBOARD_H
#include "../features/custom_shift_keys.h"
#include "../features/layer_lock.h"

enum {
    SHIFT_ALT,
    CONTROL_SUPER,
    TD_DOCS,
};

enum custom_keycodes {
    LLOCK = SAFE_RANGE,
    RESET,
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
    {KC_ASTG, KC_ASOFF}, // Shift Auto shift toggle is off 
};

uint8_t NUM_CUSTOM_SHIFT_KEYS =
    sizeof(custom_shift_keys) / sizeof(custom_shift_key_t);

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    if (!process_layer_lock(keycode, record, LLOCK)) { return false; }
    if (!process_custom_shift_keys(keycode, record)) { return false; } 

    switch (keycode) {
        case RESET:
            if (record->event.pressed) {   
                clear_oneshot_mods();
                reset_oneshot_layer();
                layer_lock_all_off();
                layer_move(0);
            }
            break;
    } 

    #ifdef CONSOLE_ENABLE
        uprintf("KL: kc: 0x%04X, col: %u, row: %u, pressed: %b, time: %u, interrupt: %b, count: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
    #endif 

    return true;
}

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

// Tap dance registration
qk_tap_dance_action_t tap_dance_actions[] = {
    [SHIFT_ALT] = ACTION_TAP_DANCE_FN_ADVANCED(reset_on_tap_4, shift_alt_finished, shift_alt_reset),
    [CONTROL_SUPER] = ACTION_TAP_DANCE_FN_ADVANCED(reset_on_tap_4, control_super_finished, control_super_reset)
};

// Keymap
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
        KC_Q, KC_W, KC_E, KC_R, KC_T,
        KC_BSPC, KC_O, KC_I, KC_U, KC_Y,

        KC_A, KC_S, KC_D,  KC_F, KC_G,
        KC_P, KC_L, KC_K, KC_J, KC_H,

        KC_Z, KC_X, KC_C, KC_V, KC_B,
        KC_SLASH, KC_COMM, KC_DOT, KC_M, KC_N,

        KC_SPACE, TD(CONTROL_SUPER), 
        OSL(1), TD(SHIFT_ALT) 
    ),

    [1] = LAYOUT(
        KC_ESCAPE, KC_ASTERISK, KC_PLUS, KC_EQUAL, KC_PERCENT,                                                                 
        KC_BSPC, KC_GRAVE, KC_QUOTE, KC_DOUBLE_QUOTE, KC_DOLLAR,

        KC_TAB, KC_LBRC, KC_LCBR, KC_LPRN, KC_LT,
        KC_ENTER, KC_RBRC, KC_RCBR, KC_RPRN, KC_GT, 

        KC_PIPE, KC_AMPERSAND, KC_QUESTION, KC_EXLM,  KC_COLN,                        
        KC_AT, KC_UNDS, KC_BSLS, KC_MINS,KC_SCLN,  

        LLOCK, KC_TRANSPARENT,                                                                                   
        OSL(2), KC_TRANSPARENT
    ),


    [2] = LAYOUT(
        KC_ESC, KC_NUM, KC_TILD, KC_BSLS, KC_F11,   
        KC_BSPC, KC_DEL, KC_HASH, KC_CIRC, KC_F12,   

        KC_P1, KC_P2, KC_P3, KC_P4, KC_P5,    
        KC_ENTER, KC_P9, KC_P8, KC_P7, KC_P6,     

        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5,
        KC_F10, KC_F9, KC_F8, KC_F7, KC_F6,  

        LLOCK, KC_TRANSPARENT,             
        OSL(3), KC_TRANSPARENT
    ),

    [3] = LAYOUT(
        KC_ESC, KC_INS, KC_UP, KC_PAUS, KC_ASTG,
        KC_BSPC, KC_F24, KC_F23, KC_F22, KC_F21, 

        KC_PSCR, KC_LEFT, KC_DOWN, KC_RIGHT, KC_CAPS,
        KC_ENTER, KC_F20, KC_F19, KC_F18, KC_F17, 

        KC_DEL, KC_HELP, KC_EXEC, KC_FIND, KC_NUM,
        KC_NO, KC_F16, KC_F15, KC_F14, KC_F13, 

        LLOCK, KC_TRANSPARENT,
        RESET, KC_TRANSPARENT
    )
};