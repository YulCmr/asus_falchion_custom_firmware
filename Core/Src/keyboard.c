#include "keyboard.h"

#define MAX_NUMBER_OF_KEYS    6
#define printf(...)

uint16_t current_matrix[6];
uint16_t old_matrix[6];


/* Matrix storage :
  {0xFFFF},
  {0xFFFF},
  {0xFFFF},
  {0xFFFF},
  {0xFFFF},
  {0xFFFF}
*/
static uint8_t matrix_keys[MAX_NUMBER_OF_KEYS];
static uint8_t matrix_modifiers = 0x00;
static uint8_t matrix_medias = 0x00;
static uint8_t number_of_keys;

// Layout layer control
static uint8_t base_layer;
static uint8_t function_layer;

// Brightness Control
static uint8_t brightness_levels[] = {0, 7, 15, 31, 63, 127, 255};
static uint8_t current_brightness;

// Flags to send HID Report only when needed
static bool media_update_needed = false;
static bool keyboard_update_needed = false;

/* Keyboard layout, see end of file */
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS];

extern USBD_HandleTypeDef hUsbDeviceFS;

static void send_matrix(void);

// HID Keyboard
 struct keyboardHID_t {
     uint8_t id;
     uint8_t modifiers;
     //uint8_t reserved;
     uint8_t key1;
     uint8_t key2;
     uint8_t key3;
     uint8_t key4;
     uint8_t key5;
     uint8_t key6;
 };

struct keyboardHID_t keyboardHID;

 // HID Media
 struct mediaHID_t {
   uint8_t id;
   uint8_t keys;
 };

struct mediaHID_t mediaHID;

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


void process_matrix_event(uint16_t key, bool logic_level) {
  switch(key) {
    /* ------------ Media keys ------------ */
    case 0x00A8:  // Mute
    case 0x00A9:  // Vol Up
    case 0x00AA:  // Vol Down
    case 0x00AB:  // Next Track
    case 0x00AC:  // Prev Track
    case 0x00AD:  // Stop
    case 0x00AE:  // Play/Pause
      if(logic_level) {
        matrix_medias |= 1<<(key-0xA8);
      }
      else {
        matrix_medias &= ~(1 << (key-0xA8));
      }
      media_update_needed = true;
      break;

    /* ------------ Brightness down ------------ */
    case 0x7803:
      if(logic_level) {
        if(current_brightness != 0) {
          IS31FL3737_set_color_all(brightness_levels[--current_brightness],
                                    brightness_levels[current_brightness],
                                    brightness_levels[current_brightness]);
        }
      }
      break;

    /* ------------ Brightness up ------------ */
    case 0x7804:
      if(logic_level) {
        if(current_brightness != 6) {
          IS31FL3737_set_color_all(brightness_levels[++current_brightness],
                                    brightness_levels[current_brightness],
                                    brightness_levels[current_brightness]);
        }
      }
      break;

    /* ------------ FN Key ------------ */
    case 0x5220 :
      if(logic_level) {
        function_layer = 2;
      }
      else {
        function_layer = 0;
      }
      break;

    /* ------------  FN Lock ------------*/
    case 0x5260 :
      if(logic_level) {
        base_layer = !base_layer;
      }
      break;

    /* ------------ Modifiers ------------ */
    case 0xE0 :
    case 0xE1 :
    case 0xE2 :
    case 0xE3 :
    case 0xE4 :
    case 0xE5 :
    case 0xE6 :
    case 0xE7 :
      if(logic_level) {
        matrix_modifiers |= 1<<(key-0xE0);
      }
      else {
        matrix_modifiers &= ~(1 << (key-0xE0));
      }
      keyboard_update_needed = true;
      break;

    /* ------------ Casual keys ------------ */
    default :
        // Simultaneously pressed key processing
        for(int i = 0; i < MAX_NUMBER_OF_KEYS; i++) {
          if(!logic_level) {
            if(matrix_keys[i] == key) {
              matrix_keys[i] = 0x00;
              number_of_keys--;
              keyboard_update_needed = true;
              return;
            }
          }
          else {
            if(matrix_keys[i] == 0x00) {
              matrix_keys[i] = key;
              number_of_keys++;
              keyboard_update_needed = true;
              return;
            }
          }
      }
      break;
  }
}

void scan_matrix(void) {
  uint16_t matrix_change;
  uint16_t col_mask = 1;
  uint16_t key;
  bool logic_level;

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

    /* Check for change in current row */
    matrix_change = current_matrix[i] ^ old_matrix[i];

    /* Process only potential change in current row */
    if(matrix_change) {
      /* Cycle through keys of current row */
      for (uint8_t c = 0; c < MATRIX_COLS; c++, col_mask <<= 1) {
        /* Test if current key has changed since previous scan */
        if (matrix_change & col_mask) {
          /* Process key logic level (Pressed or Released)*/
          if(current_matrix[i] & col_mask) logic_level = 1;
          else logic_level = 0;

          /* Retrieve keycode */
          key = keymaps[base_layer+function_layer][i][c];

          /* Process key event (and store if needed) */
          process_matrix_event(key, logic_level);

          /* Save current matrix state */;
          old_matrix[i] ^= col_mask;
        }
      }
    }
  }
  /* Send row changes (if needed) in USB Report */
  send_matrix();
}

