# SPDX-License-Identifier: Apache-2.0

zephyr_syscall_header(${ZEPHYR_BASE}/include/zephyr/drivers/mbox.h)

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_MBOX_MAUSYS_PCI mbox_mausys_pci.c)
