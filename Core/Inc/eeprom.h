#ifndef EEPROM_H_
#define EEPROM_H_

#include "main.h"

void eeprom_store_brightness_level(uint8_t brightness);
uint8_t eeprom_read_brightness_level(void);
void eeprom_store_current_pattern(uint8_t pattern);
uint8_t eeprom_read_current_pattern(void);
void eeprom_store_animation_status(uint8_t animation_status);
uint8_t eeprom_read_animation_status(void);

#endif /* EEPROM_H__ */
