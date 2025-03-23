// SPDX-License-Identifier: GPL-2.0+
/* Copyright (C) 2023 Spacemit */

#include <dm.h>
#include <reset.h>
#include <clk.h>
#include <i2c.h>
#include <asm/io.h>
#include <linux/delay.h>
#include "spacemit_i2c.h"

/* All transfers are described by this data structure */
struct spacemit_i2c_msg {
	u8 condition;
	u8 acknack;
	u8 direction;
	u8 data;
};

struct spacemit_i2c {
	u32 icr;
	u32 isr;
	u32 isar;
	u32 idbr;
	u32 ilcr;
	u32 iwcr;
	u32 irst_cyc;
	u32 ibmr;
};

/*
 * i2c_reset: - reset the host controller
 *
 */
static void i2c_reset(struct spacemit_i2c *base)
{
	u32 icr_mode;

	/* Save bus mode (standard or fast speed) for later use */
	icr_mode = readl(&base->icr) & ICR_MODE_MASK;
	writel(readl(&base->icr) & ~ICR_IUE, &base->icr); /* disable unit */
	writel(readl(&base->icr) | ICR_UR, &base->icr);	  /* reset the unit */
	udelay(100);
	writel(readl(&base->icr) & ~ICR_IUE, &base->icr); /* disable unit */

	writel(0x00, &base->isar); /* set our slave address */

	/* set control reg values */
	writel(I2C_ICR_INIT | icr_mode, &base->icr);
	writel(I2C_ISR_INIT, &base->isr); /* set clear interrupt bits */
	writel(readl(&base->icr) | ICR_IUE, &base->icr); /* enable unit */
	udelay(1e0);
}

/*
 * i2c_isr_set_cleared: - wait until certain bits of the I2C status register
 *	                  are set and cleared
 *
 * @return: 1 in case of success, 0 means timeout (no match within 10 ms).
 */
static int i2c_isr_set_cleared(struct spacemit_i2c *base, unsigned long set_mask,
			       unsigned long cleared_mask)
{
	int timeout = 1000, isr;

	do {
		isr = readl(&base->isr);
		udelay(10);
		if (timeout-- < 0)
			return 0;
	} while (((isr & set_mask) != set_mask) || ((isr & cleared_mask) != 0));

	return 1;
}

/*
 * i2c_transfer: - Transfer one byte over the i2c bus
 *
 * This function can transfer a byte over the i2c bus in both directions.
 * It is used by the public API functions.
 *
 * @return:  0: transfer successful
 *          -1: message is empty
 *          -2: transmit timeout
 *          -3: ACK missing
 *          -4: receive timeout
 *          -5: illegal parameters
 *          -6: bus is busy and couldn't be acquired
 */
