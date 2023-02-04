#ifndef RGB_H_
#define RGB_H_

#include "main.h"

#define NUMBER_OF_PATTERN 3

void load_led_pattern(uint8_t map_id);
uint8_t get_led_brightness(void);
void brightness_decrease(void);
void brightness_increase(void);
void update_led_matrix(void);
bool ledbar_animation_is_enabled(void);
void ledbar_animation_enable(void);
void ledbar_animation_disable(void);
void ledbar_animate(void);

void set_gui_lock_led(bool value);
void enable_gui_lock_led(void);
void disable_gui_lock_led(void);
void set_caps_lock_led(bool value);
void enable_caps_lock_led(void);
void disable_caps_lock_led(void);

/* Reproduces actual keyboard layout */

#define PATTERN_leds( \
    K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, K00, K01, K02, K03, K04, \
    K50, K51, K52, K53, K54, K55, K56, K57, K58, K59, K09, K08, K07,      K05, \
    K60, K61, K62, K63, K64, K65, K66, K67, K68, K69, K70, K11, K12, K10, K14, \
    K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K19, K18, K17, K16, K15, \
    K30, K31, K32,           K33,                K34, K35, K36, K37, K38, K39  \
) { \
    K00, K01, K02, K03, K04, K05, XXX, K07, K08, K09, XXX, XXX, \
    K10, K11, K12, XXX, K14, K15, K16, K17, K18, K19, XXX, XXX, \
    K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, XXX, XXX, \
    K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, XXX, XXX, \
    K40, K41, K42, K43, K44, K45, K46, K47, K48, K49, XXX, XXX, \
    K50, K51, K52, K53, K54, K55, K56, K57, K58, K59, XXX, XXX, \
    K60, K61, K62, K63, K64, K65, K66, K67, K68, K69, XXX, XXX, \
    K70, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX  \
}

#define PATTERN_bar_led( \
    K08, \
    K07, \
    K06, \
    K05, \
    K04, \
    K03, \
    K02, \
    K01, \
    K00 \
) { \
    K00, K01, K02, K03, K04, K05, K06, K07, K08 \
}

#endif /* __RGB_H_ */
