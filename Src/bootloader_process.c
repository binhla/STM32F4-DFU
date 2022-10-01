#include "bootloader_process.h"
#include "debug.h"
#include "hex_record.h"

#define EnablePrivilegedMode() __asm("SVC #0")

__asm __attribute__( ( noreturn ) ) void BootJumpASM( uint32_t SP, uint32_t RH )
{
  MSR      MSP,r0
  BX       r1
}

void boot_jump(uint32_t addr) {
	pFunction JumpToApplication = (pFunction) (*(volatile uint32_t*) (addr + 4));
  __set_MSP(*(uint32_t*) addr);
  JumpToApplication();
}

sDFU_Control_t g_DFU_Control;

void boot_process_init(void) {
	g_DFU_Control.bBoothooked = false;
	g_DFU_Control.u32BaseAddress = 0;
	g_DFU_Control.u32WriteAddress = 0;
}

void boot_process_line(const uint8_t *pBuffer, uint16_t length) {
	static sRecord_t record;
	while (length > 0) {
		if (pBuffer[0] == RECORD_SOF && pBuffer[1] != RECORD_SOF) {
			break;
		}
		length--;
		pBuffer++;
	}
	#if 1
	for (int i=0; i< length; ++i) {
		app_log_write("%c", pBuffer[i]);
	} app_log_write("\r\n");
	#endif
	if (!record_check(pBuffer, length, &record)) {
		boot_process_send_nack();
		return;
	}
	LREP_INFO(__func__, "len %d: %s", length, (char *)pBuffer);
	
	//process with every record type
	switch(record.type) {
		case HEX_RECORD_BOOT_HOOK: {
			LREP_WARNING(__func__, "Boot Hooked received");
			LREP_WARNING(__func__, "Run Device Firmware Update");
			g_DFU_Control.bBoothooked = true;
			boot_process_send_ack();
		} break;
		case HEX_RECORD_REBOOT: {
			LREP_WARNING(__func__, "Reboot received");
			boot_process_send_ack();
			LREP(__func__, "System reset after");
			for (int i=10; i>0; --i) {
				LREP_INFO(__func__, "%d", i);
			}
			NVIC_SystemReset();
		} break;
		case HEX_RECORD_EXTENDED_LINEAR_ADDRESS: {
			LREP_WARNING(__func__, "Record extend linear address");
			if (record.data_length == 2) {
				g_DFU_Control.u32BaseAddress = ((uint32_t)record.data_buff[0] << 24) + ((uint32_t)record.data_buff[1] << 16);
				LREP(__func__, "Base address 0x%08X", g_DFU_Control.u32BaseAddress);
				
				if (HAL_FLASH_Unlock() != HAL_OK) {
					LREP_ERROR(__func__, "Flash unlock error");
					boot_process_send_nack();
				}
				else {
					LREP_INFO(__func__, "Flash Unlock");
					boot_process_send_ack();
				}
			}
			else {
				boot_process_send_nack();
			}
		} break;
		case HEX_RECORD_DATA: {
			g_DFU_Control.u32WriteAddress = g_DFU_Control.u32BaseAddress + record.address;
			LREP_WARNING(__func__, "Record data 0x%08X", g_DFU_Control.u32WriteAddress);
			app_log_write("\t\t");
			for (int i=0; i<record.data_length; ++i) {
				if (i%4 == 0) app_log_write(" ");
				app_log_write("%02X", record.data_buff[i]);
			} app_log_write("\r\n");
			if (boot_process_verify_address(g_DFU_Control.u32WriteAddress) != 0) {
				boot_process_send_nack();
				break;
			}
			bool ret = false;
			uint16_t u16FlashData = 0;
			for (int i=0; i<record.data_length; i+=2) {
				if (boot_process_verify_address(g_DFU_Control.u32WriteAddress + i) != 0) {
					ret = false; break;
				}
				
				u16FlashData = *(__IO uint16_t *)(g_DFU_Control.u32WriteAddress + i);
				if (u16FlashData == *(uint16_t*)(record.data_buff+i)) {
					ret = true; continue;
				}
				
				if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, g_DFU_Control.u32WriteAddress + i, *(uint16_t*)(record.data_buff+i)) != HAL_OK) {
					LREP_ERROR(__func__, "Flash write error @0x%08X", g_DFU_Control.u32WriteAddress + i);
					ret = false; break;
				}
				
				ret = true;
			}
			
			if (ret == false) boot_process_send_nack();
			else boot_process_send_ack();
		} break;
		case HEX_RECORD_START_LINEAR_ADDRESS: {
			LREP_WARNING(__func__, "Start Linear Address: no process here");
			boot_process_send_ack();
		} break;
		case HEX_RECORD_END_OF_FILE: {
			LREP_WARNING(__func__, "End of File");
			if (HAL_FLASH_Lock() != HAL_OK) {
				LREP_ERROR(__func__, "Flash lock error");
				boot_process_send_nack();
			}
			else {
				LREP_INFO(__func__, "Flash Lock");
				boot_process_send_ack();
			}
		} break;
		default:
			break;
	}	
}