static int i2c_transfer(struct spacemit_i2c *base, struct spacemit_i2c_msg *msg)
{
	int ret;

	if (!msg)
		goto transfer_error_msg_empty;

	switch (msg->direction) {
	case I2C_WRITE:
		/* check if bus is not busy */
		if (!i2c_isr_set_cleared(base, 0, ISR_IBB))
			goto transfer_error_bus_busy;

		/* start transmission */
		writel(readl(&base->icr) & ~ICR_START, &base->icr);
		writel(readl(&base->icr) & ~ICR_STOP, &base->icr);
		writel(msg->data, &base->idbr);
		if (msg->condition == I2C_COND_START)
			writel(readl(&base->icr) | ICR_START, &base->icr);
		if (msg->condition == I2C_COND_STOP)
			writel(readl(&base->icr) | ICR_STOP, &base->icr);
		if (msg->acknack == I2C_ACKNAK_SENDNAK)
			writel(readl(&base->icr) | ICR_ACKNAK, &base->icr);
		if (msg->acknack == I2C_ACKNAK_SENDACK)
			writel(readl(&base->icr) & ~ICR_ACKNAK, &base->icr);
		writel(readl(&base->icr) & ~ICR_ALDIE, &base->icr);
		writel(readl(&base->icr) | ICR_TB, &base->icr);

		/* transmit register empty? */
		if (!i2c_isr_set_cleared(base, ISR_ITE, 0))
			goto transfer_error_transmit_timeout;

		/* clear 'transmit empty' state */
		writel(readl(&base->isr) | ISR_ITE, &base->isr);

		/* wait for ACK from slave */
		if (msg->acknack == I2C_ACKNAK_WAITACK)
			if (!i2c_isr_set_cleared(base, 0, ISR_ACKNAK))
				goto transfer_error_ack_missing;
		break;

	case I2C_READ:

		/* check if bus is not busy */
		if (!i2c_isr_set_cleared(base, 0, ISR_IBB))
			goto transfer_error_bus_busy;

		/* start receive */
		writel(readl(&base->icr) & ~ICR_START, &base->icr);
		writel(readl(&base->icr) & ~ICR_STOP, &base->icr);
		if (msg->condition == I2C_COND_START)
			writel(readl(&base->icr) | ICR_START, &base->icr);
		if (msg->condition == I2C_COND_STOP)
			writel(readl(&base->icr) | ICR_STOP, &base->icr);
		if (msg->acknack == I2C_ACKNAK_SENDNAK)
			writel(readl(&base->icr) | ICR_ACKNAK, &base->icr);
		if (msg->acknack == I2C_ACKNAK_SENDACK)
			writel(readl(&base->icr) & ~ICR_ACKNAK, &base->icr);
		writel(readl(&base->icr) & ~ICR_ALDIE, &base->icr);
		writel(readl(&base->icr) | ICR_TB, &base->icr);

		/* receive register full? */
		if (!i2c_isr_set_cleared(base, ISR_IRF, 0))
			goto transfer_error_receive_timeout;

		msg->data = readl(&base->idbr);

		/* clear 'receive empty' state */
		writel(readl(&base->isr) | ISR_IRF, &base->isr);
		break;
	default:
		goto transfer_error_illegal_param;
	}

	return 0;

transfer_error_msg_empty:
	ret = -1;
	goto i2c_transfer_finish;

transfer_error_transmit_timeout:
	ret = -2;
	goto i2c_transfer_finish;

transfer_error_ack_missing:
	ret = -3;
	goto i2c_transfer_finish;

transfer_error_receive_timeout:
	ret = -4;
	goto i2c_transfer_finish;

transfer_error_illegal_param:
	ret = -5;
	goto i2c_transfer_finish;

transfer_error_bus_busy:
	ret = -6;
	goto i2c_transfer_finish;

i2c_transfer_finish:
	i2c_reset(base);
	return ret;
}

static int __i2c_read(struct spacemit_i2c *base, uchar chip, u8 *addr, int alen,
		      uchar *buffer, int len)
{
	struct spacemit_i2c_msg msg;

	if (len == 0) {
		pr_err("reading zero byte is invalid\n");
		return -EINVAL;
	}

	i2c_reset(base);

	/* dummy chip address write */
	msg.condition = I2C_COND_START;
	msg.acknack   = I2C_ACKNAK_WAITACK;
	msg.direction = I2C_WRITE;
	msg.data = (chip << 1);
	msg.data &= 0xFE;
	if (i2c_transfer(base, &msg))
		return -1;

	/*
	 * send memory address bytes;
	 * alen defines how much bytes we have to send.
	 */
	while (--alen >= 0) {
		msg.condition = I2C_COND_NORMAL;
		msg.acknack   = I2C_ACKNAK_WAITACK;
		msg.direction = I2C_WRITE;
		msg.data      = addr[alen];
		if (i2c_transfer(base, &msg))
			return -1;
	}

	/* start read sequence */
	msg.condition = I2C_COND_START;
	msg.acknack   = I2C_ACKNAK_WAITACK;
	msg.direction = I2C_WRITE;
	msg.data      = (chip << 1);
	msg.data     |= 0x01;
	if (i2c_transfer(base, &msg))
		return -1;

	/* read bytes; send NACK at last byte */
	while (len--) {
		if (len == 0) {
			msg.condition = I2C_COND_STOP;
			msg.acknack   = I2C_ACKNAK_SENDNAK;
		} else {
			msg.condition = I2C_COND_NORMAL;
			msg.acknack   = I2C_ACKNAK_SENDACK;
		}

		msg.direction = I2C_READ;
		msg.data      = 0x00;
		if (i2c_transfer(base, &msg))
			return -1;

		*buffer = msg.data;
		buffer++;
	}

	i2c_reset(base);

	return 0;
}

