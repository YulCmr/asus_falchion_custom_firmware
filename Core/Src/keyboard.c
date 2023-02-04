#include "keyboard.h"

#define MAX_NUMBER_OF_KEYS    6

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
static uint8_t macro_layer;
static bool gui_active = 1;

static uint8_t led_pattern = 0;

// Flags to send HID Report only when needed
static bool media_update_needed = false;
static bool keyboard_update_needed = false;

/* Keyboard layout, see end of file */
const uint16_t keymap[][MATRIX_ROWS][MATRIX_COLS];

extern USBD_HandleTypeDef hUsbDeviceFS;

bool current_base_layer(void) {
  return base_layer;
}

bool function_layer_is_enabled(void) {
  if(function_layer != 0) return 1;
  else return 0;
}

bool macro_layer_is_enabled(void) {
  if(macro_layer != 0) return 1;
  else return 0;
}

uint8_t get_led_pattern(void) {
  return led_pattern;
}

void set_led_pattern(uint8_t pattern) {
  led_pattern = pattern;
}

static void send_matrix(void);

// HID Keyboard
 struct keyboardHID_t {
     uint8_t id;
     uint8_t modifiers;
     uint8_t reserved;
     uint8_t key1;
     uint8_t key2;
     uint8_t key3;
     uint8_t key4;
     uint8_t key5;
     uint8_t key6;
 };

struct keyboardHID_t keyboardHID;

// HID Leds
 struct ledsHID_t {
     uint8_t leds_bitfield;
 };

struct ledsHID_t ledsHID;

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

/* Main processing of pressed and depressed key. This is the place you can
change keyboard behaviour on keypresses. Look for required keycode in keyboard.h
May need some improvement over time to improve readability */
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
    /* RGB animation on/off */
    case 0x7823:
      if(logic_level) {
        if(ledbar_animation_is_enabled()) ledbar_animation_disable();
        else ledbar_animation_enable();
      }
      break;
    /* Macro key (FN+ALT) */
    case 0x7700:
      if(logic_level) {
        macro_layer = 2;
      }
      else {
        macro_layer = 0;
      }
      update_led_matrix();
      break;
    case 0x700B:
    if(logic_level) {
      gui_active = !gui_active;
      set_gui_lock_led(gui_active);
    }
    break;

    /* ------------ Led Pattern left ------------ */
    case 0x7821:
      if(logic_level) {
        if(led_pattern != 0)
        load_led_pattern(--led_pattern);
      }
      break;
    /* ------------ Led Pattern right ------------ */
    case 0x7822:
      if(logic_level) {
        if(led_pattern != NUMBER_OF_PATTERN-1)
        load_led_pattern(++led_pattern);
      }
      break;
    /* ------------ Brightness down ------------ */
    case 0x7803:
      if(logic_level) {
        brightness_decrease();
      }
      break;

    /* ------------ Brightness up ------------ */
    case 0x7804:
      if(logic_level) {
        brightness_increase();
      }
      break;

    /* ------------ FN Key ------------ */
    case 0x5220 :
      if(logic_level) {
        function_layer = 2;
      }
      else {
        function_layer = 0;
        /* if Macro layer was active, disable it */
        if(macro_layer != 0) macro_layer = 0;  //disabled for now, may be useless in definitive
      }
      update_led_matrix();
      break;

    /* ------------  FN Lock ------------*/
    case 0x5260 :
      if(logic_level) {
        base_layer = !base_layer;
        update_led_matrix();
      }
      break;

    /* ------------ Modifiers ------------ */
    case 0xE3 :
    case 0xE7 :
      if(gui_active) {
        if(logic_level) {
          matrix_modifiers |= 1<<(key-0xE0);
        }
        else {
          matrix_modifiers &= ~(1 << (key-0xE0));
        }
        keyboard_update_needed = true;
      }
      break;
    case 0xE0 :
    case 0xE1 :
    case 0xE2 :
    case 0xE4 :
    case 0xE5 :
    case 0xE6 :
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
          key = keymap[base_layer+function_layer+macro_layer][i][c];

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

  /* Read HID Led report (Doesn't actually uses USB on each cycle,
  only retrieves a uint8_t which is updated by host in another callback) */
  USBD_HID_ReceiveReport(&hUsbDeviceFS, &ledsHID, sizeof(ledsHID));
  if(((ledsHID.leds_bitfield>>1)&1) == 0x1) enable_caps_lock_led();
  else disable_caps_lock_led();
}

