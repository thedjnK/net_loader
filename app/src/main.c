#include <stdio.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/retention/bootmode.h>
#include <zephyr/mgmt/mcumgr/mgmt/mgmt.h>
#include <zephyr/mgmt/mcumgr/mgmt/callbacks.h>

#ifdef CONFIG_NET_DHCPV4
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_core.h>
#endif

struct mgmt_callback os_mgmt_callback;

enum mgmt_cb_return os_mgmt_callback_function(uint32_t event, enum mgmt_cb_return prev_status,
					      int32_t *rc, uint16_t *group, bool *abort_more,
					      void *data, size_t data_size)
{
	if (event == MGMT_EVT_OP_OS_MGMT_RESET) {
/* TODO: Check if a valid image has been uploaded */
		/*
		 * Tell bootloader to load firmware loader image, this will actually run the
		 * newly updated image once then reboot back into the net loader application
		 */

		int rc = bootmode_set(BOOT_MODE_TYPE_BOOTLOADER);

/* TODO: check status */
	}

	/* Return OK status code to continue with acceptance to underlying handler */
	return MGMT_CB_OK;
}

int main(void)
{
#ifdef CONFIG_NET_DHCPV4
	struct net_if *default_network_interface = net_if_get_default();
	net_dhcpv4_start(default_network_interface);
#endif

	/* Setup OS management callbacks */
	os_mgmt_callback.callback = os_mgmt_callback_function;
	os_mgmt_callback.event_id = MGMT_EVT_OP_OS_MGMT_RESET;
	mgmt_callback_register(&os_mgmt_callback);

	printk("Ready to receive firmware...");
	return 0;
}
