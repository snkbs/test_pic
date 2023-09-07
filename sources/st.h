#pragma once
#ifndef __ST_H__
#define __ST_H__

#include <stdlib.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/dwt.h>
#include <libopencm3/stm32/rcc.h>

void st_setup(void);
uint32_t st_gettick(void);

uint32_t st_get_ms(void);
uint32_t st_get_us(void);

void delay_ms(uint32_t ms);
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
void delay_us(uint32_t us);
#endif


#endif // __ST_H__

