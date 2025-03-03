#include <dm.h>
#include <dm/pinctrl.h>
#include <dt-bindings/pinctrl/k1-pinctrl.h>
#include <mapmem.h>
#include <regmap.h>
#include <syscon.h>
#include <asm/io.h>
#include <linux/err.h>
#include <linux/bitfield.h>
#include <linux/bitops.h>

struct spacemit_pin {
    u16 pin;
    u16 flag;
    u8 gpiofunc;
};

struct spacemit_pinctrl {
    struct udevice *dev;

};

static const struct udevice_id k1_pc_ids[] = {
    { .compatible = "spacemit,k1-pinctrl", .data = NULL}
};

static int spacemit_pinctrl_probe(struct udevice *dev)
{
    return 1;
};


static const struct pinctrl_ops spacemit_pinctrl_ops = {
    .pinconf_set = NULL,
    .pinmux_group_set = NULL
};

U_BOOT_DRIVER(pinctrl_k1) = {
    .name = "pinctrl_k1",
    .id = UCLASS_PINCTRL,
    .of_match = k1_pc_ids,
    .probe = spacemit_pinctrl_probe,
    .ops = &spacemit_pinctrl_ops
};