void send_matrix(void) {

  /* Copy previously processed and stored keys */
  keyboardHID.id = 1;
  keyboardHID.modifiers = matrix_modifiers;
  keyboardHID.key1 = matrix_keys[0];
  keyboardHID.key2 = matrix_keys[1];
  keyboardHID.key3 = matrix_keys[2];
  keyboardHID.key4 = matrix_keys[3];
  keyboardHID.key5 = matrix_keys[4];
  keyboardHID.key6 = matrix_keys[5];

  mediaHID.id = 2;
  mediaHID.keys = matrix_medias;

  /* If needed, send keys on USB */
  if(keyboard_update_needed == true) {
    USBD_HID_SendReport(&hUsbDeviceFS, &keyboardHID, sizeof(keyboardHID));
    keyboard_update_needed = false;
  }
  if(media_update_needed == true) {
    USBD_HID_SendReport(&hUsbDeviceFS, &mediaHID, sizeof(mediaHID));
    media_update_needed = false;
  }
}

/* Edit Wisely ... */
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_96_iso(
        KC_ESC,   KC_1,     KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,      KC_0,     KC_MINS,   KC_EQL,
        KC_TAB,   KC_Q,     KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,      KC_P,     KC_LBRC,   KC_RBRC,
        KC_CAPS,  KC_A,     KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,      KC_SCLN,  KC_QUOT,   KC_NONUS_HASH,  KC_ENT,
        KC_LSFT,  KC_BSLS,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,   KC_DOT,   KC_SLSH,                   KC_RSFT,
        KC_LCTL,  KC_LWIN,  KC_LALT,                   KC_SPC,                             KC_RALT,   QK_MOMENTARY,        KC_RCTL,        KC_BSPC,
        KC_INS,   KC_DEL,            KC_PGUP, KC_PGDN,                                                KC_DOWN,  KC_RGHT,   KC_LEFT,        KC_UP
    ),
    [1] = LAYOUT_96_iso(
        KC_GRV,   KC_F1,    KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,     KC_F10,   KC_F11,    KC_F12,
        KC_TAB,   KC_Q,     KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,      KC_P,     KC_LBRC,   KC_RBRC,
        KC_CAPS,  KC_A,     KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,      KC_SCLN,  KC_QUOT,   KC_NONUS_HASH,  KC_ENT,
        KC_LSFT,  KC_BSLS,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,   KC_DOT,   KC_SLSH,                   KC_RSFT,
        KC_LCTL,  KC_LWIN,  KC_LALT,                   KC_SPC,                             KC_RALT,   QK_MOMENTARY,        KC_RCTL,        KC_BSPC,
        KC_INS,   KC_DEL,            KC_PGUP, KC_PGDN,                                                KC_DOWN,  KC_RGHT,   KC_LEFT,        KC_UP
    ),
    [2] = LAYOUT_96_iso(
        KC_GRV,   KC_F1,    KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,     KC_F10,   KC_F11,    KC_F12,
        KC_NO,    KC_MPLY,  KC_MSTP, KC_MPRV, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, KC_NO,   KC_NO,     KC_PSCR,  KC_NO,     KC_NO,
        KC_NO,    KC_F13,   KC_F14,  KC_F15,  KC_F16,  KC_F17,  KC_NO,   KC_NO,   KC_NO,   KC_NO,     KC_NO,    KC_NO,     KC_NO,          KC_NO,
        KC_NO,    KC_NO,    KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,     KC_NO,    KC_NO,                     KC_NO,
        KC_NO,    GUI_TOG,  KC_NO,                     KC_NO,                              KC_NO,     QK_MOMENTARY,        KC_MS_RIGHT,    KC_BSPC,
        QK_TOGGLE_LAYER,    KC_NO,   KC_NO,   KC_NO,                                                  BL_DOWN,  RGB_MOD,   RGB_RMOD,       BL_UP
    ),
    [3] = LAYOUT_96_iso(
        KC_GRV,   KC_F1,    KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,     KC_F10,   KC_F11,    KC_F12,
        KC_NO,    KC_MPLY,  KC_MSTP, KC_MPRV, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, KC_NO,   KC_NO,     KC_PSCR,  KC_NO,     KC_NO,
        KC_NO,    KC_F13,   KC_F14,  KC_F15,  KC_F16,  KC_F17,  KC_NO,   KC_NO,   KC_NO,   KC_NO,     KC_NO,    KC_NO,     KC_NO,          KC_NO,
        KC_NO,    KC_NO,    KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,     KC_NO,    KC_NO,                     KC_NO,
        KC_NO,    GUI_TOG,  KC_NO,                     KC_NO,                              KC_NO,     QK_MOMENTARY,        KC_MS_RIGHT,    KC_BSPC,
        QK_TOGGLE_LAYER,    KC_NO,   KC_NO,   KC_NO,                                                  BL_DOWN,  RGB_MOD,   RGB_RMOD,       BL_UP
    )
};
