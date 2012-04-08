/*
 * This file is part of the stm32dfu project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __STM32MEM_H
#define __STM32MEM_H

#include <usb.h>

struct mem_sector {
	int id;
	uint32_t addr;
	uint32_t size;
	uint8_t mode;
#define MEM_MODE_READ	0x01
#define MEM_MODE_WRITE	0x02
#define MEM_MODE_ERASE	0x04
};

struct mem_target {
	int id;
	char name[40];
	uint32_t base;
	int num_sectors;
};

/* Enumerate memory regions */
int stm32mem_get_target(dfu_device *dev, int id, struct mem_target *target);
int stm32mem_get_sector(dfu_device *dev, int id, int index, 
			struct mem_sector *sect);

/* Memory programming routines */
void *stm32mem_flash_prepare(dfu_device *dev, int id);
int stm32mem_flash_write(void *hh, uint32_t addr, const void *buf, int len);
void stm32mem_flash_cancel(void *hh);
int stm32mem_flash_commit(void *hh, void (*progress)(double prog, void *data),
			void *data);

#endif

