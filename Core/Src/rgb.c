#include "rgb.h"

#define NUMBER_OF_PATTERNS  3
#define WHITE 0
#define MIAMI   1
#define RUST  2
#define MIAMI_FNLK NUMBER_OF_PATTERNS

#define NUMBER_OF_BRIGHTNESS_SETTINGS 11

static uint8_t brightness_levels[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
static uint8_t current_brightness = NUMBER_OF_BRIGHTNESS_SETTINGS-1;
static uint8_t animation_accent_color_position = 0; //10 is out of scope
static bool ledbar_animation_flag = true;
static bool led_matrix_update_is_needed = false;

const uint32_t ledmaps[][DRIVER_LED_TOTAL];
const bool fn_ledmaps[][DRIVER_LED_TOTAL];
const bool macro_ledmaps[][DRIVER_LED_TOTAL];
const uint32_t bar_ledmaps[][9];
const uint32_t bar_animation_accent_color[NUMBER_OF_PATTERNS];
static uint32_t bar_animation_circular_buffer[9];
static uint32_t bar_animation_short_buffer[3];

void matrix_enable(void) {
  if(HAL_GPIO_ReadPin(GPIOB, matrix_Pin) != GPIO_PIN_SET) {
    HAL_GPIO_WritePin(GPIOB, matrix_Pin, GPIO_PIN_SET);
  }
}

void matrix_disable(void) {
  if(HAL_GPIO_ReadPin(GPIOB, matrix_Pin) != GPIO_PIN_RESET) {
    HAL_GPIO_WritePin(GPIOB, matrix_Pin, GPIO_PIN_RESET);
  }
}

void matrix_toggle(void) {
  if(HAL_GPIO_ReadPin(GPIOB, matrix_Pin) == GPIO_PIN_RESET) {
    HAL_GPIO_WritePin(GPIOB, matrix_Pin, GPIO_PIN_SET);
  }
  else HAL_GPIO_WritePin(GPIOB, matrix_Pin, GPIO_PIN_RESET);
}

bool ledbar_animation_is_enabled(void) {
  return ledbar_animation_flag;
}

void ledbar_animation_enable(void) {
  ledbar_animation_flag = true;
}

void ledbar_animation_disable(void) {
  ledbar_animation_flag = false;
}

void set_gui_lock_led(bool value) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, value);
}

void enable_gui_lock_led(void) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
}

void disable_gui_lock_led(void) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
}

void set_caps_lock_led(bool value) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, value);
}

void enable_caps_lock_led(void) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}

void disable_caps_lock_led(void) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
}

void ask_for_led_matrix_update(void) {
  led_matrix_update_is_needed = true;
}

