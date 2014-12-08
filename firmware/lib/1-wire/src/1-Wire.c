/*
 *  1-Wire.c
 *
 *  Author: Kestutis Bivainis
 *
 *  Original source code from
 *  http://kazus.ru/forums/showthread.php?t=100566
 */

#include "chip.h"
#include "1-Wire.h"

bool One_Wire_Pin_Read(void)
{
    Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, ONE_WIRE_PORT, ONE_WIRE_PIN);
    return Chip_GPIO_GetPinState(LPC_GPIO_PORT, ONE_WIRE_PORT, ONE_WIRE_PIN);
}

void One_Wire_Pin_Low(void)
{
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, ONE_WIRE_PORT, ONE_WIRE_PIN);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, ONE_WIRE_PORT, ONE_WIRE_PIN, 0);
}

void One_Wire_Pin_High(void)
{
    Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, ONE_WIRE_PORT, ONE_WIRE_PIN);
}

uint8_t One_Wire_Reset(void)
{
    uint8_t tmp;

    if (One_Wire_Pin_Read() == 0) {
       return ONE_WIRE_BUS_LOW_ERROR;
    }

    One_Wire_Pin_Low();
    DWT_Delay(TIME_RESET_LOW);
    One_Wire_Pin_High();
    DWT_Delay(TIME_PULSE_DELAY_HIGH);

    if (One_Wire_Pin_Read()) {
        tmp = ONE_WIRE_ERROR_NO_ECHO;
    } else {
        tmp = ONE_WIRE_SUCCESS;
    }

    One_Wire_Pin_High();
    DWT_Delay(TIME_AFTER_RESET);

    return tmp;
}

void One_Wire_Write_Byte(uint8_t byte)
{
    uint8_t cnt;

    for (cnt = 0; cnt != 8; cnt++) {
        One_Wire_Write_Bit(byte & (1 << cnt));
    }
}

void One_Wire_Write_Bit(uint8_t bit)
{
    One_Wire_Pin_Low();

    if (bit) {
        DWT_Delay(TIME_PULSE_DELAY_LOW);
        One_Wire_Pin_High();
        DWT_Delay(TIME_PULSE_DELAY_HIGH);
    } else {
        DWT_Delay(TIME_PULSE_DELAY_HIGH);
        One_Wire_Pin_High();
        DWT_Delay(TIME_PULSE_DELAY_LOW);
    }
}

uint8_t One_Wire_Read_Byte(void)
{
    uint8_t tmp = 0;
    uint8_t cnt;

    for (cnt = 0; cnt != 8; cnt++) {
        if (One_Wire_Read_Bit()) {
            tmp |= (1 << cnt);
        }
    }

    DWT_Delay(TIME_PULSE_DELAY_HIGH);
    return tmp;
}

uint8_t One_Wire_Read_Bit(void)
{
    uint8_t tmp;

    One_Wire_Pin_Low();
    DWT_Delay(TIME_HOLD_DOWN);
    One_Wire_Pin_High();
    DWT_Delay(TIME_PULSE_DELAY_LOW);

    if (One_Wire_Pin_Read()) {
        tmp = 1;
    } else {
       tmp = 0;
    }

    DWT_Delay(TIME_PULSE_DELAY_HIGH);

    return tmp;
}
