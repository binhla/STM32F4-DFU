#include "bootloader_process.h"
#include "debug.h"
#include "hex_record.h"

#define EnablePrivilegedMode() __asm("SVC #0")

__asm __attribute__( ( noreturn ) ) void BootJumpASM( uint32_t SP, uint32_t RH )
{
  MSR      MSP,r0
  BX       r1
}

void boot_jump(uint32_t *addr) {
	if( CONTROL_nPRIV_Msk & __get_CONTROL( ) ) {  
		/* not in privileged mode */
    EnablePrivilegedMode( ) ;
  }
	
	NVIC->ICER[ 0 ] = 0xFFFFFFFF ;
	NVIC->ICER[ 1 ] = 0xFFFFFFFF ;
	NVIC->ICER[ 2 ] = 0xFFFFFFFF ;
	NVIC->ICER[ 3 ] = 0xFFFFFFFF ;
	NVIC->ICER[ 4 ] = 0xFFFFFFFF ;
	NVIC->ICER[ 5 ] = 0xFFFFFFFF ;
	NVIC->ICER[ 6 ] = 0xFFFFFFFF ;
	NVIC->ICER[ 7 ] = 0xFFFFFFFF ;
	
	NVIC->ICPR[ 0 ] = 0xFFFFFFFF ;
	NVIC->ICPR[ 1 ] = 0xFFFFFFFF ;
	NVIC->ICPR[ 2 ] = 0xFFFFFFFF ;
	NVIC->ICPR[ 3 ] = 0xFFFFFFFF ;
	NVIC->ICPR[ 4 ] = 0xFFFFFFFF ;
	NVIC->ICPR[ 5 ] = 0xFFFFFFFF ;
	NVIC->ICPR[ 6 ] = 0xFFFFFFFF ;
	NVIC->ICPR[ 7 ] = 0xFFFFFFFF ;
	
	SysTick->CTRL = 0 ;
	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk ;
	
	SCB->SHCSR &= ~( SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk ) ;
								 
	if( CONTROL_SPSEL_Msk & __get_CONTROL( ) ) {  /* MSP is not active */
		__set_MSP( __get_PSP( ) ) ;
		__set_CONTROL( __get_CONTROL( ) & ~CONTROL_SPSEL_Msk ) ;
	}
	
	SCB->VTOR = ( uint32_t )addr ;
	
	BootJumpASM( addr[ 0 ], addr[ 1 ] ) ;
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
	if (!record_check(pBuffer, length, &record)) {
		return;
	}
	LREP(__func__, "len %d: %s", length, (char *)pBuffer);
	
	//todo:
	//process with every record type
	switch(record.type) {
		default:
			break;
	}	
}
