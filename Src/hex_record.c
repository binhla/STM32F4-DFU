#include "hex_record.h"
#include "debug.h"

bool is_hex_char(uint8_t xChar) {
	if ('0' <= xChar && xChar <= '9') return true;
	if ('a' <= xChar && xChar <= 'f') return true;
	if ('A' <= xChar && xChar <= 'F') return true;
	return false;
}

uint8_t hex_digit(uint8_t xChar) {
	if ('0' <= xChar && xChar <= '9') return xChar - '0';
	if ('a' <= xChar && xChar <= 'f') return xChar - 'a' + 0x0A;
	if ('A' <= xChar && xChar <= 'F') return xChar - 'A' + 0x0A;
	return 0;
}

uint8_t char_digit(uint8_t xHex) {
	if (xHex < 0x0A) return xHex + '0';
	else return xHex - 0x0A + 'A';
}

uint8_t record_get_u8(const uint8_t *pBuffer) {
	uint8_t u8Data = hex_digit(pBuffer[0]);
	u8Data <<= 4;
	u8Data |= hex_digit(pBuffer[1]);
	return u8Data;
}
uint8_t record_get_u16(const uint8_t *pBuffer) {
	uint16_t u16Data = 0;
	for (int i=0; i<4; ++i) {
		u16Data <<= 4;
		u16Data |= hex_digit(pBuffer[0]);
	}
	return u16Data;
}

uint16_t record_set_u8(uint8_t *pBuffer, uint8_t u8Data) {
	pBuffer[0] = char_digit(u8Data >> 4);
	pBuffer[1] = char_digit(u8Data & 0x0F);
	return 2;
}

uint16_t record_set_u16(uint8_t *pBuffer, uint16_t u16Data) {
	for (int i=0; i<4; ++i) {
		pBuffer[3-i] = char_digit(u16Data & 0x000F);
		u16Data >>= 4;
	}
	return 4;
}

bool record_check(const uint8_t *pBuffer, uint16_t length, sRecord_t *pRecord) {
	if (pBuffer[0] == RECORD_SOF) {
		pBuffer++;
		length--;
	}
	if (length < RECORD_MINIMUM_LENGTH-1) /*minus 1 because the last NULL character was not in buffer*/
	{
		LREP_ERROR(__func__, "record length too short %d", length);
		return false;
	}
	
	for (int i=0; i< 8; ++i) {
		if (!is_hex_char(pBuffer[i])) {
			LREP_ERROR(__func__, "hex char error header");
			return false;
		}
	}
	
	pRecord->data_length = record_get_u8(pBuffer);
	pRecord->address = record_get_u16(pBuffer + 2);
	pRecord->type = record_get_u8(pBuffer + 6);
	
	/*the value that should be the record length*/
	uint16_t record_length = (uint16_t )10 + (uint16_t)2*pRecord->data_length;
	if (length < record_length) {
		LREP_ERROR(__func__, "error data length %d", record_length);
		return false;
	}
	
	for (int i=8; i<record_length; ++i) {
		if (!is_hex_char(pBuffer[i])) {
			LREP_ERROR(__func__, "hex char error data or crc");
			return false;
		}
	}
	
	const uint8_t *pDataBuffer = pBuffer + 8;
	for (int i = 0; i < pRecord->data_length; ++i) {
		pRecord->data_buff[i] = record_get_u8(pDataBuffer + i*2);
	}
	
	pRecord->crc = record_get_u8(pDataBuffer + pRecord->data_length*2);
	
	uint8_t cal_crc = record_cal_crc(pRecord);
	if (pRecord->crc != cal_crc) {
		LREP_ERROR(__func__, "crc error 0x%02X 0x%02X", pRecord->crc, cal_crc);
		return false;
	}
	
	/*debug*/
	LREP_INFO(__func__, "record type %d", pRecord->type);
	LREP_INFO(__func__, "record address %d", pRecord->address);
	LREP_INFO(__func__, "record data length %d", pRecord->data_length);
	for (int i=0; i < pRecord->data_length; ++i) {
		app_log_write("%02X ", pRecord->data_buff[0]);
	} app_log_write("\r\n");
	
	return true;
}

uint8_t record_cal_crc(const sRecord_t *pRecord) {	
	uint8_t u8CRC = 0;
	u8CRC += pRecord->data_length;
	u8CRC += (pRecord->address >> 8);
	u8CRC += (pRecord->address & 0x00FF);
	u8CRC += pRecord->type;
	for (int i =0; i< pRecord->data_length; ++i)
		u8CRC += pRecord->data_buff[i];
	u8CRC = !u8CRC;
	u8CRC += 1;
	return u8CRC;
}


