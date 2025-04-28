#include <dm.h>
#include <reset.h>
#include <i2c.h>
#include <asm/io.h>
#include <linux/delay.h>

/* Shall the current transfer have a start/stop condition? */
#define I2C_COND_NORMAL		0
#define I2C_COND_START		1
#define I2C_COND_STOP		2

/* Shall the current transfer be ack/nacked or being waited for it? */
#define I2C_ACKNAK_WAITACK	1
#define I2C_ACKNAK_SENDACK	2
#define I2C_ACKNAK_SENDNAK	4

/* Specify who shall transfer the data (master or slave) */
#define I2C_READ		0
#define I2C_WRITE		1

#if (CONFIG_SYS_I2C_SPEED == 400000)
#define I2C_ICR_INIT	(ICR_FM | ICR_BEIE | ICR_IRFIE | ICR_ITEIE | ICR_GCD \
		| ICR_SCLE)
#else
#define I2C_ICR_INIT	(ICR_BEIE | ICR_IRFIE | ICR_ITEIE | ICR_GCD | ICR_SCLE)
#endif

/* ----- Control register bits ---------------------------------------- */

#define ICR_START	0x1		/* start bit */
#define ICR_STOP	0x2		/* stop bit */
#define ICR_ACKNAK	0x4		/* send ACK(0) or NAK(1) */
#define ICR_TB		0x8		/* transfer byte bit */
#define ICR_MA		BIT(12)  	/* master abort */
#define ICR_SCLE	BIT(13)  	/* master clock enable, mona SCLEA */
#define ICR_IUE		BIT(14) 	/* unit enable */
#define ICR_GCD		BIT(21) 	/* general call disable */
#define ICR_ITEIE	BIT(19)  	/* enable tx interrupts */
#define ICR_IRFIE	BIT(20)  	/* enable rx interrupts, mona: DRFIE */
#define ICR_BEIE	BIT(22)  	/* enable bus error ints */
#define ICR_SSDIE	BIT(24)  	/* slave STOP detected int enable */
#define ICR_ALDIE	BIT(18)		/* enable arbitration interrupt */
#define ICR_SADIE	BIT(23)		/* slave address detected int enable */
#define ICR_UR		BIT(10)		/* unit reset */
#define ICR_SM		(0x0)		/* Standard Mode */
#define ICR_FM		BIT(8)		/* Fast Mode */
#define ICR_MODE_MASK	(0x300)		/* Mode mask */
/* ----- Status register bits ----------------------------------------- */

#define ISR_RWM		BIT(13)   	/* read/write mode */
#define ISR_ACKNAK	BIT(14)   	/* ack/nak status */
#define ISR_UB		BIT(15)   	/* unit busy */
#define ISR_IBB		BIT(16)   	/* bus busy */
#define ISR_SSD		BIT(24)   	/* slave stop detected */
#define ISR_ALD		BIT(18)   	/* arbitration loss detected */
#define ISR_ITE		BIT(19)   	/* tx buffer empty */
#define ISR_IRF		BIT(20)   	/* rx buffer full */
#define ISR_GCAD	BIT(21)		/* general call address detected */
#define ISR_SAD		BIT(23)		/* slave address detected */
#define ISR_BED		BIT(22)		/* bus error no ACK/NAK */

#define I2C_ISR_INIT	0x1FDE000


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


struct spacemit_i2c_priv {
	struct spacemit_i2c *base;
	struct reset_ctl_bulk resets;
	// TODO: clk is not implemented.
	u32 clk_rate;
};

/*
 * i2c_reset: - reset the host controller
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

#ifdef CONFIG_SYS_I2C_SLAVE
	writel(CONFIG_SYS_I2C_SLAVE, &base->isar); /* set our slave address */
#else
	writel(0x00, &base->isar); /* set our slave address */
#endif
	/* set control reg values */
	writel(I2C_ICR_INIT | icr_mode, &base->icr);
	writel(I2C_ISR_INIT, &base->isr); /* set clear interrupt bits */
	writel(readl(&base->icr) | ICR_IUE, &base->icr); /* enable unit */
	udelay(10);
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
	} while (((isr & set_mask) != set_mask)
			|| ((isr & cleared_mask) != 0));

	return 1;
}


