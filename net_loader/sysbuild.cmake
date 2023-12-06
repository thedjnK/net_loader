#
# Copyright (C) 2023 Jamie M.
#
# SPDX-License-Identifier: Apache-2.0
#

set_target_properties(mcuboot PROPERTIES BOARD ${BOARD})
set_target_properties(app PROPERTIES BOARD ${BOARD}@0)
