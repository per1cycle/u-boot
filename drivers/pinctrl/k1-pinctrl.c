
#include <dm/read.h>
#include <linux/io.h>
#include <clk.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <dm/lists.h>
#include <dm/pinctrl.h>
#include <asm-generic/gpio.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <dm/device_compat.h>


#define SPACEMIT_PINCTRL(x, y, z) {.pin = x, .offset = y, .name = z}
#define OFFSET(x) (x << 2)

struct spacemit_pin {
	unsigned int pin;
	unsigned int offset;
	char *name;
};

/*
 * +---------+----------+-----------+--------+--------+----------+--------+
 * |   pull  |   drive  | schmitter |  slew  |  edge  |  strong  |   mux  |
 * | up/down | strength |  trigger  |  rate  | detect |   pull   |  mode  |
 * +---------+----------+-----------+--------+--------+----------+--------+
 *   3 bits     3 bits     2 bits     1 bit    3 bits     1 bit    3 bits
 */
#define PAD_MUX			GENMASK(2, 0)
#define PAD_STRONG_PULL		BIT(3)
#define PAD_EDGE_RISE		BIT(4)
#define PAD_EDGE_FALL		BIT(5)
#define PAD_EDGE_CLEAR		BIT(6)
#define PAD_SLEW_RATE		GENMASK(12, 11)
#define PAD_SLEW_RATE_EN	BIT(7)
#define PAD_SCHMITT		GENMASK(9, 8)
#define PAD_DRIVE		GENMASK(12, 10)
#define PAD_PULLDOWN		BIT(13)
#define PAD_PULLUP		BIT(14)
#define PAD_PULL_EN		BIT(15)

