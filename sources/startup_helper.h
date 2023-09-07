#pragma once
#ifndef __STARTUP_HELPER_H__
#define __STARTUP_HELPER_H__

#include <stdint.h>
#include <libopencmsis/core_cm3.h>


extern uint32_t fw_offset __attribute__((__section__(".noinit")));
extern uint32_t vectors_ram_addr __attribute__((__section__(".noinit")));

void startup_helper(void);

#endif

