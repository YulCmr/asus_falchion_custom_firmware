#include "M24M01RP.h"

extern I2C_HandleTypeDef *hi2c1;
/**
  * @brief  Read an amount of data in blocking mode from a specific memory address
  * @param[in] mem_addr : internal memory address
  * @param[out] data : pointer to data buffer
  * @param[in] size : amount of data to be read
  * @retval HAL status
  */
uint32_t i2c_mem_read(uint32_t mem_addr, uint8_t *data, uint16_t size)
{
    HAL_StatusTypeDef status = HAL_ERROR;
    uint8_t eeprom_addr = EEPROM_ADDR_RD | ((mem_addr & 0x00010000) >> 15); //add A16 to the device select command
    uint16_t tmp_addr = mem_addr & 0x0000FFFF;

    /* Check if the EEPROM is ready for read operation */
    if(HAL_I2C_IsDeviceReady(&hi2c1, eeprom_addr, 300, 1000) != HAL_OK) return EEPROM_TIMEOUT;

    //Start reading process
     if(HAL_I2C_Mem_Read(&hi2c1 , (uint16_t)eeprom_addr, tmp_addr, I2C_MEMADD_SIZE_16BIT, data, size, 1000) != HAL_OK) {
       return EEPROM_FAIL;
     }


    /* If all operations OK, return EEPROM_OK (0) */
    return EEPROM_OK;
}

/**
  * @brief  Write an amount of data in blocking mode
  * @param[in] mem_addr : internal memory address
  * @param[in] data : pointer to data buffer
  * @param[in] size : amount of data to be written
  * @retval HAL status
  */
uint32_t i2c_mem_write(uint32_t mem_addr, uint8_t *data, uint16_t size)
{
    HAL_StatusTypeDef status = HAL_ERROR;
    uint8_t eeprom_addr = EEPROM_ADDR_WR | ((mem_addr & 0x00010000) >> 15); //add A16 to the device select command
    uint16_t tmp_addr = mem_addr & 0x0000FFFF;

    //Check if the EEPROM is ready for a new operation
    if(HAL_I2C_IsDeviceReady(&hi2c1, eeprom_addr, 300, 1000) != HAL_OK) {
      return EEPROM_TIMEOUT;
    }

    //Start writting process
    if(HAL_I2C_Mem_Write(&hi2c1 , (uint16_t)eeprom_addr, tmp_addr, I2C_MEMADD_SIZE_16BIT, data, size, 1000) != HAL_OK) {
      return EEPROM_FAIL;
    }

    return EEPROM_OK;
}
