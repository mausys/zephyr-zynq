/*
 * Copyright (c) 2026 Simon Maurer <mail@maurer.systems>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/mbox.h>

#define LOG_LEVEL CONFIG_MBOX_LOG_LEVEL
#include <zephyr/logging/log.h>
#include <zephyr/irq.h>

LOG_MODULE_REGISTER(mbox_pci);

#define DT_DRV_COMPAT mausys_mbox_pci

#define MSIC_ENABLED     BIT(0)
#define MSIC_SET         BIT(1)
#define MSIC_PENDING     BIT(1)
#define MSIC_VECTOR_POS  2
#define MSIC_VECTOR_MASK 0x1f

struct mbox_pci_data {
	DEVICE_MMIO_RAM;
	mbox_callback_t cb;
	void *user_data;
	const struct device *dev;
	bool enabled;
};

struct mbox_pci_config {
	DEVICE_MMIO_ROM;
	uint32_t irq;
	uint32_t msi_vector;
};

static void mbox_pci_isr(const struct device *dev)
{
	struct mbox_pci_data *data = dev->data;
	if (data->enabled && data->cb) {
		data->cb(dev, 0, data->user_data, NULL);
	}
}

static int mbox_pci_send(const struct device *dev, uint32_t channel, const struct mbox_msg *msg)
{
	const struct mbox_pci_config *config = dev->config;
	mm_reg_t base = DEVICE_MMIO_GET(dev);

	if (msg) {
		LOG_WRN("Sending data not supported");
	}

	if (channel != 0) {
		return -EINVAL;
	}

	uint32_t reg = sys_read32(base);

	if (!(reg & MSIC_ENABLED)) {
		LOG_WRN("MSI not enabled");
		return -EIO;
	}

	if (reg & MSIC_PENDING) {
		LOG_WRN("MSI pending");
		return -EBUSY;
	}

	sys_write32(MSIC_SET | (config->msi_vector << MSIC_VECTOR_POS), base);

	return 0;
}

static int mbox_pci_register_callback(const struct device *dev, uint32_t channel,
				      mbox_callback_t cb, void *user_data)
{
	struct mbox_pci_data *data = dev->data;
	uint32_t key;

	if (channel != 0) {
		return -EINVAL;
	}

	key = irq_lock();
	data->cb = cb;
	data->user_data = user_data;
	irq_unlock(key);

	return 0;
}

static int mbox_pci_mtu_get(const struct device *dev)
{
	/* We only support signalling */
	return 0;
}

static uint32_t mbox_pci_max_channels_get(const struct device *dev)
{
	return 1;
}

static int mbox_pci_set_enabled(const struct device *dev, uint32_t channel, bool enable)
{
	struct mbox_pci_data *data = dev->data;
	const struct mbox_pci_config *config = dev->config;

	if (channel != 0) {
		return -EINVAL;
	}

	if ((enable == 0 && !data->enabled) || (enable != 0 && data->enabled)) {
		return -EALREADY;
	}

	if (enable && (data->cb == NULL)) {
		LOG_WRN("Enabling channel without a registered callback\n");
	}

	data->enabled = enable;

	if (enable) {
		irq_enable(config->irq);
	} else {
		irq_disable(config->irq);
	}

	return 0;
}

static const struct mbox_driver_api mbox_pci_driver_api = {
	.send = mbox_pci_send,
	.register_callback = mbox_pci_register_callback,
	.mtu_get = mbox_pci_mtu_get,
	.max_channels_get = mbox_pci_max_channels_get,
	.set_enabled = mbox_pci_set_enabled,
};

#define MAILBOX_INSTANCE_DEFINE(idx)                                                               \
	static struct mbox_pci_data mbox_pci_##idx##_data;                                         \
	const static struct mbox_pci_config mbox_pci_##idx##_config = {                            \
		DEVICE_MMIO_ROM_INIT(DT_DRV_INST(idx)),                                            \
		.irq = DT_INST_IRQN(idx),                                                          \
		.msi_vector = 0,                                                                   \
	};                                                                                         \
                                                                                                   \
	static int mbox_pci_##idx##_init(const struct device *dev)                                 \
	{                                                                                          \
		DEVICE_MMIO_MAP(dev, K_MEM_CACHE_NONE);                                            \
		IRQ_CONNECT(DT_INST_IRQN(idx), DT_INST_IRQ(idx, priority), mbox_pci_isr,           \
			    DEVICE_DT_INST_GET(idx), 0);                                           \
		irq_enable(DT_INST_IRQN(idx));                                                     \
		return 0;                                                                          \
	}                                                                                          \
	DEVICE_DT_INST_DEFINE(idx, mbox_pci_##idx##_init, NULL, &mbox_pci_##idx##_data,            \
			      &mbox_pci_##idx##_config, POST_KERNEL, CONFIG_MBOX_INIT_PRIORITY,    \
			      &mbox_pci_driver_api)

#define MAILBOX_INST(idx) MAILBOX_INSTANCE_DEFINE(idx);

DT_INST_FOREACH_STATUS_OKAY(MAILBOX_INST)
