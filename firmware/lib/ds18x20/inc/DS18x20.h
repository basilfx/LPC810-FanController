/*
 *  DS18D20.h
 *
 *  Author: Kestutis Bivainis
 *
 *  Original source code from
 *  http://kazus.ru/forums/showthread.php?t=100566
 */

#ifndef _DS18x20_H
#define _DS18x20_H

#include <stdint.h>

#include "1-Wire.h"
#include "1-WireCRC.h"

#define DS18x20_CONVERT_T_CMD         0x44
#define DS18x20_WRITE_STRATCHPAD_CMD  0x4E
#define DS18x20_READ_STRATCHPAD_CMD   0xBE
#define DS18x20_COPY_STRATCHPAD_CMD   0x48
#define DS18x20_RECALL_E_CMD          0xB8
#define DS18x20_READ_POWER_SUPPLY_CMD 0xB4

#define DS18x20_STRATCHPAD_SIZE       0x09
#define DS18x20_SERIAL_NUM_SIZE       0x08

/* The DS18S20 has a fixed 9-bit accuracy. Others have 9-12 bit. */
#define DS18x20_MODEL_DS18S20         0x10
#define DS18x20_MODEL_DS18B20         0x28
#define DS18x20_MODEL_DS1822          0x20

uint8_t DS18x20_Search_Rom(uint8_t *devices_found, uint8_t (* serial)[ONE_WIRE_DEVICE_NUMBER_MAX][DS18x20_SERIAL_NUM_SIZE]);
uint8_t DS18x20_Search_Rom_One_Device(uint8_t *serial_num[DS18x20_SERIAL_NUM_SIZE]);

uint8_t DS18x20_Start_Conversion_by_ROM(uint8_t (*serial)[DS18x20_SERIAL_NUM_SIZE]);
uint8_t DS18x20_Get_Conversion_Result_by_ROM_CRC(uint8_t (*serial)[DS18x20_SERIAL_NUM_SIZE], int32_t *temperature);

uint8_t DS18x20_Start_Conversion_Skip_Rom(void);
uint8_t DS18x20_Read_Temp_NoCRC_Skip_Rom(uint8_t (*serial)[DS18x20_SERIAL_NUM_SIZE], int32_t* temperature);

int32_t DS18x20_Convert_Temperature(uint8_t pad[DS18x20_STRATCHPAD_SIZE], uint8_t (* serial)[DS18x20_SERIAL_NUM_SIZE]);

#endif
