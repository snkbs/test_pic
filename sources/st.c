#include "st.h"

static volatile uint32_t st_ms = 0;

void st_setup(void)
{
	// for delay_us()
	// r.m. 31.10
	dwt_enable_cycle_counter();
	
	nvic_set_priority(NVIC_SYSTICK_IRQ, 0);
	systick_set_frequency(1000, rcc_ahb_frequency);				// 1kHz counter	
	systick_interrupt_enable();
	systick_counter_enable();
}

uint32_t st_get_ms(void)
{
	return st_ms;
}

void delay_ms(uint32_t ms)
{
	uint32_t start = st_get_ms();
	while ((st_get_ms() - start) < ms) ;
}

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
void delay_us(uint32_t us)
{	
    uint32_t start = dwt_read_cycle_counter();
    uint32_t delta = us * (rcc_ahb_frequency / 1000000);
	while ((dwt_read_cycle_counter() - start) < delta) ;
}
#endif

void sys_tick_handler(void)
{
	st_ms++;
}