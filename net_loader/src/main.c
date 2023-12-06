/*
 * Copyright (C) 2023 Jamie M.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <bootutil/bootutil_public.h>
#include <app_version.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/retention/bootmode.h>
#include <zephyr/dfu/mcuboot.h>
#include <zephyr/mgmt/mcumgr/grp/img_mgmt/img_mgmt.h>
#include <zephyr/mgmt/mcumgr/mgmt/mgmt.h>
#include <zephyr/mgmt/mcumgr/mgmt/callbacks.h>

#ifdef CONFIG_NET_DHCPV4
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_core.h>
#endif

static struct mgmt_callback smp_callback;
static struct mgmt_callback img_mgmt_callback;
static struct mgmt_callback os_mgmt_callback;
static bool received_image_pending = false;

static bool is_image_valid()
{
	struct mcuboot_img_header hdr;
	struct boot_swap_state swap_state;

	if (received_image_pending == false) {
		return false;
	} else if (boot_read_bank_header(FIXED_PARTITION_ID(slot1_partition), &hdr,
					 sizeof(hdr)) != 0) {
		return false;
	} else if (boot_read_swap_state_by_id(FIXED_PARTITION_ID(slot1_partition),
					      &swap_state) != 0) {
		return false;
	}

	return true;
}

static enum mgmt_cb_return smp_callback_function(uint32_t event, enum mgmt_cb_return prev_status,
						 int32_t *rc, uint16_t *group, bool *abort_more,
						 void *data, size_t data_size)
{
	if (event == MGMT_EVT_OP_CMD_RECV) {
		struct mgmt_evt_op_cmd_arg *cmd_data = (struct mgmt_evt_op_cmd_arg *)data;

/* TODO: deal with being unable to check if this is read or write */
		if (cmd_data->group == MGMT_GROUP_ID_IMAGE && cmd_data->id == IMG_MGMT_ID_STATE) {
			/*
			 * Set state command, disallow use of command by returning a group error
			 * code here where the error is actually 0 which, being success, will
			 * skip adding it to the output
			 */
			*rc = IMG_MGMT_ERR_OK;
			*group = MGMT_GROUP_ID_IMAGE;
			return MGMT_CB_ERROR_ERR;
		}
	}

	/* Return OK status code to continue with acceptance to underlying handler */
	return MGMT_CB_OK;
}

static enum mgmt_cb_return img_mgmt_callback_function(uint32_t event,
						      enum mgmt_cb_return prev_status, int32_t *rc,
						      uint16_t *group, bool *abort_more,
						      void *data, size_t data_size)
{
	if (event == MGMT_EVT_OP_IMG_MGMT_DFU_PENDING) {
		received_image_pending = true;
	}

	/* Return OK status code to continue with acceptance to underlying handler */
	return MGMT_CB_OK;
}

static enum mgmt_cb_return os_mgmt_callback_function(uint32_t event,
						     enum mgmt_cb_return prev_status, int32_t *rc,
						     uint16_t *group, bool *abort_more, void *data,
						     size_t data_size)
{
	if (event == MGMT_EVT_OP_OS_MGMT_RESET) {
		/*
		 * Tell bootloader to load firmware loader image, this will actually run the
		 * newly updated image once then reboot back into the net loader application
		 */
		if (is_image_valid() == true) {
			int rc = bootmode_set(BOOT_MODE_TYPE_BOOTLOADER);
		}

/* TODO: check status */
	}

	/* Return OK status code to continue with acceptance to underlying handler */
	return MGMT_CB_OK;
}

int main(void)
{
	const struct device *const wdt = DEVICE_DT_GET(DT_ALIAS(watchdog0));
#ifdef CONFIG_NET_DHCPV4
	struct net_if *default_network_interface = net_if_get_default();
	net_dhcpv4_start(default_network_interface);
#endif

	printk("Network loader firmware version %s\n", APP_VERSION_STRING);

	/* Setup SMP callbacks */
	smp_callback.callback = smp_callback_function;
	smp_callback.event_id = MGMT_EVT_OP_CMD_RECV;
	mgmt_callback_register(&smp_callback);

	/* Setup IMG management callbacks */
	img_mgmt_callback.callback = img_mgmt_callback_function;
	img_mgmt_callback.event_id = MGMT_EVT_OP_IMG_MGMT_DFU_PENDING;
	mgmt_callback_register(&img_mgmt_callback);

	/* Setup OS management callbacks */
	os_mgmt_callback.callback = os_mgmt_callback_function;
	os_mgmt_callback.event_id = MGMT_EVT_OP_OS_MGMT_RESET;
	mgmt_callback_register(&os_mgmt_callback);

	printk("Ready to receive firmware...");

	/* Feed watchdog when needed, this should be half the timeout time */
	while (1) {
		int rc = wdt_feed(wdt, 0);

		if (rc) {
			printk("Failed to feed watchdog: %d\n", rc);
		}

		k_sleep(K_SECONDS(CONFIG_WATCHDOG_FEED_TIME));
	}

	return 0;
}
