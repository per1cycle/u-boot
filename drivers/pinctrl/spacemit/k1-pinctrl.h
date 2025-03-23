#ifndef K1_PINCTRL_H
#define K1_PINCTRL_H
#include <dm.h>
#include <dm/device-internal.h>
#include <dm/lists.h>
#include <dm/pinctrl.h>
#include <errno.h>
#include <malloc.h>


struct spacemit_pin {
	unsigned pin;
	const char* name;
};

struct spacemit_pinctrl_soc_info {
    
};

struct spacemit_pinctrl_priv {
	void __iomem *regs;
	struct spacemit_pinctrl_soc_info info;
};


#endif // K1_PINCTRL_H