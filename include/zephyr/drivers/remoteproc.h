/*
 * Copyright (c) 2026 Simon Maurer <mail@maurer.systems>
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_DRIVERS_REMOTEPROC_H_
#define ZEPHYR_INCLUDE_DRIVERS_REMOTEPROC_H_

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fw_rsc_carveout;
struct fw_rsc_vdev;

#ifdef CONFIG_OPENAMP_RPMSG_VDEV
struct rpmsg_device;
#endif

void *z_rproc_get_rsc_table(void);

size_t z_rproc_get_rsc_table_size(void);

struct fw_rsc_carveout *z_rproc_get_carveout_by_name(const char *name);

struct fw_rsc_carveout *z_rproc_get_carveout_by_index(unsigned int idx);

struct fw_rsc_vdev *z_rproc_get_vdev(unsigned int idx);

#ifdef CONFIG_OPENAMP_RPMSG_VDEV

struct rpmsg_device *z_rproc_get_rpmsg_device(const struct device *dev);

#endif

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_DRIVERS_REMOTEPROC_H_ */