struct spacemit_pin spacemit_k1_pins[] = {
	SPACEMIT_PINCTRL(0, OFFSET(1), "GPIO_00"),
	SPACEMIT_PINCTRL(1, OFFSET(2), "GPIO_01"),
	SPACEMIT_PINCTRL(2, OFFSET(3), "GPIO_02"),
	SPACEMIT_PINCTRL(3, OFFSET(4), "GPIO_03"),
	SPACEMIT_PINCTRL(4, OFFSET(5), "GPIO_04"),
	SPACEMIT_PINCTRL(5, OFFSET(6), "GPIO_05"),
	SPACEMIT_PINCTRL(6, OFFSET(7), "GPIO_06"),
	SPACEMIT_PINCTRL(7, OFFSET(8), "GPIO_07"),
	SPACEMIT_PINCTRL(8, OFFSET(9), "GPIO_08"),
	SPACEMIT_PINCTRL(9, OFFSET(10), "GPIO_09"),
	SPACEMIT_PINCTRL(10, OFFSET(11), "GPIO_10"),
	SPACEMIT_PINCTRL(11, OFFSET(12), "GPIO_11"),
	SPACEMIT_PINCTRL(12, OFFSET(13), "GPIO_12"),
	SPACEMIT_PINCTRL(13, OFFSET(14), "GPIO_13"),
	SPACEMIT_PINCTRL(14, OFFSET(15), "GPIO_14"),
	SPACEMIT_PINCTRL(15, OFFSET(16), "GPIO_15"),
	SPACEMIT_PINCTRL(16, OFFSET(17), "GPIO_16"),
	SPACEMIT_PINCTRL(17, OFFSET(18), "GPIO_17"),
	SPACEMIT_PINCTRL(18, OFFSET(19), "GPIO_18"),
	SPACEMIT_PINCTRL(19, OFFSET(20), "GPIO_19"),
	SPACEMIT_PINCTRL(20, OFFSET(21), "GPIO_20"),
	SPACEMIT_PINCTRL(21, OFFSET(22), "GPIO_21"),
	SPACEMIT_PINCTRL(22, OFFSET(23), "GPIO_22"),
	SPACEMIT_PINCTRL(23, OFFSET(24), "GPIO_23"),
	SPACEMIT_PINCTRL(24, OFFSET(25), "GPIO_24"),
	SPACEMIT_PINCTRL(25, OFFSET(26), "GPIO_25"),
	SPACEMIT_PINCTRL(26, OFFSET(27), "GPIO_26"),
	SPACEMIT_PINCTRL(27, OFFSET(28), "GPIO_27"),
	SPACEMIT_PINCTRL(28, OFFSET(29), "GPIO_28"),
	SPACEMIT_PINCTRL(29, OFFSET(30), "GPIO_29"),
	SPACEMIT_PINCTRL(30, OFFSET(31), "GPIO_30"),
	SPACEMIT_PINCTRL(31, OFFSET(32), "GPIO_31"),
	SPACEMIT_PINCTRL(32, OFFSET(33), "GPIO_32"),
	SPACEMIT_PINCTRL(33, OFFSET(34), "GPIO_33"),
	SPACEMIT_PINCTRL(34, OFFSET(35), "GPIO_34"),
	SPACEMIT_PINCTRL(35, OFFSET(36), "GPIO_35"),
	SPACEMIT_PINCTRL(36, OFFSET(37), "GPIO_36"),
	SPACEMIT_PINCTRL(37, OFFSET(38), "GPIO_37"),
	SPACEMIT_PINCTRL(38, OFFSET(39), "GPIO_38"),
	SPACEMIT_PINCTRL(39, OFFSET(40), "GPIO_39"),
	SPACEMIT_PINCTRL(40, OFFSET(41), "GPIO_40"),
	SPACEMIT_PINCTRL(41, OFFSET(42), "GPIO_41"),
	SPACEMIT_PINCTRL(42, OFFSET(43), "GPIO_42"),
	SPACEMIT_PINCTRL(43, OFFSET(44), "GPIO_43"),
	SPACEMIT_PINCTRL(44, OFFSET(45), "GPIO_44"),
	SPACEMIT_PINCTRL(45, OFFSET(46), "GPIO_45"),
	SPACEMIT_PINCTRL(46, OFFSET(47), "GPIO_46"),
	SPACEMIT_PINCTRL(47, OFFSET(48), "GPIO_47"),
	SPACEMIT_PINCTRL(48, OFFSET(49), "GPIO_48"),
	SPACEMIT_PINCTRL(49, OFFSET(50), "GPIO_49"),
	SPACEMIT_PINCTRL(50, OFFSET(51), "GPIO_50"),
	SPACEMIT_PINCTRL(51, OFFSET(52), "GPIO_51"),
	SPACEMIT_PINCTRL(52, OFFSET(53), "GPIO_52"),
	SPACEMIT_PINCTRL(53, OFFSET(54), "GPIO_53"),
	SPACEMIT_PINCTRL(54, OFFSET(55), "GPIO_54"),
	SPACEMIT_PINCTRL(55, OFFSET(56), "GPIO_55"),
	SPACEMIT_PINCTRL(56, OFFSET(57), "GPIO_56"),
	SPACEMIT_PINCTRL(57, OFFSET(58), "GPIO_57"),
	SPACEMIT_PINCTRL(58, OFFSET(59), "GPIO_58"),
	SPACEMIT_PINCTRL(59, OFFSET(60), "GPIO_59"),
	SPACEMIT_PINCTRL(60, OFFSET(61), "GPIO_60"),
	SPACEMIT_PINCTRL(61, OFFSET(62), "GPIO_61"),
	SPACEMIT_PINCTRL(62, OFFSET(63), "GPIO_62"),
	SPACEMIT_PINCTRL(63, OFFSET(64), "GPIO_63"),
	SPACEMIT_PINCTRL(64, OFFSET(65), "GPIO_64"),
	SPACEMIT_PINCTRL(65, OFFSET(66), "GPIO_65"),
	SPACEMIT_PINCTRL(66, OFFSET(67), "GPIO_66"),
	SPACEMIT_PINCTRL(67, OFFSET(68), "GPIO_67"),
	SPACEMIT_PINCTRL(68, OFFSET(69), "GPIO_68"),
	SPACEMIT_PINCTRL(69, OFFSET(70), "GPIO_69"),
	SPACEMIT_PINCTRL(70, OFFSET(71), "GPIO_70/PRI_TDI"),
	SPACEMIT_PINCTRL(71, OFFSET(72), "GPIO_71/PRI_TMS"),
	SPACEMIT_PINCTRL(72, OFFSET(73), "GPIO_72/PRI_TCK"),
	SPACEMIT_PINCTRL(73, OFFSET(74), "GPIO_73/PRI_TDO"),
	SPACEMIT_PINCTRL(74, OFFSET(75), "GPIO_74"),
	SPACEMIT_PINCTRL(75, OFFSET(76), "GPIO_75"),
	SPACEMIT_PINCTRL(76, OFFSET(77), "GPIO_76"),
	SPACEMIT_PINCTRL(77, OFFSET(78), "GPIO_77"),
	SPACEMIT_PINCTRL(78, OFFSET(79), "GPIO_78"),
	SPACEMIT_PINCTRL(79, OFFSET(80), "GPIO_79"),
	SPACEMIT_PINCTRL(80, OFFSET(81), "GPIO_80"),
	SPACEMIT_PINCTRL(81, OFFSET(82), "GPIO_81"),
	SPACEMIT_PINCTRL(82, OFFSET(83), "GPIO_82"),
	SPACEMIT_PINCTRL(83, OFFSET(84), "GPIO_83"),
	SPACEMIT_PINCTRL(84, OFFSET(85), "GPIO_84"),
	SPACEMIT_PINCTRL(85, OFFSET(86), "GPIO_85"),
	SPACEMIT_PINCTRL(86, OFFSET(123), "GPIO_86"),
	SPACEMIT_PINCTRL(87, OFFSET(124), "GPIO_87"),
	SPACEMIT_PINCTRL(88, OFFSET(125), "GPIO_88"),
	SPACEMIT_PINCTRL(89, OFFSET(126), "GPIO_89"),
	SPACEMIT_PINCTRL(90, OFFSET(127), "GPIO_90"),
	SPACEMIT_PINCTRL(91, OFFSET(128), "GPIO_91"),
	SPACEMIT_PINCTRL(92, OFFSET(129), "GPIO_92"),
	SPACEMIT_PINCTRL(93, OFFSET(117), "GPIO_93/PWR_SCL"),
	SPACEMIT_PINCTRL(94, OFFSET(118), "GPIO_94/PWR_SDA"),
	SPACEMIT_PINCTRL(95, OFFSET(119), "GPIO_95/VCX0_EN"),
	SPACEMIT_PINCTRL(96, OFFSET(120), "GPIO_96/DVL0"),
	SPACEMIT_PINCTRL(97, OFFSET(121), "GPIO_97/DVL1"),
	SPACEMIT_PINCTRL(98, OFFSET(93), "GPIO_98/QSPI_DAT3"),
	SPACEMIT_PINCTRL(99, OFFSET(92), "GPIO_99/QSPI_DAT2"),
	SPACEMIT_PINCTRL(100, OFFSET(91), "GPIO_100/QSPI_DAT1"),
	SPACEMIT_PINCTRL(101, OFFSET(90), "GPIO_101/QSPI_DAT0"),
	SPACEMIT_PINCTRL(102, OFFSET(95), "GPIO_102/QSPI_CLK"),
	SPACEMIT_PINCTRL(103, OFFSET(94), "GPIO_103/QSPI_CS1"),
	SPACEMIT_PINCTRL(104, OFFSET(110), "GPIO_104/MMC1_DAT3"),
	SPACEMIT_PINCTRL(105, OFFSET(111), "GPIO_105/MMC1_DAT2"),
	SPACEMIT_PINCTRL(106, OFFSET(112), "GPIO_106/MMC1_DAT1"),
	SPACEMIT_PINCTRL(107, OFFSET(113), "GPIO_107/MMC1_DAT0"),
	SPACEMIT_PINCTRL(108, OFFSET(114), "GPIO_108/MMC1_CMD"),
	SPACEMIT_PINCTRL(109, OFFSET(115), "GPIO_109/MMC1_CLK"),
	SPACEMIT_PINCTRL(110, OFFSET(116), "GPIO_110"),
	SPACEMIT_PINCTRL(111, OFFSET(131), "GPIO_111"),
	SPACEMIT_PINCTRL(112, OFFSET(132), "GPIO_112"),
	SPACEMIT_PINCTRL(113, OFFSET(133), "GPIO_113"),
	SPACEMIT_PINCTRL(114, OFFSET(134), "GPIO_114"),
	SPACEMIT_PINCTRL(115, OFFSET(135), "GPIO_115"),
	SPACEMIT_PINCTRL(116, OFFSET(136), "GPIO_116"),
	SPACEMIT_PINCTRL(117, OFFSET(137), "GPIO_117"),
	SPACEMIT_PINCTRL(118, OFFSET(138), "GPIO_118"),
	SPACEMIT_PINCTRL(119, OFFSET(139), "GPIO_119"),
	SPACEMIT_PINCTRL(120, OFFSET(140), "GPIO_120"),
	SPACEMIT_PINCTRL(121, OFFSET(141), "GPIO_121"),
	SPACEMIT_PINCTRL(122, OFFSET(142), "GPIO_122"),
	SPACEMIT_PINCTRL(123, OFFSET(143), "GPIO_123"),
	SPACEMIT_PINCTRL(124, OFFSET(144), "GPIO_124"),
	SPACEMIT_PINCTRL(125, OFFSET(145), "GPIO_125"),
	SPACEMIT_PINCTRL(126, OFFSET(146), "GPIO_126"),
	SPACEMIT_PINCTRL(127, OFFSET(147), "GPIO_127"),
}; 

