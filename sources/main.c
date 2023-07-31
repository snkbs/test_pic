#include <stdint.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencmsis/core_cm3.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/dma.h>

#include "startup_helper.h"
#include "st.h"

static volatile uint32_t test = 1;

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
		if (test % 2 == 0)
		{
			gpio_toggle(GPIOB, GPIO1);
		}
		test++;
		delay_ms(500);
	}
}


