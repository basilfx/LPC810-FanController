/*
 *  DS18D20.c
 *
 *  Author: Kestutis Bivainis
 *
 *  Original source code from
 *  http://kazus.ru/forums/showthread.php?t=100566
 */

#include "DS18x20.h"

/**
 *
 */
uint8_t DS18x20_Start_Conversion_by_ROM(uint8_t (*serial)[DS18x20_SERIAL_NUM_SIZE])
{

    uint8_t cnt;

    /* Reset bus */
    cnt = One_Wire_Reset();

    if (cnt != ONE_WIRE_SUCCESS) {
        return cnt;
    }

    One_Wire_Write_Byte(ONE_WIRE_MATCH_ROM);

    for(cnt = 0; cnt != 8; cnt++) {
        One_Wire_Write_Byte((*serial)[cnt]);
    }

    One_Wire_Write_Byte(DS18x20_CONVERT_T_CMD);

    return ONE_WIRE_SUCCESS;
}

/**
 *
 */
uint8_t DS18x20_Get_Conversion_Result_by_ROM_CRC(uint8_t (*serial)[DS18x20_SERIAL_NUM_SIZE], int32_t* temperature)
{
    uint8_t cnt;
    uint8_t pad[DS18x20_STRATCHPAD_SIZE];

    /* Reset bus */
    cnt = One_Wire_Reset();

    if (cnt != ONE_WIRE_SUCCESS) {
        return cnt;
    }

    One_Wire_Write_Byte(ONE_WIRE_MATCH_ROM);

    for(cnt = 0; cnt != 8; cnt++) {
        One_Wire_Write_Byte((*serial)[cnt]);
    }
    One_Wire_Write_Byte(DS18x20_READ_STRATCHPAD_CMD);

    for(cnt = 0; cnt != DS18x20_STRATCHPAD_SIZE; cnt++) {
        pad[cnt] = One_Wire_Read_Byte();
    }

    /* Verify CRC checksum */
    if (Crc8Dallas(DS18x20_STRATCHPAD_SIZE, pad)) {
        return ONE_WIRE_CRC_ERROR;
    }

    *temperature = DS18x20_Convert_Temperature(pad, serial);

    return ONE_WIRE_SUCCESS;
}

/**
 *
 */
uint8_t DS18x20_Search_Rom_One_Device(uint8_t* serial[DS18x20_SERIAL_NUM_SIZE])
{
    uint8_t cnt_bits;
    uint8_t cnt_bytes;
    uint8_t tmp;
    uint8_t tmp2 = 0;
    uint8_t dev_cnt = 0;

    /* Reset bus */
    tmp = One_Wire_Reset();

    if (tmp != ONE_WIRE_SUCCESS) {
        return tmp;
    }

    One_Wire_Write_Byte(ONE_WIRE_SEARCH_ROM);

    for (cnt_bytes = 0; cnt_bytes != 8; cnt_bytes++) {
        for (cnt_bits = 0; cnt_bits != 8; cnt_bits++)  {
            tmp = One_Wire_Read_Bit();

            if (One_Wire_Read_Bit() == tmp) {
                dev_cnt++;
            }

            One_Wire_Write_Bit(tmp);

            if (tmp) {
                tmp2 |= (1 << cnt_bits);
            }
        }

        *serial[cnt_bytes]=tmp2;
        tmp2=0;
    }

    /* Verify CRC checksum */
    if (Crc8Dallas(DS18x20_SERIAL_NUM_SIZE, *serial)) {
        return ONE_WIRE_CRC_ERROR;
    }

    return ONE_WIRE_SUCCESS;
}

/**
 *
 */