struct spacemit_k1_pinctrl_info {
	void __iomem *base;
};

struct spacemit_k1_pinctrl_info k1_pinctrl_info = {};

struct spacemit_k1_pinctrl_priv
{
	struct udevice *udev;
	struct spacemit_k1_pinctrl_info *info;
};

static int spacemit_k1_pinctrl_probe(struct udevice *dev)
{
	struct spacemit_k1_pinctrl_info *info = (struct spacemit_k1_pinctrl_info* )dev_get_driver_data(dev);
	struct spacemit_k1_pinctrl_priv *priv = (struct spacemit_k1_pinctrl_priv* )dev_get_priv(dev);

	if (!info) {
		printk("[ERROR]: cannot get k1_pinctrl_info\n");
		return -EINVAL;
	}

	priv->udev = dev;
	info->base = dev_read_addr_ptr(dev);
	priv->info = info;

	if (!priv->info->base) {
		printk("[ERROR]: failed to set reg base.\n");
		return -EINVAL;
	}
	return 0;
}

static int spacemit_get_pins_count(struct udevice *dev)
{
	return ARRAY_SIZE(spacemit_k1_pins);
}

static const char* spacemit_get_pin_name(struct udevice *dev, unsigned selector)
{
	return spacemit_k1_pins[selector].name;
}

