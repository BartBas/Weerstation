/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <string.h>

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 6

#define ADC_GAIN ADC_GAIN_1
#define VREF_MV 3300  /* 5 V reference in millivolts */
#define ADC_RESOLUTION 12




volatile uint32_t Led_Sleep_Value[2] = { 200, 1000 };

const struct device *gpioa = DEVICE_DT_GET(DT_NODELABEL(gpioa));

static const struct device *adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc1));

uint32_t map_adc_mv(uint32_t mv_val) {
	const uint32_t x_min = 0;
	const uint32_t x_max = 3300;   // millivolts range
	const uint32_t y_min = 5;
	const uint32_t y_max = 1000;

	return y_min + ((mv_val - x_min) * (y_max - y_min)) / (x_max - x_min);
}

void blink(uint32_t id)
{

	
	gpio_pin_configure(gpioa, id, GPIO_OUTPUT_INACTIVE);


	while (1) {
		gpio_pin_toggle(gpioa, id);

		k_msleep(Led_Sleep_Value[id]);
	}
}

void readadc(uint32_t id, uint32_t channel)
{
	int16_t sample_buffer;
	struct adc_sequence sequence = {
		.channels = BIT(channel),
		.buffer = &sample_buffer,
		.buffer_size = sizeof(sample_buffer),
		.resolution = 12,
	};

	while (1) {
		if (adc_read(adc_dev, &sequence) == 0) {
			uint32_t mv = (sample_buffer * VREF_MV) / (1 << ADC_RESOLUTION);
			printk("ADC raw: %d -> %d mV\n", sample_buffer, mv);
			Led_Sleep_Value[id] = map_adc_mv(mv);
		}
		k_sleep(K_MSEC(5));
	}
}

void blink0(void)
{
	blink(0);
}

void blink1(void)
{
	blink(1);
}

void readadc0(void)
{
	readadc(0,4);
}

K_THREAD_DEFINE(adc0_id, STACKSIZE, readadc0,
	NULL, NULL, NULL,  /* p1, p2, p3 */
	PRIORITY, 0, 0);
K_THREAD_DEFINE(blink0_id, STACKSIZE, blink0, NULL, NULL, NULL,
		PRIORITY, 0, 0);
K_THREAD_DEFINE(blink1_id, STACKSIZE, blink1, NULL, NULL, NULL,
		PRIORITY, 0, 0);