/*
 * i2c_transfer: - Transfer one byte over the i2c bus
 *
 * This function can tranfer a byte over the i2c bus in both directions.
 * It is used by the public API functions.
 *
 * @return:  0: transfer successful
 *          -1: message is empty
 *          -2: transmit timeout
 *          -3: ACK missing
 *          -4: receive timeout
 *          -5: illegal parameters
 *          -6: bus is busy and couldn't be aquired
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
	debug("i2c_transfer: error: 'msg' is empty\n");
	ret = -1;
	goto i2c_transfer_finish;

transfer_error_transmit_timeout:
	debug("i2c_transfer: error: transmit timeout\n");
	ret = -2;
	goto i2c_transfer_finish;

transfer_error_ack_missing:
	debug("i2c_transfer: error: ACK missing\n");
	ret = -3;
	goto i2c_transfer_finish;

transfer_error_receive_timeout:
	debug("i2c_transfer: error: receive timeout\n");
	ret = -4;
	goto i2c_transfer_finish;

transfer_error_illegal_param:
	debug("i2c_transfer: error: illegal parameters\n");
	ret = -5;
	goto i2c_transfer_finish;

transfer_error_bus_busy:
	debug("i2c_transfer: error: bus is busy\n");
	ret = -6;
	goto i2c_transfer_finish;

i2c_transfer_finish:
	debug("i2c_transfer: ISR: 0x%04x\n", readl(&base->isr));
	i2c_reset(base);
	return ret;
}


static int spacemit_set_bus_speed(struct udevice *bus, unsigned int speed)
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

static int spacemit_i2c_read(struct spacemit_i2c *base, uchar chip, u8 *addr, int alen,
	uchar *buffer, int len)
{
	struct spacemit_i2c_msg msg;

	if (len == 0) {
		pr_err("reading zero byte is invalid\n");
		return -EINVAL;
	}

	i2c_reset(base);

	/* dummy chip address write */
	debug("i2c_read: dummy chip address write\n");
	msg.condition = I2C_COND_START;
	msg.acknack = I2C_ACKNAK_WAITACK;
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
		msg.acknack = I2C_ACKNAK_WAITACK;
		msg.direction = I2C_WRITE;
		msg.data = addr[alen];
		if (i2c_transfer(base, &msg))
			return -1;
	}

	/* start read sequence */
	msg.condition = I2C_COND_START;
	msg.acknack = I2C_ACKNAK_WAITACK;
	msg.direction = I2C_WRITE;
	msg.data = (chip << 1);
	msg.data |= 0x01;
	if (i2c_transfer(base, &msg))
		return -1;

	/* read bytes; send NACK at last byte */
	while (len--) {
		if (len == 0) {
			msg.condition = I2C_COND_STOP;
			msg.acknack = I2C_ACKNAK_SENDNAK;
		} else {
			msg.condition = I2C_COND_NORMAL;
			msg.acknack = I2C_ACKNAK_SENDACK;
		}

		msg.direction = I2C_READ;
		msg.data = 0x00;
		if (i2c_transfer(base, &msg))
			return -1;

		*buffer = msg.data;
		buffer++;
	}

	i2c_reset(base);

	return 0;
}

static int spacemit_i2c_write(struct spacemit_i2c *base, uchar chip, u8 *addr, int alen,
	uchar *buffer, int len)
{
	struct spacemit_i2c_msg msg;

	i2c_reset(base);

	// chip address write
	msg.condition = I2C_COND_START;
	msg.acknack   = I2C_ACKNAK_WAITACK;
	msg.direction = I2C_WRITE;
	msg.data = (chip << 1);
	msg.data &= 0xFE;

	if (i2c_transfer(base, &msg))
		return -1;


    // send memory address bytes;
	// alen defines how much bytes we have to send.
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

		msg.acknack = I2C_ACKNAK_WAITACK;
		msg.direction = I2C_WRITE;
		msg.data = *(buffer++);

		if (i2c_transfer(base, &msg))
			return -1;
	}

	i2c_reset(base);

	return 0;
}

static int spacemit_i2c_xfer(struct udevice *bus, struct i2c_msg *msg, int nmsgs)
{
	struct spacemit_i2c_priv *i2c = dev_get_priv(bus);
	struct i2c_msg *dmsg, *omsg, dummy;

	memset(&dummy, 0, sizeof(struct i2c_msg));

	/*
	 * We expect either two messages (one with an offset and one with the
	 * actual data) or one message (just data or offset/data combined)
	 */
	if (nmsgs > 2 || nmsgs == 0) {
		debug("%s: Only one or two messages are supported.", __func__);
		return -1;
	}

	omsg = nmsgs == 1 ? &dummy : msg;
	dmsg = nmsgs == 1 ? msg : msg + 1;

	if (dmsg->flags & I2C_M_RD)
		return spacemit_i2c_read(i2c->base, dmsg->addr, omsg->buf,
				  omsg->len, dmsg->buf, dmsg->len);
	else
		return spacemit_i2c_write(i2c->base, dmsg->addr, omsg->buf,
				   omsg->len, dmsg->buf, dmsg->len);
}

static int spacemit_i2c_probe(struct udevice *bus)
{
	struct spacemit_i2c_priv *priv = dev_get_priv(bus);
	int ret;

	ret = reset_get_bulk(bus, &priv->resets);
	ret = reset_deassert_bulk(&priv->resets);
	if (ret) {
		debug("I2C probe: failed to reset \n");
		return ret;
	}

	// TODO: clk is not implemented, the clk_rate is hard coded
	priv->clk_rate = 100000;

	ret = spacemit_set_bus_speed(bus, priv->clk_rate);
	return 0;
}

static const struct dm_i2c_ops spacemit_i2c_ops = {
    .xfer = spacemit_i2c_xfer,
	.set_bus_speed = spacemit_set_bus_speed
};

static const struct udevice_id spacemit_i2c_ids[] = {
    { .compatible = "spacemit,i2c" },
};

U_BOOT_DRIVER(i2c_spacemit) = {
	.name = "i2c_spacemit",
	.id	= UCLASS_I2C,
	.of_match = spacemit_i2c_ids,
	.probe = spacemit_i2c_probe,
	.priv_auto = sizeof(struct spacemit_i2c_priv),
	.ops = &spacemit_i2c_ops,
};