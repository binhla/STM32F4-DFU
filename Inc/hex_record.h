#ifndef __HEX_RECORD_H
#define __HEX_RECORD_H

#include <stdbool.h>
#include <stdint.h>
#include "intel_hex_mod_format.h"

bool is_hex_char(uint8_t xChar);
uint8_t hex_digit(uint8_t xChar);
uint8_t char_digit(uint8_t xHex);

uint8_t record_get_u8(const uint8_t *pBuffer);
uint8_t record_get_u16(const uint8_t *pBuffer);

uint16_t record_set_u8(uint8_t *pBuffer, uint8_t u8Data);
uint16_t record_set_u16(uint8_t *pBuffer, uint16_t u16Data);

bool record_check(const uint8_t *pBuffer, uint16_t length, sRecord_t *pRecord);
uint8_t record_cal_crc(const sRecord_t *pRecord);

#endif