static int spacemit_pinmux_set(struct udevice *dev, unsigned pin_selector,
	unsigned func_selector)
{
	struct spacemit_k1_pinctrl_priv *priv = dev_get_priv(dev);

	if(pin_selector >= ARRAY_SIZE(spacemit_k1_pins))
		return -EINVAL;
	
	printk("[DEBUG]: Implement k1 pinctrl %s\n", __func__);
	printk("pin_selector: %d, func_selector: %d\n", pin_selector, func_selector);
	printk("pin name: %s\n", spacemit_k1_pins[pin_selector].name);

	void __iomem *reg = priv->info->base + spacemit_k1_pins[pin_selector].offset;
	u32 val = readl(reg);
	val &= ~PAD_MUX;
	val |= func_selector;
	writel(val, reg);
	printk("reg: %p, val: 0x%08x\n", reg, val);

	return 0;
}

static int spacemit_pinconf_set(struct udevice *dev, unsigned pin_selector,
	unsigned param, unsigned argument)
{

	return 0;
}

static int spacemit_get_pin_muxing(struct udevice *dev, unsigned int selector,
	char *buf, int size)
{
	struct spacemit_k1_pinctrl_priv *priv = dev_get_priv(dev);
	void __iomem *reg = priv->info->base + spacemit_k1_pins[selector].offset;
	u32 val = readl(reg);

	switch (val & PAD_MUX) {
	case 0:
		strncpy(buf, "GPIO", size);
		break;
	case 1:
		strncpy(buf, "FUNC1", size);
		break;
	case 2:
		strncpy(buf, "FUNC2", size);
		break;
	default:
		strncpy(buf, "UNKNOWN", size);
		break;
	}

	return 0;
}

static struct pinctrl_ops spacemit_k1_pinctrl_ops = {
	.get_pins_count = spacemit_get_pins_count,
	.get_pin_name = spacemit_get_pin_name,
	.pinmux_set = spacemit_pinmux_set,
	.get_pin_muxing = spacemit_get_pin_muxing,
};

static const struct udevice_id k1_pinctrl_ids[] = {
	/* k1 pinctrl */
	{ .compatible = "spacemit,k1-pinctrl", .data = (ulong)&k1_pinctrl_info},
	{ /* sentinel */ }
};

U_BOOT_DRIVER(spacemit_k1_pinctrl) = {
	.name		= "spacemit_k1_pinctrl",
	.id		= UCLASS_PINCTRL,
	.of_match	= k1_pinctrl_ids,
	.ops		= &spacemit_k1_pinctrl_ops,
	.probe		= spacemit_k1_pinctrl_probe,
};
