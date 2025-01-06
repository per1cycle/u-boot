// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2024, Kongyang Liu <seashell11234455@gmail.com>
 */

#include <asm/io.h>

int board_init(void)
{
	for(int i = 1; i < 16; i ++)
	{
		writel(0x1041, 0xd401e000 + i * 4);
	}
	return 0;
}
