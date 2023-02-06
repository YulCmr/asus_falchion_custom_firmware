#include "main.h"


#ifndef __M24M01RP_H
#define __M24M01RP_H

#define TIMEOUT_COM     7
#define EEPROM_ADDR_WR  0xA0
#define EEPROM_ADDR_RD  0xA1

#define SIZE_PAGE 256
#define NB_PAGES  4096

#define EEPROM_OK                  0
#define EEPROM_FAIL                1
#define EEPROM_TIMEOUT             2

uint32_t i2c_mem_read(uint32_t mem_addr, uint8_t *p_data, uint16_t size);
uint32_t i2c_mem_write(uint32_t mem_addr, uint8_t *p_data, uint16_t size);


#endif /* __M24M01RP_H */
