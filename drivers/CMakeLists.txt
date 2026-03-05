# SPDX-License-Identifier: Apache-2.0

add_subdirectory_ifdef(CONFIG_MBOX mbox)

add_subdirectory_ifdef(CONFIG_REMOTEPROC remoteproc)
