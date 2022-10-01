#ifndef __BOOTLOADER_PROCESS_H 
#define __BOOTLOADER_PROCESS_H

#include "config.h"
#include "intel_hex_mod_format.h"
#include <stdbool.h>
#include <stdint.h>

//#define		BOOT_SIZE				0x4000
//#define CONFIG_START_ADDRESS 0x08004000
//#define CONFIG_SIZE 0x800
#define APP_START_ADDRESS 0x08008000
#define JUMP_TO_APP(addr) {__set_MSP(*(__IO uint32_t*) addr); ((pFunction) *(__IO uint32_t*) (addr + 4))();}

typedef  void (*pFunction)(void);

void boot_jump(uint32_t addr);

typedef struct {
	bool 			bBoothooked;
	uint32_t 	u32BaseAddress;
	uint32_t 	u32WriteAddress;
	int 			sector;
} sDFU_Control_t;

#define PROCESS_DEBUG 0

extern sDFU_Control_t g_DFU_Control;

void boot_process_init(void);
void boot_process_line(const uint8_t *pBuffer, uint16_t length);

void boot_process_send_ack(void);
void boot_process_send_nack(void);

int boot_process_get_sector(uint32_t address);
int boot_process_verify_address(uint32_t address);

#endif
