#
# Copyright (C) 2023 Jamie M.
#
# SPDX-License-Identifier: Apache-2.0
#

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_MCUBOOT_WATCHDOG_SETUP src/watchdog.c)
