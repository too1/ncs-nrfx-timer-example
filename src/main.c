/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <nrfx_timer.h>
#include <device.h>
#include <devicetree.h>
/* new repository added old branch removed and new private one as well*/
/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   250

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
/* additions */

#define OUT_PUT_PIN DT_ALIAS(led3)

#define CUST_IO_PIN DT_ALIAS(gpiocus0)
#define SEC_CUST_IO_PIN DT_ALIAS(gpiocus1)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
// additions
static const struct gpio_dt_spec signal = GPIO_DT_SPEC_GET(OUT_PUT_PIN, gpios);

static const struct gpio_dt_spec signal2 = GPIO_DT_SPEC_GET(CUST_IO_PIN, gpios);

static const struct gpio_dt_spec signalC = GPIO_DT_SPEC_GET(SEC_CUST_IO_PIN, gpios);

// Get a reference to the TIMER1 instance
static const nrfx_timer_t my_timer = NRFX_TIMER_INSTANCE(1);

// Interrupt handler for the timer
// NOTE: This callback is triggered by an interrupt. Many drivers or modules in Zephyr can not be accessed directly from interrupts, 
//		 and if you need to access one of these from the timer callback it is necessary to use something like a k_work item to move execution out of the interrupt context. 
void timer1_event_handler(nrf_timer_event_t event_type, void * p_context)
{
	//static int counter = 0;
	static int sjrZ = 0;
	static int sjr2 = 0;
	static int sjr3 = 0;
	switch(event_type) {
		case NRF_TIMER_EVENT_COMPARE0:
			// Do your work here
			// printk("Timer 1 callback. Counter = %d\n", counter++);
			
			sjrZ = gpio_pin_toggle_dt(&signalC);
			if (sjrZ < 0) {
			return;

			}


			// added break below to shortcut rest out
			//break;
						
			
			sjr2 = gpio_pin_toggle_dt(&signal2);
			if (sjr2 < 0) {
			return;
			}
			
			
			sjr3 = gpio_pin_toggle_dt(&signal);
			if (sjr3 < 0) {
			return;
			}
			


			break;
		
		default:
			break;
	}
}

// Function for initializing the TIMER1 peripheral using the nrfx driver
static void timer1_init(void)
{
	nrfx_timer_config_t timer_config = NRFX_TIMER_DEFAULT_CONFIG;
	timer_config.bit_width = NRF_TIMER_BIT_WIDTH_32;
	timer_config.frequency = NRF_TIMER_FREQ_1MHz;

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

void main(void)
{
	//additions
	int sjr;

	if(!device_is_ready(signal.port)){
		return;
	}
	sjr = gpio_pin_configure_dt(&signal, GPIO_OUTPUT_ACTIVE);
	if (sjr < 0) {
		return;
	}
int sjrb;

	if(!device_is_ready(signal2.port)){
		return;
	}
	sjrb = gpio_pin_configure_dt(&signal2, GPIO_OUTPUT_ACTIVE);
	if (sjrb < 0) {
		return;
	}
int dope;
	if(!device_is_ready(signalC.port)){
		return;
	}
	dope = gpio_pin_configure_dt(&signalC, GPIO_OUTPUT_ACTIVE);
	if(dope < 0) {
		return;
	}


	// end adds


	int ret;

	if (!device_is_ready(led.port)) {
		return;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}

	// Initialize TIMER1
	timer1_init();

	// Setup TIMER1 to generate callbacks every 417 micro seconds
	timer1_repeated_timer_start(415);

	while (1) {
		



		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return;
		}
		k_msleep(SLEEP_TIME_MS);
	}
}