void boot_process_send_ack(void) {
	LREP(__func__, "Send ACK");
	const char *str = RECORD_MSG_ACK;
	HAL_UART_Transmit(&UART_DFU, (uint8_t *)str, RECORD_MINIMUM_LENGTH, 0xFFFF);
}

void boot_process_send_nack(void) {
	LREP_ERROR(__func__, "Send NACK");
	const char *str = RECORD_MSG_NACK;
	HAL_UART_Transmit(&UART_DFU, (uint8_t *)str, RECORD_MINIMUM_LENGTH, 0xFFFF);
}

/* Base address of the Flash sectors (for STM32F4 only)*/
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

int boot_process_get_sector(uint32_t address) {
	if (ADDR_FLASH_SECTOR_0 <= address  && address < ADDR_FLASH_SECTOR_1)  return 0;
	if (ADDR_FLASH_SECTOR_1 <= address  && address < ADDR_FLASH_SECTOR_2)  return 1;
	if (ADDR_FLASH_SECTOR_2 <= address  && address < ADDR_FLASH_SECTOR_3)  return 2;
	if (ADDR_FLASH_SECTOR_3 <= address  && address < ADDR_FLASH_SECTOR_4)  return 3;
	if (ADDR_FLASH_SECTOR_4 <= address  && address < ADDR_FLASH_SECTOR_5)  return 4;
	if (ADDR_FLASH_SECTOR_5 <= address  && address < ADDR_FLASH_SECTOR_6)  return 5;
	if (ADDR_FLASH_SECTOR_6 <= address  && address < ADDR_FLASH_SECTOR_7)  return 6;
	if (ADDR_FLASH_SECTOR_7 <= address  && address < ADDR_FLASH_SECTOR_8)  return 7;
	if (ADDR_FLASH_SECTOR_8 <= address  && address < ADDR_FLASH_SECTOR_9)  return 8;
	if (ADDR_FLASH_SECTOR_9 <= address  && address < ADDR_FLASH_SECTOR_10) return 9;
	if (ADDR_FLASH_SECTOR_10 <= address && address < ADDR_FLASH_SECTOR_11) return 10;
	if (ADDR_FLASH_SECTOR_11 <= address && address < 0x08100000) 					 return 11;
	return -1;
}

int boot_process_verify_address(uint32_t address) {
	int sectorIdx = boot_process_get_sector(address);
	if (sectorIdx == -1) {
		LREP_ERROR(__func__, "Not a Flash address");
		return -1;
	}
	
	if (sectorIdx < 2) {
		LREP_ERROR(__func__, "First two sectors (32kB) is for bootloader");
		return -1;
	}
	
	if (sectorIdx == g_DFU_Control.sector) {
		return 0;
	}
	
	FLASH_EraseInitTypeDef EraseInitStruct = {
		.TypeErase = FLASH_TYPEERASE_SECTORS,
		.VoltageRange = FLASH_VOLTAGE_RANGE_3,
		.Sector = sectorIdx,
		.NbSectors = 1,
	};
	uint32_t SectorError = 0;
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
		LREP_ERROR(__func__, "Erase Flash Sector %d Error", sectorIdx);
		return -1;
	}
	LREP_INFO(__func__, "Erase Flash Sector %d Success", sectorIdx);
	g_DFU_Control.sector = sectorIdx;
	return 0;
}


