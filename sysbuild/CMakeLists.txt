#
# Copyright (C) 2023 Jamie M.
#
# SPDX-License-Identifier: Apache-2.0
#

function(${SYSBUILD_CURRENT_MODULE_NAME}_pre_cmake)
  cmake_parse_arguments(INPUT "" "" "IMAGES" ${ARGN})

  math(EXPR WDT_FEED_TIME "${SB_CONFIG_WATCHDOG_TIMEOUT_TIME} / 2")

  set_property(TARGET mcuboot APPEND_STRING PROPERTY CONFIG "CONFIG_MCUBOOT_WATCHDOG_SETUP=y\n")
  set_property(TARGET mcuboot APPEND_STRING PROPERTY CONFIG "CONFIG_MCUBOOT_WATCHDOG_TIMEOUT=${SB_CONFIG_WATCHDOG_TIMEOUT_TIME}\n")

  foreach(image ${INPUT_IMAGES})
    set_property(TARGET ${image} APPEND_STRING PROPERTY CONFIG "CONFIG_WATCHDOG=y\n")
    set_property(TARGET ${image} APPEND_STRING PROPERTY CONFIG "CONFIG_WATCHDOG_FEED_TIME=${WDT_FEED_TIME}\n")
  endforeach()
endfunction()

