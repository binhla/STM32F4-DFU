#ifndef __INTEL_HEX_MOD_FORMAT_H
#define __INTEL_HEX_MOD_FORMAT_H

#include <stdio.h>
#include <stdint.h>

/*
 Message format
 :llaaaatt[dd...]cc'#
 :----------------------- the colon start every Intel HEX record
  ll--------------------- the number of data bytes in [dd...]
    aaaa----------------- the starting address for subsequent data in the record
		    tt--------------- HEX record type
				  [dd...]-------- data field
					       cc------ checksum
								   #-- end of record
 */

#define RECORD_SOF		':'
#define RECORD_EOF		'#'

#define RECORD_MINIMUM_LENGTH 12
 /*
  HEX record type
	00 data record
	01 end of file record
	02 extended segment address record
	04 extended linear address record
	05 start linear address record (MDK-ARM only)
	*/
	
typedef enum {
	HEX_RECORD_DATA = 0x00,
	HEX_RECORD_END_OF_FILE = 0x01,
	HEX_RECORD_EXTENDED_SEGMENT_ADDRESS = 0x02,
	HEX_RECORD_EXTENDED_LINEAR_ADDRESS = 0x04,
	HEX_RECORD_START_LINEAR_ADDRESS = 0x05,
	
	HEX_RECORD_BOOT_HOOK = 0x11,
	HEX_RECORD_BOOT_ACK = 0x12,
	HEX_RECORD_BOOT_NACK = 0x13,
	HEX_RECORD_JUMP = 0x14,
	HEX_RECORD_REBOOT = 0x15,
} hex_record_t;


typedef struct {
	uint8_t data_length;
	uint16_t address;
	uint8_t type;
	uint8_t data_buff[256];
	uint8_t crc;
} sRecord_t;

#endif /*__INTEL_HEX_MOD_FORMAT_H*/