/* Edit Wisely ... */
const uint16_t keymap[][MATRIX_ROWS][MATRIX_COLS] = {
    // Base & FN LOCK Layouts */
    [0] = LAYOUT_falchion_iso(
        KC_ESC,   KC_1,     KC_2,   KC_3,    KC_4,   KC_5,   KC_6,    KC_7,   KC_8,   KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,
        KC_TAB,   KC_Q,     KC_W,   KC_E,    KC_R,   KC_T,   KC_Y,    KC_U,   KC_I,   KC_O,     KC_P,     KC_LBRC,  KC_RBRC,            KC_DEL,
        KC_CAPS,  KC_A,     KC_S,   KC_D,    KC_F,   KC_G,   KC_H,    KC_J,   KC_K,   KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,   KC_PGUP,
        KC_LSFT,  KC_BSLS,  KC_Z,   KC_X,    KC_C,   KC_V,   KC_B,    KC_N,   KC_M,   KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,  KC_UP,    KC_PGDN,
        KC_LCTL,  KC_LWIN,  KC_LALT,                 KC_SPC,                          KC_RALT,  KC_FN,    KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT
    ),
    [1] = LAYOUT_falchion_iso(
        KC_GRV,   KC_F1,    KC_F2,  KC_F3,   KC_F4,  KC_F5,  KC_F6,   KC_F7,  KC_F8,  KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_BSPC,  KC_INS,
        KC_TAB,   KC_Q,     KC_W,   KC_E,    KC_R,   KC_T,   KC_Y,    KC_U,   KC_I,   KC_O,     KC_P,     KC_LBRC,  KC_RBRC,            KC_DEL,
        KC_CAPS,  KC_A,     KC_S,   KC_D,    KC_F,   KC_G,   KC_H,    KC_J,   KC_K,   KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,   KC_PGUP,
        KC_LSFT,  KC_BSLS,  KC_Z,   KC_X,    KC_C,   KC_V,   KC_B,    KC_N,   KC_M,   KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,  KC_UP,    KC_PGDN,
        KC_LCTL,  KC_LWIN,  KC_LALT,                 KC_SPC,                          KC_RALT,  KC_FN,    KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT
    ),
    /* FN Layouts: Basic & FNLOCK */
    [2] = LAYOUT_falchion_iso(
        KC_GRV,   KC_F1,    KC_F2,  KC_F3,   KC_F4,  KC_F5,  KC_F6,   KC_F7,  KC_F8,  KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_NO,    KC_FNLK,
        KC_NO,    KC_MPLY,  KC_MSTP,KC_MPRV, KC_MNXT,KC_MUTE,KC_VOLD, KC_VOLU,KC_NO,  KC_NO,    KC_PSCR,  KC_NO,    KC_NO,              RGB_HUI,
        KC_NO,    KC_NO,    KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,
        KC_NO,    KC_NO,    KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,    BL_UP,    KC_NO,
        KC_NO,    GUI_TOG,  QK_MACRO,                KC_NO,                           KC_NO,    KC_FN,    KC_NO,    RGB_MOD,  BL_DOWN,  RGB_RMOD
    ),
    [3] = LAYOUT_falchion_iso(
        KC_GRV,   KC_F1,    KC_F2,  KC_F3,   KC_F4,  KC_F5,  KC_F6,   KC_F7,  KC_F8,  KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_NO,    KC_FNLK,
        KC_NO,    KC_MPLY,  KC_MSTP,KC_MPRV, KC_MNXT,KC_MUTE,KC_VOLD, KC_VOLU,KC_NO,  KC_NO,    KC_PSCR,  KC_NO,    KC_NO,              RGB_HUI,
        KC_NO,    KC_NO,    KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,
        KC_NO,    KC_NO,    KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,    BL_UP,    KC_NO,
        KC_NO,    GUI_TOG,  QK_MACRO,                KC_NO,                           KC_NO,    KC_FN,    KC_NO,    RGB_MOD,  BL_DOWN,  RGB_RMOD
    ),
    /* Macro layouts : Basic & FNLOCK */
    [4] = LAYOUT_falchion_iso(
        KC_NO,    KC_NO,    KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,
        KC_NO,    KC_NO,    KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,              KC_NO,
        KC_NO,    KC_F13,   KC_F14, KC_F15,  KC_F16, KC_F17, KC_NO,   KC_NO,  KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,
        KC_NO,    KC_NO,    KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,
        KC_NO,    KC_NO,    QK_MACRO,                KC_NO,                           KC_NO,    KC_FN,    KC_NO,    KC_NO,    KC_NO,    KC_NO
    ),
    [5] = LAYOUT_falchion_iso(
        KC_NO,    KC_NO,    KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,
        KC_NO,    KC_NO,    KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,              KC_NO,
        KC_NO,    KC_F13,   KC_F14, KC_F15,  KC_F16, KC_F17, KC_NO,   KC_NO,  KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,
        KC_NO,    KC_NO,    KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,
        KC_NO,    KC_NO,    QK_MACRO,                KC_NO,                           KC_NO,    KC_FN,    KC_NO,    KC_NO,    KC_NO,    KC_NO
    )
};
