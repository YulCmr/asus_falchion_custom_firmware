#include "rgb.h"

#define NUMBER_OF_PATTERNS  3
#define WHITE 0
#define MIAMI   1
#define RUST  2
#define MIAMI_FNLK NUMBER_OF_PATTERNS

#define NUMBER_OF_BRIGHTNESS_SETTINGS 11

static uint8_t brightness_levels[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
static uint8_t current_brightness = NUMBER_OF_BRIGHTNESS_SETTINGS-1;

//LAYOUT_96_iso(
//     KC_ESC,   KC_1,     KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,      KC_0,     KC_MINS,   KC_EQL,
//     KC_TAB,   KC_Q,     KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,      KC_P,     KC_LBRC,   KC_RBRC,
//     KC_CAPS,  KC_A,     KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,      KC_SCLN,  KC_QUOT,   KC_NONUS_HASH,  KC_ENT,
//     KC_LSFT,  KC_BSLS,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,   KC_DOT,   KC_SLSH,                   KC_RSFT,
//     KC_LCTL,  KC_LWIN,  KC_LALT,                   KC_SPC,                             KC_RALT,   QK_MOMENTARY,        KC_RCTL,        KC_BSPC,
//     KC_INS,   KC_DEL,            KC_PGUP, KC_PGDN,                                                KC_DOWN,  KC_RGHT,   KC_LEFT,        KC_UP
// )

/* Edit Wisely ... */
const uint32_t ledmaps[][DRIVER_LED_TOTAL] = {
    /* Base led layouts */
    [WHITE] = PATTERN_leds(
        0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
        0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,           0xFFFFFF,
        0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
        0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
        0xFFFFFF, 0xFFFFFF, 0xFFFFFF,                     0xFFFFFF,                               0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF
    ),
    [MIAMI] = PATTERN_leds(
      0xFF024E, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0xFF024E, 0xFF024E,
      0xFF024E, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF,           0xFF024E,
      0xFF024E, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0xFF024E, 0xFF024E,
      0xFF024E, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0xFF024E, 0xFF024E, 0xFF024E,
      0xFF024E, 0xFF024E, 0xFF024E,                     0xFF024E,                               0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E
    ),
    [RUST] = PATTERN_leds(
      0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0xFF024E, 0x000000,
      0xFF024E, 0xFF024E, 0x024EFF, 0xFF024E, 0xFF024E, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,           0x000000,
      0x000000, 0x024EFF, 0x024EFF, 0x024EFF, 0xFF024E, 0xFF024E, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0xFF024E, 0x000000,
      0xFF024E, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
      0xFF024E, 0x000000, 0xFF024E,                     0xFF024E,                               0x000000, 0xFF024E, 0x000000, 0x000000, 0x000000, 0x000000
    ),
  /* FN-LOCK Led layouts, duplicate if no change. Customise base pattern otherwise */
    [NUMBER_OF_PATTERNS+WHITE] = PATTERN_leds(
        0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
        0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,           0xFFFFFF,
        0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
        0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
        0xFFFFFF, 0xFFFFFF, 0xFFFFFF,                     0xFFFFFF,                               0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF
    ),
    [NUMBER_OF_PATTERNS+MIAMI] = PATTERN_leds(
        0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E,
        0xFF024E, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF,           0xFF024E,
        0xFF024E, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0xFF024E, 0xFF024E,
        0xFF024E, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0x024EFF, 0xFF024E, 0xFF024E, 0xFF024E,
        0xFF024E, 0xFF024E, 0xFF024E,                     0xFF024E,                               0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E
    ),
    [NUMBER_OF_PATTERNS+RUST] = PATTERN_leds(
        0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0xFF024E, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0xFF024E, 0x000000,
        0xFF024E, 0xFF024E, 0x024EFF, 0xFF024E, 0xFF024E, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,           0x000000,
        0x000000, 0x024EFF, 0x024EFF, 0x024EFF, 0xFF024E, 0xFF024E, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0xFF024E, 0x000000,
        0xFF024E, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
        0xFF024E, 0x000000, 0xFF024E,                     0xFF024E,                               0x000000, 0xFF024E, 0x000000, 0x000000, 0x000000, 0x000000
    )
};

const bool fn_ledmaps[][DRIVER_LED_TOTAL] = {
    [0] = PATTERN_leds(
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,
        0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0,    0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        0, 0, 0,       0,          0, 0, 0, 1, 1, 1
    ),
    [1] = PATTERN_leds(
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0,    0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        0, 0, 0,       0,          0, 0, 0, 1, 1, 1
    )
};

const bool macro_ledmaps[][DRIVER_LED_TOTAL] = {
    [0] = PATTERN_leds(
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0,
        0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0,       0,          0, 0, 0, 0, 0, 0
    ),
    [1] = PATTERN_leds(
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0,
        0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0,       0,          0, 0, 0, 0, 0, 0
    )
};

void update_led_matrix(void) {
  uint8_t current_used_led_pattern = get_led_pattern();
  uint8_t fn_lock_offset;

  if(current_base_layer() == FNLK_LAYER) {
    fn_lock_offset = 3;
  }
  else fn_lock_offset = 0;

  for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
      /* If FN Key is pressed down, we will process different color for them
      See "fn_ledmaps" to check which keys are concerned with this color shift */
      if( (macro_layer_is_enabled() == true && (macro_ledmaps[current_base_layer()][i] == true) && function_layer_is_enabled() == true) ) {
        /* Controls color of macro keys while FN+MACRO is held down */
        switch(current_used_led_pattern) {
          case WHITE:
              IS31FL3737_set_color(i, 0,
                      (uint8_t)0xFF,
                      (uint8_t)0x00,
                      (uint8_t)0x00);
              break;
          case RUST:
              IS31FL3737_set_color(i, 0,
                      (uint8_t)0x00,
                      (uint8_t)0xFF,
                      (uint8_t)0xFF);
              break;
          /* Default uses complementary color based on current pattern. Currently buggy depending of brightness. Don't know why ... */
          default:
              IS31FL3737_set_color(i, 1,
                      (uint8_t)(ledmaps[current_used_led_pattern+fn_lock_offset][i]>>16),
                      (uint8_t)((ledmaps[current_used_led_pattern+fn_lock_offset][i]>>8)&0xFF),
                      (uint8_t)(ledmaps[current_used_led_pattern+fn_lock_offset][i]&0xFF) );
              break;
        }
      }
      else if( (function_layer_is_enabled() == true && (fn_ledmaps[current_base_layer()][i] == true) && macro_layer_is_enabled() == false) ) {
        /* Controls color of function keys while FN is held down */
        switch(current_used_led_pattern) {
          case WHITE:
              IS31FL3737_set_color(i, 0,
                      (uint8_t)0xFF,
                      (uint8_t)0x00,
                      (uint8_t)0x00);
              break;
          case RUST:
              IS31FL3737_set_color(i, 0,
                      (uint8_t)0x00,
                      (uint8_t)0xFF,
                      (uint8_t)0xFF);
              break;
          /* Default uses complementary color based on current pattern. Currently buggy depending of brightness. Don't know why ... */
          default:
              IS31FL3737_set_color(i, 1,
                      (uint8_t)(ledmaps[current_used_led_pattern+fn_lock_offset][i]>>16),
                      (uint8_t)((ledmaps[current_used_led_pattern+fn_lock_offset][i]>>8)&0xFF),
                      (uint8_t)(ledmaps[current_used_led_pattern+fn_lock_offset][i]&0xFF) );
              break;
        }
      }
      else
        IS31FL3737_set_color(i, 0,
                (uint8_t)(ledmaps[current_used_led_pattern+fn_lock_offset][i]>>16),
                (uint8_t)((ledmaps[current_used_led_pattern+fn_lock_offset][i]>>8)&0xFF),
                (uint8_t)(ledmaps[current_used_led_pattern+fn_lock_offset][i]&0xFF) );
  }
}

