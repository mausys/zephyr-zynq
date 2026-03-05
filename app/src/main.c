/*
 * Copyright (c) 2026 Maurer Systems GmbH
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>

#include <app_version.h>

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

int main(void)
{
	printk("main\n");
	while (1) {
		printk("main\n");
		k_sleep(K_MSEC(10000));
	}

	return 0;
}
