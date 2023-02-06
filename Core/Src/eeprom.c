#include "eeprom.h"

#define PATTERN_OFFSET      0x00
#define BRIGHTNESS_OFFSET   PATTERN_OFFSET + sizeof(uint8_t)
#define ANIMATION_OFFSET    BRIGHTNESS_OFFSET + sizeof(uint8_t)

void eeprom_store_brightness_level(uint8_t brightness) {
  i2c_mem_write(BRIGHTNESS_OFFSET, &brightness, sizeof(uint8_t));
}

uint8_t eeprom_read_brightness_level(void) {
  uint8_t brightness;
  i2c_mem_read(BRIGHTNESS_OFFSET, &brightness, sizeof(uint8_t));
  return brightness;
}

void eeprom_store_current_pattern(uint8_t pattern) {
  i2c_mem_write(PATTERN_OFFSET, &pattern, sizeof(uint8_t));
}

uint8_t eeprom_read_current_pattern(void) {
  uint8_t current_pattern;
  i2c_mem_read(PATTERN_OFFSET, &current_pattern, sizeof(uint8_t));
  return current_pattern;
}

void eeprom_store_animation_status(uint8_t animation_status) {
  i2c_mem_write(ANIMATION_OFFSET, &animation_status, sizeof(uint8_t));
}

uint8_t eeprom_read_animation_status(void) {
  uint8_t animation_status;
  i2c_mem_read(ANIMATION_OFFSET, &animation_status, sizeof(uint8_t));
  return animation_status;
}
