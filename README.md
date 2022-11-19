# Network application loader for Zephyr

This project allows a supported zephyr board with a network interface to run an MCUmgr server allowing remote uploading of a firmware image to run. This allows devices to be connected and run directly from ethernet networks.
This relies upon mcuboot running the images directly from the slots, whereby slot0 is for the network bootloader and slot1 is for the user image. The user image cannot be confirmed so that when the board reboots, it will revert back to the network application loader image, and wait for a new upload.

## Requirements

 * Zephyr-supported board, see: https://docs.zephyrproject.org/latest/boards/index.html
 * Zephyr-supported network interface
 * MCUmgr-protocol application for device interaction

## Zephyr version

This code targets Zephyr RTOS main branch, which is currently version 3.2.

## License

This code is released under the Apache 2.0 license
