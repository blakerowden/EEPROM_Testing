/**
  ******************************************************************************
  * @file           : eeprom.cpp
  * @brief          : Contains the functionalities of the EEPROM
  ******************************************************************************
  * @attention
  * Adapted from EEPROM and STM32 by Controllers Tech found at: 
  * https://controllerstech.com/eeprom-and-stm32/
  *
  * Author: TeamArrow
  * Date Last Updated: 24/02/2022
  * ***************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "eeprom.hpp"

/* Public Class Functions ----------------------------------------------------*/

// Constructor
EEPROM::EEPROM(I2C_HandleTypeDef * hi2c) {
    _hi2c = hi2c;
};

// Write the data to the EEPROM
HAL_StatusTypeDef EEPROM::write(uint16_t page, uint16_t offset, uint8_t *data, uint16_t size) {
    
    // Find out the number of bit, where the page addressing starts
	int paddrPosition = log(_pageSize)/log(2);

	// calculate the start page and the end page
	uint16_t startPage = page;
	uint16_t endPage = page + ((size+offset)/_pageSize);

	// number of pages to be written
	uint16_t numofPages = (endPage-startPage) + 1;
	uint16_t pos = 0;

	// write the data
	for (int i = 0; i < numofPages; i++) {

		/* calculate the address of the memory location
		 * Here we add the page address with the byte address
		 */

		uint16_t MemAddress = startPage << paddrPosition | offset;
		uint16_t bytesRemaining = bytes_to_write(size, offset);  // calculate the remaining bytes to be written

        // Attempt to write the data to the EEPROM
        status = HAL_OK;
		status = HAL_I2C_Mem_Write(_hi2c, _devAddress, MemAddress, _dataSize, &data[pos], bytesRemaining,  _timeout);  // write the data to the EEPROM
        if (status != HAL_OK) {
            // Error handling
            return status;
        }

		startPage += 1;     // increment the page, so that a new page address can be selected for further write
		offset = 0;         // since we will be writing to a new page, so offset will be 0
		size = size - bytesRemaining;   // reduce the size of the bytes
		pos += bytesRemaining;          // update the position for the data buffer

		HAL_Delay (5);  // Write cycle delay (5ms)

	}

    return status;
}
    
// READ the data from the EEPROM
HAL_StatusTypeDef EEPROM::read(uint16_t page, uint16_t offset, uint8_t *data, uint16_t size) {
        
    int paddrPosition = log(_pageSize)/log(2);

	uint16_t startPage = page;
	uint16_t endPage = page + ((size + offset)/_pageSize);

	uint16_t numofPages = (endPage - startPage) + 1;
	uint16_t pos = 0;

	for (int i = 0; i < numofPages; i++) {

		uint16_t MemAddress = startPage << paddrPosition | offset;
		uint16_t bytesRemaining = bytes_to_write(size, offset);
        
        // Attempt to read the data from the EEPROM
        status = HAL_OK;
		status = HAL_I2C_Mem_Read(_hi2c, _devAddress, MemAddress, _dataSize, &data[pos], bytesRemaining, _timeout);
		if (status != HAL_OK) {
            // Error handling
            return status;
        }

        startPage += 1;
		offset = 0;
		size = size - bytesRemaining;
		pos += bytesRemaining;
	}

    return status;
}

// Erase a page in the EEPROM Memory
void EEPROM::page_erase (uint16_t page)
{
	// calculate the memory address based on the page number
	int paddrPosition = log(PAGE_SIZE)/log(2);
	uint16_t MemAddress = page<<paddrPosition;

	// create a buffer to store the reset values
	uint8_t data[PAGE_SIZE];
	memset(data, 0xff, PAGE_SIZE);

	// write the data to the EEPROM
	HAL_I2C_Mem_Write(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, data, PAGE_SIZE, 1000);

	HAL_Delay (5);  // write cycle delay 
}


/* Private Class Functions ---------------------------------------------------*/

// Calculate the number of bytes to be written
uint16_t EEPROM::bytes_to_write (uint16_t size, uint16_t offset)
{
	if ((size + offset) < _pageSize) return size;
	else return EEPROM::_pageSize - offset;
}
