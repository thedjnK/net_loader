#include <stdio.h>

#ifdef CONFIG_NET_DHCPV4
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_core.h>
#endif

int main(void)
{
#ifdef CONFIG_NET_DHCPV4
	struct net_if *default_network_interface = net_if_get_default();
	net_dhcpv4_start(default_network_interface);
#endif

	printk("Ready to receive firmware...");
	return 0;
}
