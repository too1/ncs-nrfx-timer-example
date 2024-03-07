/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <nrfx_timer.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

// Get a reference to the TIMER1 instance
static const nrfx_timer_t my_timer = NRFX_TIMER_INSTANCE(1);

// Interrupt handler for the timer
// NOTE: This callback is triggered by an interrupt. Many drivers or modules in Zephyr can not be accessed directly from interrupts, 
//		 and if you need to access one of these from the timer callback it is necessary to use something like a k_work item to move execution out of the interrupt context. 
void timer1_event_handler(nrf_timer_event_t event_type, void * p_context)
{
	static int counter = 0;
	switch(event_type) {
		case NRF_TIMER_EVENT_COMPARE0:
			// Do your work here
			printk("Timer 1 callback. Counter = %d\n", counter++);
			break;
		
		default:
			break;
	}
}

// Function for initializing the TIMER1 peripheral using the nrfx driver
static void timer1_init(void)
{
	nrfx_timer_config_t timer_config = NRFX_TIMER_DEFAULT_CONFIG(1000000);
	timer_config.bit_width = NRF_TIMER_BIT_WIDTH_32;

	int err = nrfx_timer_init(&my_timer, &timer_config, timer1_event_handler);
	if (err != NRFX_SUCCESS) {
		printk("Error initializing timer: %x\n", err);
	}

	IRQ_DIRECT_CONNECT(TIMER1_IRQn, 0, nrfx_timer_1_irq_handler, 0);
	irq_enable(TIMER1_IRQn);
}

// Function for scheduling repeated callbacks from TIMER1
static void timer1_repeated_timer_start(uint32_t timeout_us)
{
	nrfx_timer_enable(&my_timer);

	nrfx_timer_extended_compare(&my_timer, NRF_TIMER_CC_CHANNEL0, timeout_us, 
                                NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
}

int main(void)
{
	int ret;

	if (!device_is_ready(led.port)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	// Initialize TIMER1
	timer1_init();

	// Setup TIMER1 to generate callbacks every second
	timer1_repeated_timer_start(1000000);

	while (1) {
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return 0;
		}
		k_msleep(SLEEP_TIME_MS);
	}

	return 0;
}
