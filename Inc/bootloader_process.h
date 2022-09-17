#ifndef __BOOTLOADER_PROCESS_H 
#define __BOOTLOADER_PROCESS_H

#include "config.h"
#include "intel_hex_mod_format.h"

//#define		BOOT_SIZE				0x4000
//#define CONFIG_START_ADDRESS 0x08004000
//#define CONFIG_SIZE 0x800
#define APP_START_ADDRESS 0x08008000
#define JUMP_TO_APP(addr) {__set_MSP(*(__IO uint32_t*) addr); ((pFunction) *(__IO uint32_t*) (addr + 4))();}

typedef  void (*pFunction)(void);

void boot_jump(uint32_t *addr);

void boot_process_line(const uint8_t *pBuffer, uint16_t length);

#endif