uint8_t get_led_brightness(void) {
  return brightness_levels[current_brightness];
}

void load_led_pattern(uint8_t current_used_led_pattern) {
  if(current_used_led_pattern > NUMBER_OF_PATTERNS) current_used_led_pattern = 0;
  /* In case of overflow by 0 */
  else if(current_used_led_pattern == 255) current_used_led_pattern = NUMBER_OF_PATTERNS;
  set_led_pattern(current_used_led_pattern);

  update_led_matrix();
}

void brightness_decrease(void) {
  if(current_brightness != 0) {
    current_brightness--;
    update_led_matrix();
  }
}

void brightness_increase(void) {
  if(current_brightness < NUMBER_OF_BRIGHTNESS_SETTINGS-1) {
    current_brightness++;
    update_led_matrix();
  }
}

// Led bar = {
//   93, //(+)
//   92,
//   91,
//   90,
//   89,
//   88,
//   87,
//   86,
//   85 //(-)
// }

is31_led g_is31_leds[96] = {
  {0, B_1, A_1, C_1},
  {0, B_2, A_2, C_2},
  {0, B_3, A_3, C_3},
  {0, B_4, A_4, C_4},
  {0, B_5, A_5, C_5},
  {0, B_6, A_6, C_6},
  {0, B_7, A_7, C_7},
  {0, B_8, A_8, C_8},
  {0, B_9, A_9, C_9},
  {0, B_10, A_10, C_10},
  {0, B_11, A_11, C_11},
  {0, B_12, A_12, C_12},

  {0, E_1, D_1, F_1},
  {0, E_2, D_2, F_2},
  {0, E_3, D_3, F_3},
  {0, E_4, D_4, F_4},
  {0, E_5, D_5, F_5},
  {0, E_6, D_6, F_6},
  {0, E_7, D_7, F_7},
  {0, E_8, D_8, F_8},
  {0, E_9, D_9, F_9},
  {0, E_10, D_10, F_10},
  {0, E_11, D_11, F_11},
  {0, E_12, D_12, F_12},

  {0, H_1, G_1, I_1},
  {0, H_2, G_2, I_2},
  {0, H_3, G_3, I_3},
  {0, H_4, G_4, I_4},
  {0, H_5, G_5, I_5},
  {0, H_6, G_6, I_6},
  {0, H_7, G_7, I_7},
  {0, H_8, G_8, I_8},
  {0, H_9, G_9, I_9},
  {0, H_10, G_10, I_10},
  {0, H_11, G_11, I_11},
  {0, H_12, G_12, I_12},

  {0, K_1, J_1, L_1},
  {0, K_2, J_2, L_2},
  {0, K_3, J_3, L_3},
  {0, K_4, J_4, L_4},
  {0, K_5, J_5, L_5},
  {0, K_6, J_6, L_6},
  {0, K_7, J_7, L_7},
  {0, K_8, J_8, L_8},
  {0, K_9, J_9, L_9},
  {0, K_10, J_10, L_10},
  {0, K_11, J_11, L_11},
  {0, K_12, J_12, L_12},

  {1, B_1, A_1, C_1},
  {1, B_2, A_2, C_2},
  {1, B_3, A_3, C_3},
  {1, B_4, A_4, C_4},
  {1, B_5, A_5, C_5},
  {1, B_6, A_6, C_6},
  {1, B_7, A_7, C_7},
  {1, B_8, A_8, C_8},
  {1, B_9, A_9, C_9},
  {1, B_10, A_10, C_10},
  {1, B_11, A_11, C_11},
  {1, B_12, A_12, C_12},

  {1, E_1, D_1, F_1},
  {1, E_2, D_2, F_2},
  {1, E_3, D_3, F_3},
  {1, E_4, D_4, F_4},
  {1, E_5, D_5, F_5},
  {1, E_6, D_6, F_6},
  {1, E_7, D_7, F_7},
  {1, E_8, D_8, F_8},
  {1, E_9, D_9, F_9},
  {1, E_10, D_10, F_10},
  {1, E_11, D_11, F_11},
  {1, E_12, D_12, F_12},

  {1, H_1, G_1, I_1},
  {1, H_2, G_2, I_2},
  {1, H_3, G_3, I_3},
  {1, H_4, G_4, I_4},
  {1, H_5, G_5, I_5},
  {1, H_6, G_6, I_6},
  {1, H_7, G_7, I_7},
  {1, H_8, G_8, I_8},
  {1, H_9, G_9, I_9},
  {1, H_10, G_10, I_10},
  {1, H_11, G_11, I_11},
  {1, H_12, G_12, I_12},

  {1, K_1, J_1, L_1},
  {1, K_2, J_2, L_2},
  {1, K_3, J_3, L_3},
  {1, K_4, J_4, L_4},
  {1, K_5, J_5, L_5},
  {1, K_6, J_6, L_6},
  {1, K_7, J_7, L_7},
  {1, K_8, J_8, L_8},
  {1, K_9, J_9, L_9},
  {1, K_10, J_10, L_10},
  {1, K_11, J_11, L_11},
  {1, K_12, J_12, L_12}
};
