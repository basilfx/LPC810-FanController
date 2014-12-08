/*
 *  1-WireCRC.h
 *
 *  Author: Kestutis Bivainis
 *
 *  Original source code from
 *  http://kazus.ru/forums/showthread.php?t=100566
 */

#ifndef _1_WIRE_H
#define _1_WIRE_H

#include <stdint.h>
#include <lpc_types.h>

#define ONE_WIRE_PIN  4
#define ONE_WIRE_PORT 0

// in microseconds
#define TIME_PULSE_DELAY_LOW  10
#define TIME_PULSE_DELAY_HIGH 60
#define TIME_RESET_LOW        480
#define TIME_AFTER_RESET      350
#define TIME_HOLD_DOWN        2

#define ONE_WIRE_READ_ROM    0x33
#define ONE_WIRE_SKIP_ROM    0xCC
#define ONE_WIRE_SEARCH_ROM  0xF0
#define ONE_WIRE_MATCH_ROM   0x55

#define ONE_WIRE_SUCCESS       0x00
#define ONE_WIRE_ERROR_NO_ECHO 0x01
#define ONE_WIRE_BUS_LOW_ERROR 0x02
#define ONE_WIRE_DEVICE_BUSY   0x03
#define ONE_WIRE_CRC_ERROR     0x04

/* Max number of devices on the bus */
#define ONE_WIRE_DEVICE_NUMBER_MAX 3

/* Micro seconds delay function */
#define DWT_Delay(x) delay_us(x)

bool One_Wire_Pin_Read(void);
void One_Wire_Pin_Low(void);
void One_Wire_Pin_High(void);

uint8_t One_Wire_Reset(void);

void One_Wire_Write_Bit(uint8_t Bit);
void One_Wire_Write_Byte(uint8_t Byte);
uint8_t One_Wire_Read_Bit(void);
uint8_t One_Wire_Read_Byte(void);

#endif