/* Updates led matrix buffer. Needs to be called each time you made some edit in leds and you wants it to be reflected now */
void update_led_matrix(void) {
  uint8_t current_used_led_pattern = get_led_pattern();
  uint8_t fn_lock_offset;

  /* If no update is needed, return */
  if(led_matrix_update_is_needed == false) return;

  if(current_base_layer() == FNLK_LAYER) {
    fn_lock_offset = 3;
  }
  else fn_lock_offset = 0;

  for (int i = 0; i < DRIVER_LED_TOTAL-9; i++) {
      /* If FN Key is pressed down, we will process different color for them
      See "fn_ledmaps" to check which keys are concerned with this color shift */

      /* Controls color of macro keys while FN+MACRO is held down */
      if( (macro_layer_is_enabled() == true && (macro_ledmaps[current_base_layer()][i] == true) && function_layer_is_enabled() == true) ) {
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
      /* Controls color of function keys while FN is held down */
      else if( (function_layer_is_enabled() == true && (fn_ledmaps[current_base_layer()][i] == true) && macro_layer_is_enabled() == false) ) {
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
      /* Controls color of every other keys except ledbar */
      else
        IS31FL3737_set_color(i, 0,
                (uint8_t)(ledmaps[current_used_led_pattern+fn_lock_offset][i]>>16),
                (uint8_t)((ledmaps[current_used_led_pattern+fn_lock_offset][i]>>8)&0xFF),
                (uint8_t)(ledmaps[current_used_led_pattern+fn_lock_offset][i]&0xFF) );
  }
  /* Handles ledbar update */
  for (int i = 0; i < 9; i++) {
    if(ledbar_animation_is_enabled()) {
      IS31FL3737_set_color((i+85), 0,
              (uint8_t)(bar_animation_circular_buffer[i]>>16),
              (uint8_t)((bar_animation_circular_buffer[i]>>8)&0xFF),
              (uint8_t)(bar_animation_circular_buffer[i]&0xFF) );
    }
    else {
      IS31FL3737_set_color((i+85), 0,
              (uint8_t)(bar_ledmaps[current_used_led_pattern][i]>>16),
              (uint8_t)((bar_ledmaps[current_used_led_pattern][i]>>8)&0xFF),
              (uint8_t)(bar_ledmaps[current_used_led_pattern][i]&0xFF) );
    }
  }
}

/* Used in IS31FL3737 to handle brightness levels */
uint8_t get_led_brightness(void) {
  return brightness_levels[current_brightness];
}

/* Used to switch from one led pattern to another */
void load_led_pattern(uint8_t current_used_led_pattern) {
  if(current_used_led_pattern > NUMBER_OF_PATTERNS) current_used_led_pattern = 0;
  /* In case of overflow by 0 */
  else if(current_used_led_pattern == 255) current_used_led_pattern = NUMBER_OF_PATTERNS;
  set_led_pattern(current_used_led_pattern);

  ask_for_led_matrix_update();
  //update_led_matrix();
}

void brightness_decrease(void) {
  if(current_brightness != 0) {
    current_brightness--;
    ask_for_led_matrix_update();
    //update_led_matrix();
  }
}

void brightness_increase(void) {
  if(current_brightness < NUMBER_OF_BRIGHTNESS_SETTINGS-1) {
    current_brightness++;
    ask_for_led_matrix_update();
    //update_led_matrix();
  }
}

/* Calculates new "frame" for ledbar animation then asks for a driver update */
void ledbar_animate(void) {
  uint8_t current_used_led_pattern = get_led_pattern();
  static uint32_t start = 0;
  static uint8_t iterations = 0;
  static uint8_t max_iteration_number = 50; //Increasing this value will slow down animation
  static uint16_t timeout = 50;
  static uint16_t r1, g1, b1;
  static uint16_t r2, g2, b2;
  static uint16_t r3, g3, b3;
  static uint16_t r4, g4, b4;
  static int16_t r_diff, g_diff, b_diff;

  /* Initial condition, do not modify */
  if(start == 0) {
    start = HAL_GetTick();
  }

  /* Gets calls every "timeout" milliseconds, decrease value to accelerate animation */
  if((HAL_GetTick() - start) > timeout) {
    for(int i = 0; i < 9; i++) {
      bar_animation_circular_buffer[i] = bar_ledmaps[current_used_led_pattern][i];
    }

    /* Load current cell color */
    r1 = (uint8_t)(bar_ledmaps[current_used_led_pattern][animation_accent_color_position]>>16);
    g1 = (uint8_t)((bar_ledmaps[current_used_led_pattern][animation_accent_color_position]>>8)&0xFF);
    b1 = (uint8_t)(bar_ledmaps[current_used_led_pattern][animation_accent_color_position]&0xFF);

    /* Load accent color */
    r2 = (uint8_t)(bar_animation_accent_color[current_used_led_pattern]>>16);
    g2 = (uint8_t)((bar_animation_accent_color[current_used_led_pattern]>>8)&0xFF);
    b2 = (uint8_t)(bar_animation_accent_color[current_used_led_pattern]&0xFF);

    /* Load tail cell color depending on current position */
    if(animation_accent_color_position == 0) {
      r3 = (uint8_t)(bar_ledmaps[current_used_led_pattern][7]>>16);
      g3 = (uint8_t)((bar_ledmaps[current_used_led_pattern][7]>>8)&0xFF);
      b3 = (uint8_t)(bar_ledmaps[current_used_led_pattern][7]&0xFF);
    }
    else if(animation_accent_color_position == 1) {
      r3 = (uint8_t)(bar_ledmaps[current_used_led_pattern][8]>>16);
      g3 = (uint8_t)((bar_ledmaps[current_used_led_pattern][8]>>8)&0xFF);
      b3 = (uint8_t)(bar_ledmaps[current_used_led_pattern][8]&0xFF);
    }
    else {
      r3 = (uint8_t)(bar_ledmaps[current_used_led_pattern][animation_accent_color_position-2]>>16);
      g3 = (uint8_t)((bar_ledmaps[current_used_led_pattern][animation_accent_color_position-2]>>8)&0xFF);
      b3 = (uint8_t)(bar_ledmaps[current_used_led_pattern][animation_accent_color_position-2]&0xFF);
    }

    /* Caculate color gradients */
    r_diff = (r2 - r1) / max_iteration_number;
    g_diff = (g2 - g1) / max_iteration_number;
    b_diff = (b2 - b1) / max_iteration_number;

    r4 = (r1 + (iterations * r_diff));
    g4 = (g1 + (iterations * g_diff));
    b4 = (b1 + (iterations * b_diff));

    bar_animation_short_buffer[2] = (r4&0xFF)<<16;
    bar_animation_short_buffer[2] += (g4&0xFF)<<8;
    bar_animation_short_buffer[2] += (b4&0xFF);

    bar_animation_short_buffer[1] = bar_animation_accent_color[current_used_led_pattern];

    r_diff = (r3 - r2) / max_iteration_number;
    g_diff = (g3 - g2) / max_iteration_number;
    b_diff = (b3 - b2) / max_iteration_number;

    r4 = (r2 + (iterations * r_diff));
    g4 = (g2 + (iterations * g_diff));
    b4 = (b2 + (iterations * b_diff));

    bar_animation_short_buffer[0] = (r4&0xFF)<<16;
    bar_animation_short_buffer[0] += (g4&0xFF)<<8;
    bar_animation_short_buffer[0] += (b4&0xFF);

    /* copy short buffer in big buffer */
    bar_animation_circular_buffer[animation_accent_color_position] = bar_animation_short_buffer[2];

    if(animation_accent_color_position == 0) {
      bar_animation_circular_buffer[8] = bar_animation_short_buffer[1];
      bar_animation_circular_buffer[7] = bar_animation_short_buffer[0];
    }
    else if(animation_accent_color_position == 1) {
      bar_animation_circular_buffer[0] = bar_animation_short_buffer[1];
      bar_animation_circular_buffer[8] = bar_animation_short_buffer[0];
    }
    else  {
      bar_animation_circular_buffer[animation_accent_color_position-1] = bar_animation_short_buffer[1];
      bar_animation_circular_buffer[animation_accent_color_position-2] = bar_animation_short_buffer[0];
    }

    /* Iterate 50 times to calculate 50 gradient from color A to color B */
    if(iterations == max_iteration_number) {
      iterations = 0;

      /* each time we finished a full gradient, roll gradients position */
      if(animation_accent_color_position == 8) {
        animation_accent_color_position = 0;
      }
      else {
        animation_accent_color_position++;
      }
    }
    else iterations++;

    /* Reset timestamp */
    start = HAL_GetTick();

    ask_for_led_matrix_update();
    //update_led_matrix();
  }
}


// [0] = LAYOUT_falchion_iso(
//     KC_ESC,   KC_1,     KC_2,   KC_3,    KC_4,   KC_5,   KC_6,    KC_7,   KC_8,   KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,
//     KC_TAB,   KC_Q,     KC_W,   KC_E,    KC_R,   KC_T,   KC_Y,    KC_U,   KC_I,   KC_O,     KC_P,     KC_LBRC,  KC_RBRC,            KC_DEL,
//     KC_CAPS,  KC_A,     KC_S,   KC_D,    KC_F,   KC_G,   KC_H,    KC_J,   KC_K,   KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,   KC_PGUP,
//     KC_LSFT,  KC_BSLS,  KC_Z,   KC_X,    KC_C,   KC_V,   KC_B,    KC_N,   KC_M,   KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,  KC_UP,    KC_PGDN,
//     KC_LCTL,  KC_LWIN,  KC_LALT,                 KC_SPC,                          KC_RALT,  KC_FN,    KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT
// ),

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

/* True is key has an alternate FN function. x2 because FNLOCK or not*/
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

/* True is key has an alternate MACRO function. x2 because FNLOCK or not*/
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

/* Color accent you want in ledbar animation one each led pattern */
const uint32_t bar_animation_accent_color[NUMBER_OF_PATTERNS] = {
  0xFF0000,  //Red on  WHITE
  0xFF024E,  //Pink on MIAMI
  0x000000   //Off on RUST
};

/* Base color pattern for ledbar on each main pattern */
const uint32_t bar_ledmaps[][9] = {
    [WHITE] = PATTERN_bar_led(
        0xFFFFFF, // (+)
        0xFFFFFF,
        0xFFFFFF,
        0xFFFFFF,
        0xFFFFFF,
        0xFFFFFF,
        0xFFFFFF,
        0xFFFFFF,
        0xFFFFFF  // (-)
    ),
    [MIAMI] = PATTERN_bar_led(
        0x024EFF,
        0x024EFF,
        0x024EFF,
        0x024EFF,
        0x024EFF,
        0x024EFF,
        0x024EFF,
        0x024EFF,
        0x024EFF
    ),
    [RUST] = PATTERN_bar_led(
        0x000000,
        0x000000,
        0x000000,
        0x000000,
        0x000000,
        0x000000,
        0x000000,
        0x000000,
        0x000000
    )
};

// DO NOT TOUCH. GPIO MAPPING
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
