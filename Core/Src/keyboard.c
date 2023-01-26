#include "keyboard.h"

uint16_t current_matrix[6];
uint16_t old_matrix[6];

//bool current_matrix[MATRIX_ROWS][MATRIX_COLS];

/* Matrix storage :
  {0xFFFF},
  {0xFFFF},
  {0xFFFF},
  {0xFFFF},
  {0xFFFF},
  {0xFFFF}
*/

uint16_t cols[MATRIX_COLS] = {GPIO_PIN_0,
                              GPIO_PIN_1,
                              GPIO_PIN_2,
                              GPIO_PIN_3,
                              GPIO_PIN_4,
                              GPIO_PIN_5,
                              GPIO_PIN_6,
                              GPIO_PIN_7,
                              GPIO_PIN_8,
                              GPIO_PIN_9,
                              GPIO_PIN_10,
                              GPIO_PIN_11,
                              GPIO_PIN_12,
                              GPIO_PIN_13
                            };

uint16_t rows[MATRIX_ROWS] = {GPIO_PIN_0,
                              GPIO_PIN_1,
                              GPIO_PIN_2,
                              GPIO_PIN_3,
                              GPIO_PIN_4,
                              GPIO_PIN_5
                            };


const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_96_iso(
        KC_ESC,   KC_1,     KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,      KC_0,     KC_MINS,   KC_EQL,
        KC_TAB,   KC_Q,     KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,      KC_P,     KC_LBRC,   KC_RBRC,
        KC_CAPS,  KC_A,     KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,      KC_SCLN,  KC_QUOT,   KC_NONUS_HASH,  KC_ENT,
        KC_LSFT,  KC_BSLS,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,   KC_DOT,   KC_SLSH,                   KC_RSFT,
        KC_LCTL,  KC_LWIN,  KC_LALT,                   KC_SPC,                             KC_RALT,   KC_NO,               KC_RCTL,        KC_BSPC,
        KC_INS,   KC_DEL,            KC_PGUP, KC_PGDN,                                                KC_DOWN,  KC_RGHT,   KC_LEFT,        KC_UP
    )
  };

// uint8_t isPowerOfTwo(uint16_t n) {
//     return n && (!(n & (n - 1)));
// }


uint8_t get_bit_position(uint16_t n) {
  // if (!isPowerOfTwo(n))
  //     return -1;
  uint8_t i = 1, pos = 0;

  // Iterate through bits of n till we find a set bit
  // i&n will be non-zero only when 'i' and 'n' have a set bit
  // at same position
  while (!(i & n)) {
      // Unset current bit and set the next bit in 'i'
      i = i << 1;
      // increment position
      ++pos;
  }
  return pos;
}

void scan_matrix(void) {
  /* Reset current matrix before a new read */
  memset(current_matrix, 0x00, sizeof(current_matrix));

  /* Read current matrix */
  for(int i = 0; i < MATRIX_ROWS; i++) {
    /* Turn on required row */
    GPIOA->ODR = 1<<i;

    /* Wait before read (Needs to be fine tuned later) */
    HAL_Delay(1);

    /* Store each GPIO Read in current matrix */
    current_matrix[i] = (uint16_t)GPIOC->IDR&0x3FFF;
  }

  printf("%04x\r\n", current_matrix[0]);
  printf("%04x\r\n", current_matrix[1]);
  printf("%04x\r\n", current_matrix[2]);
  printf("%04x\r\n", current_matrix[3]);
  printf("%04x\r\n", current_matrix[4]);
  printf("%04x\r\n", current_matrix[5]);
}

void process_matrix(void) {
  uint16_t key;

  for(int i = 0; i < MATRIX_ROWS; i++) {
      if(memcmp(old_matrix, current_matrix, sizeof(current_matrix) != 0) ) {
        uint16_t diff_row = old_matrix[i] ^ current_matrix[i];
        for(int j = 0; j < MATRIX_COLS; j++) {
          //TODO - Differentiel ou non ?
          //https://github.com/tmk/tmk_keyboard/blob/master/tmk_core/common/keyboard.c
        }
        // key = keymaps[0][i][pos];
        // printf("[asus]PKey (%d): %d/%04x\r\n", i, pos, key);
      }
  }

  memcpy(old_matrix, current_matrix, sizeof(current_matrix));

}

