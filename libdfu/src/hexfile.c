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
#include <stdio.h>
#include <string.h>

#include "hexfile.h"

static char * hexify(char *hex, const unsigned char *buf, int size)
{
	hex[0] = 0;
	while(size--)
		sprintf(hex, "%s%02hhx", hex, *buf++);
	return hex;
}

static char * unhexify(unsigned char *buf, const char *hex, int size)
{
	unsigned char *ret = buf;
	unsigned int tmp;

	while(size--) {
		sscanf(hex, "%02x", &tmp);
		*buf++ = tmp;
		hex += 2;
	}
	return ret;
}

int hexfile_parse( const char *filename, void (*record_cb)(void *context, 
				   uint32_t addr, const uint8_t *data, int len),
			       void *context )
{
	FILE *hex = fopen(filename, "r");
	uint8_t buf[256];
	int len, csum, i;
	uint32_t addrhi = 0;

	if(!hex) 
		return -1;

	while(!feof(hex)) {
		fgets(buf, sizeof(buf), hex);
		if(strlen(buf) == 0) continue;
		if(buf[0] != ':') goto ditch;
		if(sscanf(buf+1, "%02x", &len) != 1) goto ditch;
		unhexify(buf, buf+1, len+5);
		for(csum = 0, i = 0; i < len+5; i++) 
			csum += buf[i];
		if(csum & 0xFF) goto ditch;
		/* Line is ok */
		switch(buf[3]) { /* switch on record type */
		    case 0x00: /* Data record */
			if(!record_cb) continue;
			record_cb(context, addrhi + (buf[1] << 8) + buf[2], 
					buf+4, len);
			break;
		    case 0x04: /* High address record */
			addrhi = (buf[4] << 24) + (buf[5] << 16);
			break;
		    case 0x05: /* Entry record, ignore */
		    case 0x01: /* End of file record */
			break;
		    default:
			goto ditch;
		}
	}

	fclose(hex);
	return 0;
ditch:
	fclose(hex);
	return -2;
}

