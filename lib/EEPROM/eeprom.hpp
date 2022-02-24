/**
  ******************************************************************************
  * @file           : eeprom.hpp
  * @brief          : Header for the EEPROM
  ******************************************************************************
  * @attention
  *
  *
  * Date Last Updated: 24/02/2022
  * ***************************************************************************
  */

#ifndef EEPROM_HPP
#define EEPROM_HPP

/* Defines -------------------------------------------------------------------*/

#define MAX_EEPROM_TIMEOUT 100

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "stm32l4xx_hal.h"
#include "math.h"

/* Class Structure -----------------------------------------------------------*/

class EEPROM 
{
public:

    uint8_t * pData; 
    HAL_StatusTypeDef status;

private:

    static const uint32_t _timeout = 1000;               // Timeout in ms
    static const uint16_t _devAddress = (0x50 << 1);    // Device address

    // Define the Page Size and number of pages
    static const uint8_t _pageSize = 64;    // in Bytes
    static const uint16_t _pageNum = 512;    // Number of pages

    I2C_HandleTypeDef * _hi2c;
    static const uint16_t _dataSize = 2;

public:
    
    EEPROM(I2C_HandleTypeDef * hi2c);
    ~EEPROM();
    
    /** 
     * @brief write the data to the EEPROM
     * @param page is the number of the start page. Range from 0 to _pageNum-1
     * @param offset is the start byte offset in the page. Range from 0 to _pageSize-1
     * @param data is the pointer to the data to write in bytes
     * @param size is the size of the data
     */
    HAL_StatusTypeDef write(uint16_t page, uint16_t offset, uint8_t *data, uint16_t size);
    
    /** 
     * @brief READ the data from the EEPROM
     * @param page is the number of the start page. Range from 0 to _pageNum-1
     * @param offset is the start byte offset in the page. Range from 0 to _pageSize-1
     * @param data is the pointer to the data to write in bytes
     * @param size is the size of the data
    */
    HAL_StatusTypeDef read(uint16_t page, uint16_t offset, uint8_t *data, uint16_t size);

    /** 
     * @brief Erase a page in the EEPROM Memory
     * @page is the number of page to erase
     * In order to erase multiple pages, just use this function in the for loop
     */
    void page_erase (uint16_t page);

private:

    /** 
     * @brief calculate the number of bytes to be written
     * @param size is the size of the data
     * @param offset is the start byte offset in the page. Range from 0 to _pageSize-1
     */
    uint16_t bytes_to_write(uint16_t size, uint16_t offset);
};

#endif // EEPROM_HPP