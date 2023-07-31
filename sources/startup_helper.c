#include "startup_helper.h"

uint32_t fw_offset;
uint32_t vectors_ram_addr;

void startup_helper(void)
{
	SCB_CCR |= SCB_CCR_STKALIGN;
	asm volatile("dmb");
	SCB_VTOR = vectors_ram_addr;																	// set ram vectors addrs
}
