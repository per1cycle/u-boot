#ifndef __PINCTRL_K1_H__
#define __PINCTRL_K1_H__

enum spacemit_pin_io_type {
	IO_TYPE_NONE = 0,
	IO_TYPE_1V8,
	IO_TYPE_3V3,
	IO_TYPE_EXTERNAL,
};

#define PIN_POWER_STATE_1V8		1800
#define PIN_POWER_STATE_3V3		3300

#define K1_PIN_IO_TYPE		GENMASK(2, 1)
#define K1_FUNC_PIN(_id, _gpiofunc, _io) \
    { .pin = _id, .flag = K1_PIN_IO_TYPE | _io, .gpiofunc = _gpiofunc }


#endif /* __PINCTRL_K1_H__ */