#include <stdio.h>
#include <stdint.h>

#include <bootutil/mcuboot_status.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>

//#define WATCHDOG_WINDOW (CONFIG_MCUBOOT_WATCHDOG_TIMEOUT * 32768 - 1)
#define WATCHDOG_WINDOW (CONFIG_MCUBOOT_WATCHDOG_TIMEOUT * 1000)

void mcuboot_status_change(mcuboot_status_type_t status)
{
	if (status == MCUBOOT_STATUS_STARTUP) {
		/* Setup watchdog and start it */
		const struct device *const wdt = DEVICE_DT_GET(DT_ALIAS(watchdog0));

		if (!device_is_ready(wdt)) {
			printk("%s: device not ready.\n", wdt->name);
			return;
		}

		struct wdt_timeout_cfg wdt_config = {
			.flags = WDT_FLAG_RESET_SOC,
			.window.min = 0,
			.window.max = WATCHDOG_WINDOW,
		};

int wdt_channel_id, err;
		wdt_channel_id = wdt_install_timeout(wdt, &wdt_config);

		err = wdt_setup(wdt, WDT_OPT_PAUSE_HALTED_BY_DBG);
		if (err < 0) {
			printk("Watchdog setup error\n");
		}
	}
}

