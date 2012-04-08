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

#include <usb.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>

#include "dfu.h"
#include "stm32mem.h"

#define STM32_CMD_GETCOMMANDS		0x00
#define STM32_CMD_SETADDRESSPOINTER	0x21
#define STM32_CMD_ERASE			0x41

/* In STM32 each alternate setting is a different target memory. 
 * These are indicated by undocumented contents of the interface string
 * descriptors:
 * 	'@Internal Flash   /0x8000000/8*001Ka,56*001Kg'
 * This means "Internal Flash" starting at 0x8000000.
 * 8 * 1KB read-only, followed by 56 * 1KB read/erase/write
 */

int stm32mem_get_target(dfu_device *dev, int id, struct mem_target *t)
{
	char buf[128];
	uint32_t base;
	int n;
	char *s;

	if(id >= dev->num_altsetting)
		return 1;

	t->id = id;

	if(usb_get_string_simple(dev->h, dev->ifdesc[id].iInterface, 
				buf, sizeof(buf)) <= 0) 
		return 2;

	if(sscanf(buf, "@%[^/]/%i/%n", t->name, (int*)&t->base, &n) < 2)
		return 3;

	t->num_sectors = 0;
	/* Count sectors */
	for(s = buf + n - 1; s; s = strchr(s + 1, ',')) 
		t->num_sectors += atoi(s + 1);

	return 0;
}

int stm32mem_get_sector(dfu_device *dev, int id, int index, 
			struct mem_sector *sect)
{
	static int oldid = -1;
	static char buf[128];
	int n, count = 0;
	uint32_t base;
	char *s;

	if(oldid != id) {
		if(usb_get_string_simple(dev->h, dev->ifdesc[id].iInterface, 
					buf, sizeof(buf)) <= 0) 
			return 1;
		oldid = id;
	}

	if(sscanf(buf, "@%*[^/]/%i/%n", (int*)&base, &n) < 1)
		return 2;

	/* Count sectors */
	for(s = buf + n - 1; s; s = strchr(s + 1, ',')) {
		int num, size;
		char flag;
		sscanf(s + 1, "%d*%dK%c", &num, &size, &flag);
		size *= 1024;
		if((count <= index) && (index < count + num)) {
			sect->id = index;
			sect->addr = base + ((index - count) * size);
			sect->size = size;
			sect->mode = flag & 0x07;
			return 0;
		}
		base += num * size;
		count += num;
	}

	return 3;
}

static int stm32_download(dfu_device *dev, uint16_t wBlockNum, 
			void *data, int size)
{
	struct dfu_status status;
	int i;

	if((i = dfu_dnload(dev, wBlockNum, data, size)) < 0) return i;
	while(1) {
		if((i = dfu_getstatus(dev, &status)) < 0) return i;
		switch(status.bState) {
		    case STATE_DFU_DNBUSY:
			usleep(status.bwPollTimeout * 1000);
			break;
		    case STATE_DFU_DNLOAD_IDLE:
			return 0;
		    default:    
			return -1;
		}
	}
}

static int stm32mem_erase(dfu_device *dev, uint32_t addr)
{
        uint8_t request[5];

        request[0] = STM32_CMD_ERASE;
        memcpy(request+1, &addr, sizeof(addr));

        return stm32_download(dev, 0, request, sizeof(request));
}

static int stm32mem_write(dfu_device *dev, void *data, int size)
{
        return stm32_download(dev, 2, data, size);
}

struct stm32mem_flash {
	dfu_device *dev;
	int id;
	struct mem_target target;
	struct mem_sector *sector;
	uint8_t **sectbuf;
	int marked_sectors;
};

void *stm32mem_flash_prepare(dfu_device *dev, int id)
{
	struct stm32mem_flash *h = calloc(1, sizeof(*h));
	int i;

	h->dev = dev;
	h->id = id;

	stm32mem_get_target(dev, id, &h->target);
	h->sector = calloc(h->target.num_sectors, sizeof(*h->sector));
	h->sectbuf = calloc(h->target.num_sectors, sizeof(uint8_t*));
	for(i = 0; i < h->target.num_sectors; i++) 
		stm32mem_get_sector(dev, id, i, &h->sector[i]);

	return h;
}

int stm32mem_flash_write(void *hh, uint32_t addr, const void *buf, int len)
{
	struct stm32mem_flash *h = hh;
	int i;

	/* Find sector */
	for(i = 0; i < h->target.num_sectors; i++) {
		if((addr >= h->sector[i].addr) && 
		   (addr < (h->sector[i].addr + h->sector[i].size)))
			break;
	}

	if(i == h->target.num_sectors) /* No sector found */
		return -1;

	while(len) {
		int s = MIN(len, h->sector[i].addr + h->sector[i].size - addr);
		if(h->sectbuf[i] == NULL) {
			/* Allocate sector buffer if needed. */
			h->sectbuf[i] = malloc(h->sector[i].size);
			memset(h->sectbuf[i], 0xFF, h->sector[i].size);
			h->marked_sectors++;
		}
		memcpy(h->sectbuf[i] + (addr - h->sector[i].addr), buf, s);
		addr += s;
		buf += s;
		len -= s;
		i++;
	}

	return 0;
}

void stm32mem_flash_cancel(void *hh)
{
	struct stm32mem_flash *h = hh;
	int i;

	for(i = 0; i < h->target.num_sectors; i++) 
		if(h->sectbuf[i]) 
			free(h->sectbuf[i]);
	free(h->sectbuf);
	free(h->sector);
	free(h);
}

int stm32mem_flash_commit(void *hh, void (*progress)(double prog, void *data),
			void *data)
{
	struct stm32mem_flash *h = hh;
	int i, j;

	/* Not sure about this one... 
	 * How do we specify which interface to set?
	 */
	usb_set_altinterface(h->dev->h, h->id);

	/* Iterate over sectors eraseing and programming */
	for(i = 0, j = 0; i < h->target.num_sectors; i++)
		if(h->sectbuf[i]) {
			//printf("Programming memory at 0x%08X\n", h->sector[i].addr);
			stm32mem_erase(h->dev, h->sector[i].addr);
			stm32mem_write(h->dev, h->sectbuf[i], 
					h->sector[i].size);

			progress((double)++j / h->marked_sectors, data);
			free(h->sectbuf[i]);
		}

	free(h->sectbuf);
	free(h->sector);
	free(h);

	return 0;
}

