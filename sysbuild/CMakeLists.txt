#
# Copyright (C) 2023 Jamie M.
#
# SPDX-License-Identifier: Apache-2.0
#

function(${SYSBUILD_CURRENT_MODULE_NAME}_pre_cmake)
  cmake_parse_arguments(INPUT "" "" "IMAGES" ${ARGN})

  math(EXPR WDT_FEED_TIME "${SB_CONFIG_WATCHDOG_TIMEOUT_TIME} / 2")

  foreach(image ${INPUT_IMAGES})
    set_property(TARGET ${image} APPEND_STRING PROPERTY CONFIG "CONFIG_WATCHDOG=y\n")

    if("${image}" STREQUAL "mcuboot")
      set_property(TARGET ${image} APPEND_STRING PROPERTY CONFIG "CONFIG_MCUBOOT_WATCHDOG_SETUP=y\n")
      set_property(TARGET ${image} APPEND_STRING PROPERTY CONFIG "CONFIG_MCUBOOT_WATCHDOG_TIMEOUT=${SB_CONFIG_WATCHDOG_TIMEOUT_TIME}\n")
      set_property(TARGET ${image} APPEND_STRING PROPERTY CONFIG "CONFIG_BUILD_OUTPUT_BIN=n\n")
    else()
      if("${image}" STREQUAL "net_loader")
        set_property(TARGET ${image} APPEND_STRING PROPERTY CONFIG "CONFIG_BUILD_OUTPUT_BIN=n\n")
      else()
        set_property(TARGET ${image} APPEND_STRING PROPERTY CONFIG "CONFIG_BUILD_OUTPUT_HEX=n\n")
      endif()

      set_property(TARGET ${image} APPEND_STRING PROPERTY CONFIG "CONFIG_WATCHDOG_FEED_TIME=${WDT_FEED_TIME}\n")
    endif()
  endforeach()
endfunction()

if(SB_CONFIG_NET_LOADER_IMAGE)
  ExternalZephyrProject_Add(
    APPLICATION net_loader
    SOURCE_DIR ${ZEPHYR_NET_LOADER_MODULE_DIR}/net_loader
  )
  # MCUBoot default configuration is to perform a full chip erase.
  # Placing MCUBoot first in list to ensure it is flashed before other images.
  sysbuild_add_dependencies(FLASH ${DEFAULT_IMAGE} net_loader)

  set_config_bool(net_loader CONFIG_BOOTLOADER_MCUBOOT "${SB_CONFIG_BOOTLOADER_MCUBOOT}")
  set_config_string(net_loader CONFIG_MCUBOOT_SIGNATURE_KEY_FILE
                    "${SB_CONFIG_BOOT_SIGNATURE_KEY_FILE}"
  )
  set_config_string(net_loader CONFIG_MCUBOOT_ENCRYPTION_KEY_FILE
                    "${SB_CONFIG_BOOT_ENCRYPTION_KEY_FILE}"
  )

  if("${SB_CONFIG_SIGNATURE_TYPE}" STREQUAL "NONE")
    set_config_bool(net_loader CONFIG_MCUBOOT_GENERATE_UNSIGNED_IMAGE y)
  else()
    set_config_bool(net_loader CONFIG_MCUBOOT_GENERATE_UNSIGNED_IMAGE n)
  endif()

  # Use net loader's MCUboot configuration directory
  set(mcuboot_APPLICATION_CONFIG_DIR ${ZEPHYR_NET_LOADER_MODULE_DIR}/net_loader/sysbuild/mcuboot
      CACHE INTERNAL "Application configuration dir controlled by sysbuild")

  # Change to use correct board for net loader
  if("${BOARD}" STREQUAL "bl5340_dvk_cpuapp")
    set_target_properties(net_loader PROPERTIES BOARD ${BOARD}@0)
  endif()
endif()

