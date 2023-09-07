#include <stdint.h>
#include <libopencmsis/core_cm3.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "startup_helper.h"
#include "st.h"

static uint32_t cnt = 0;

int main(void)
{
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

#ifdef MAPLEMINI
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO1);					// led
	gpio_clear(GPIOB, GPIO1);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO9);					// usb
	gpio_set(GPIOB, GPIO9);																			// d+ pull-up disable
	for (uint16_t i = 0; i < 0x0400 ; ++i) asm volatile("nop");
	gpio_clear(GPIOB, GPIO9);																		// d+ pull-up enable
	gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO8); 						// button32
	gpio_clear(GPIOB, GPIO8);																		// button32 pull-down
#elif BLUEPILL
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);					// led
	gpio_set(GPIOC, GPIO13);																		// disable
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);					// usb d+ pin as gpio (for usbdisc)
	gpio_clear(GPIOA, GPIO12);																		// pull-down d+
	for (uint16_t i = 0; i < 0x0400 ; ++i) asm volatile("nop");
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_INPUT_ANALOG, GPIO12);					// reset to default
#endif

	st_setup();

	while (1)
	{
#ifdef MAPLEMINI
		gpio_toggle(GPIOB, GPIO1);
#elif BLUEPILL		
		gpio_toggle(GPIOC, GPIO13);
#endif
		if (cnt > 20)
		{		
			delay_ms(1000);
		}
		else
		{
			delay_ms(200);
		}
		cnt++;
	}
}