void print_keymap(void) {

  for(int i = 0; i < MATRIX_ROWS; i++) {
    for(int j = 0; j < MATRIX_COLS; j++) {
      printf("%04x ", keymaps[0][i][j]);
    }
    printf("\r\n");
  }

}




// const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
//     [0] = LAYOUT_96_iso(
//         KC_ESC,   KC_1,     KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,      KC_0,     KC_MINS,   KC_EQL,
//         KC_TAB,   KC_Q,     KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,      KC_P,     KC_LBRC,   KC_RBRC,
//         KC_CAPS,  KC_A,     KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,      KC_SCLN,  KC_QUOT,   KC_NONUS_HASH,  KC_ENT,
//         KC_LSFT,  KC_BSLS,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,   KC_DOT,   KC_SLSH,                   KC_RSFT,
//         KC_LCTL,  KC_LWIN,  KC_LALT,                   KC_SPC,                             KC_RALT,   MO(2),               KC_RCTL,        KC_BSPC,
//         KC_INS,   KC_DEL,            KC_PGUP, KC_PGDN,                                                KC_DOWN,  KC_RGHT,   KC_LEFT,        KC_UP
//     ),
//     [1] = LAYOUT_96_iso(
//         KC_GRV,   KC_F1,    KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,     KC_F10,   KC_F11,    KC_F12,
//         KC_TAB,   KC_Q,     KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,      KC_P,     KC_LBRC,   KC_RBRC,
//         KC_CAPS,  KC_A,     KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,      KC_SCLN,  KC_QUOT,   KC_NONUS_HASH,  KC_ENT,
//         KC_LSFT,  KC_BSLS,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,   KC_DOT,   KC_SLSH,                   KC_RSFT,
//         KC_LCTL,  KC_LWIN,  KC_LALT,                   KC_SPC,                             KC_RALT,   MO(3),               KC_RCTL,        KC_BSPC,
//         KC_INS,   KC_DEL,            KC_PGUP, KC_PGDN,                                                KC_DOWN,  KC_RGHT,   KC_LEFT,        KC_UP
//     ),
//     [2] = LAYOUT_96_iso(
//         KC_GRV,   KC_F1,    KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,     KC_F10,   KC_F11,    KC_F12,
//         KC_NO,    KC_MPLY,  KC_MSTP, KC_MPRV, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, KC_NO,   KC_NO,     KC_PSCR,  KC_NO,     KC_NO,
//         KC_NO,    KC_F13,   KC_F14,  KC_F15,  KC_F16,  KC_F17,  KC_NO,   KC_NO,   KC_NO,   KC_NO,     KC_NO,    KC_NO,     KC_NO,          KC_NO,
//         KC_NO,    KC_NO,    KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,     KC_NO,    KC_NO,                     KC_NO,
//         KC_NO,    GUI_TOG,  KC_NO,                     KC_NO,                              KC_NO,     KC_NO,               KC_MS_RIGHT,    TG(1),
//         KC_NO,    KC_NO,             KC_NO,   KC_NO,                                                  KC_NO,    KC_NO,     KC_NO,          KC_NO
//     ),
//     [3] = LAYOUT_96_iso(
//         KC_GRV,   KC_F1,    KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,     KC_F10,   KC_F11,    KC_F12,
//         KC_NO,    KC_MPLY,  KC_MSTP, KC_MPRV, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, KC_NO,   KC_NO,     KC_PSCR,  KC_NO,     KC_NO,
//         KC_NO,    KC_F13,   KC_F14,  KC_F15,  KC_F16,  KC_F17,  KC_NO,   KC_NO,   KC_NO,   KC_NO,     KC_NO,    KC_NO,     KC_NO,          KC_NO,
//         KC_NO,    KC_NO,    KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,     KC_NO,    KC_NO,                     KC_NO,
//         KC_NO,    GUI_TOG,  KC_NO,                     KC_NO,                              KC_NO,     KC_NO,               KC_MS_RIGHT,    TG(0),
//         KC_NO,    KC_NO,             KC_NO,   KC_NO,                                                  KC_NO,    KC_NO,     KC_NO,          KC_NO
//     )
// };