uint8_t DS18x20_Search_Rom(uint8_t *devices_found, uint8_t (*serial)[ONE_WIRE_DEVICE_NUMBER_MAX][DS18x20_SERIAL_NUM_SIZE])
{
  unsigned long path, next, pos;
  uint8_t bit, chk;
  uint8_t cnt_bit, cnt_byte, cnt_num,tmp;

  path = 0;
  cnt_num = 0;

  do {
    /* Reset bus */
    tmp = One_Wire_Reset();

    if (tmp != ONE_WIRE_SUCCESS) {
       return tmp;
    }

    One_Wire_Write_Byte(ONE_WIRE_SEARCH_ROM);
    next = 0;
    pos = 1;

    for (cnt_byte = 0; cnt_byte != 8; cnt_byte++)  {
        (*serial)[cnt_num][cnt_byte] = 0;

        for (cnt_bit = 0; cnt_bit != 8; cnt_bit++) {
            bit = One_Wire_Read_Bit();
            chk = One_Wire_Read_Bit();

            if (!bit && !chk) {
                if (pos & path) {
                    bit = 1;
                } else {
                    next = (path & (pos - 1)) | pos;
                }

                pos <<= 1;
            }

            One_Wire_Write_Bit(bit);

            if (bit != 0) {
                (*serial)[cnt_num][cnt_byte] |= (1 << cnt_bit);
            }
        }
    }

    path = next;
    cnt_num++;
  } while(path);

  /* Write number of devices */
  *devices_found = cnt_num;

  return ONE_WIRE_SUCCESS;
}

/**
 *
 */
uint8_t DS18x20_Start_Conversion_Skip_Rom(void)
{
    uint8_t tmp;

    /* Reset bus */
    tmp = One_Wire_Reset();

    if (tmp != ONE_WIRE_SUCCESS) {
       return tmp;
    }

    One_Wire_Write_Byte(ONE_WIRE_SKIP_ROM);
    One_Wire_Write_Byte(DS18x20_CONVERT_T_CMD);

    if (One_Wire_Read_Byte()) {
        return ONE_WIRE_DEVICE_BUSY;
    }

    return ONE_WIRE_SUCCESS;
}

/**
 *
 */
uint8_t DS18x20_Read_Temp_NoCRC_Skip_Rom(uint8_t (*serial)[DS18x20_SERIAL_NUM_SIZE], int32_t* temperature)
{
    uint8_t tmp, cnt;
    uint8_t pad[DS18x20_STRATCHPAD_SIZE];

    /* Reset bus */
    tmp = One_Wire_Reset();

    if (tmp != ONE_WIRE_SUCCESS) {
       return tmp;
    }

    One_Wire_Write_Byte(ONE_WIRE_SKIP_ROM);
    One_Wire_Write_Byte(DS18x20_READ_STRATCHPAD_CMD);

    for (cnt = 0; cnt != DS18x20_STRATCHPAD_SIZE; cnt++) {
        pad[cnt] = One_Wire_Read_Byte();
    }

    *temperature = DS18x20_Convert_Temperature(pad, serial);

    return ONE_WIRE_SUCCESS;
}

/**
 * Convert raw temperature reading into a Celsius temperature, depending on the
 * sensor model.
 */
int32_t DS18x20_Convert_Temperature(uint8_t pad[DS18x20_STRATCHPAD_SIZE], uint8_t (*serial)[DS18x20_SERIAL_NUM_SIZE])
{
    uint8_t typeS = 0;

    /* Determine sensor model based on serial */
    switch (*serial[0]) {
        case DS18x20_MODEL_DS18S20:
            typeS = 1;
            break;
        case DS18x20_MODEL_DS18B20:
            typeS = 0;
            break;
        case DS18x20_MODEL_DS1822:
            typeS = 0;
            break;
        default:
            return 0;
    }

    /* Convert to Celcius */
    int16_t raw = (pad[1] << 8) | pad[0];

    if (typeS) {
        raw = raw << 3; // 9 bit resolution default

        if (pad[7] == 0x10) {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - pad[6];
        }
    } else {
        uint8_t config = (pad[4] & 0x60);

        if (config == 0x00) {
            raw = raw & ~7;
        } else if (config == 0x20) {
            raw = raw & ~3;
        } else if (config == 0x40) {
            raw = raw & ~1;
        }
    }

    return (raw * 1000) / 16;
}