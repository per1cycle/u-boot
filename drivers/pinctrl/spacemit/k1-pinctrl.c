
#include <clk.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <dm/lists.h>
#include <dm/pinctrl.h>
#include <errno.h>
#include <malloc.h>



static const struct pinconf_param spacemit_pinconf_params[] = {
};

static unsigned int spacemit_pin_to_offset(unsigned int pin)
{
	unsigned int offset = 0;

	switch (pin) {
	case 0 ... 85:
		offset = pin + 1;
		break;
	case 86 ... 92:
		offset = pin + 37;
		break;
	case 93 ... 97:
		offset = pin + 24;
		break;
	case 98:
		offset = 93;
		break;
	case 99:
		offset = 92;
		break;
	case 100:
		offset = 91;
		break;
	case 101:
		offset = 90;
		break;
	case 102:
		offset = 95;
		break;
	case 103:
		offset = 94;
		break;
	case 104 ... 110:
		offset = pin + 6;
		break;
	case 111 ... 127:
		offset = pin + 20;
		break;
	default:
		break;
	}

	return offset << 2;
}

static int spacemit_property_set() {
	return 0;
}

static int spacemit_pinconf_set() {
	return 0;
}

const struct pinctrl_ops spacemit_pinctrl_ops = {
	.set_state = pinctrl_generic_set_state,
	.pinconf_num_params	= ARRAY_SIZE(spacemit_pinconf_params),
	.pinconf_params		= spacemit_pinconf_params,
	.pinconf_set		= spacemit_pinconf_set,
	.pinmux_property_set = spacemit_property_set,
};

static int k1_pinctrl_probe(struct udevice *dev) {
	return 0;
}

static const struct udevice_id spacemit_k1_pinctrl_ids[] = {
	{.compatible = "spacemit,k1-pinctrl"}
};

U_BOOT_DRIVER(k1_pinctrl) = {
	.name		= "k1-pinctrl",
	.id		= UCLASS_PINCTRL,
	.of_match	= spacemit_k1_pinctrl_ids,
	.probe		= k1_pinctrl_probe,
};