static int __i2c_write(struct spacemit_i2c *base, uchar chip, u8 *addr, int alen,
		       uchar *buffer, int len)
{
	struct spacemit_i2c_msg msg;

	i2c_reset(base);

	/* chip address write */
	msg.condition = I2C_COND_START;
	msg.acknack   = I2C_ACKNAK_WAITACK;
	msg.direction = I2C_WRITE;
	msg.data = (chip << 1);
	msg.data &= 0xFE;
	if (i2c_transfer(base, &msg))
		return -1;

	/*
	 * send memory address bytes;
	 * alen defines how much bytes we have to send.
	 */
	while (--alen >= 0) {
		msg.condition = I2C_COND_NORMAL;
		msg.acknack   = I2C_ACKNAK_WAITACK;
		msg.direction = I2C_WRITE;
		msg.data      = addr[alen];
		if (i2c_transfer(base, &msg))
			return -1;
	}

	/* write bytes; send NACK at last byte */
	while (len--) {
		if (len == 0)
			msg.condition = I2C_COND_STOP;
		else
			msg.condition = I2C_COND_NORMAL;

		msg.acknack   = I2C_ACKNAK_WAITACK;
		msg.direction = I2C_WRITE;
		msg.data      = *(buffer++);

		if (i2c_transfer(base, &msg))
			return -1;
	}

	i2c_reset(base);

	return 0;
}

struct spacemit_i2c_priv {
	struct spacemit_i2c *base;
	struct reset_ctl_bulk resets;
	u32 clk_rate;

};

static int spacemit_i2c_xfer(struct udevice *bus, struct i2c_msg *msg, int nmsgs)
{
	struct spacemit_i2c_priv *i2c = dev_get_priv(bus);
	struct i2c_msg *dmsg, *omsg, dummy;

	memset(&dummy, 0, sizeof(struct i2c_msg));

	/*
	 * We expect either two messages (one with an offset and one with the
	 * actual data) or one message (just data or offset/data combined)
	 */
	if (nmsgs > 2 || nmsgs == 0)
		return -1;

	omsg = nmsgs == 1 ? &dummy : msg;
	dmsg = nmsgs == 1 ? msg : msg + 1;

	if (dmsg->flags & I2C_M_RD)
		return __i2c_read(i2c->base, dmsg->addr, omsg->buf,
				  omsg->len, dmsg->buf, dmsg->len);
	else
		return __i2c_write(i2c->base, dmsg->addr, omsg->buf,
				   omsg->len, dmsg->buf, dmsg->len);
}

static int spacemit_i2c_set_bus_speed(struct udevice *bus, unsigned int speed)
{
	struct spacemit_i2c_priv *priv = dev_get_priv(bus);
	u32 val;

	if (speed > 100000)
		val = ICR_FM;
	else
		val = ICR_SM;
	clrsetbits_le32(&priv->base->icr, ICR_MODE_MASK, val);

	return 0;
}

static int spacemit_i2c_bind(struct udevice *bus)
{
	return 0;
}

static int spacemit_i2c_probe(struct udevice *bus)
{
	struct spacemit_i2c_priv *priv = dev_get_priv(bus);
	int ret;

	ret = reset_get_bulk(bus, &priv->resets);
	ret = reset_deassert_bulk(&priv->resets);
	if (ret)
		return ret;

	priv->base = (void *)devfdt_get_addr_ptr(bus);
	ret = dev_read_u32(bus, "clock-frequency", &priv->clk_rate);
	if (ret) {
		pr_info("Default to 100kHz\n");
		/* default clock rate: 100k */
		priv->clk_rate = 100000;
	}

	ret = spacemit_i2c_set_bus_speed(bus, priv->clk_rate);
	return 0;
}

static const struct dm_i2c_ops spacemit_i2c_ops = {
	.xfer		= spacemit_i2c_xfer,
	.set_bus_speed	= spacemit_i2c_set_bus_speed,
};

static const struct udevice_id spacemit_i2c_ids[] = {
	{ .compatible = "spacemit,i2c" },
	{ }
};

U_BOOT_DRIVER(i2c_spacemit) = {
	.name	= "i2c_spacemit",
	.id	= UCLASS_I2C,
	.of_match = spacemit_i2c_ids,
	.bind	= spacemit_i2c_bind,
	.probe	= spacemit_i2c_probe,
	.priv_auto = sizeof(struct spacemit_i2c_priv),
	.ops	= &spacemit_i2c_ops,
};
