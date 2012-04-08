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

/*
 * Some of this is not my work.  I don't know who the original author is.
 * If you're the origial author please contact me for credit.
 */

#include <usb.h>
#include <stdint.h>

#include "dfu.h"

#define DFU_DETACH_TIMEOUT 1000
#define USB_DEFAULT_TIMEOUT 1000

static void dfu_find_ifaces( void * context, struct usb_device *dev, dfu_enum_func callback)
{
	int i, j;
	struct usb_config_descriptor *config;
	struct usb_interface_descriptor *iface;

	for(i = 0; i < dev->descriptor.bNumConfigurations; i++) {
		config = &dev->config[i];

		for(j = 0; j < config->bNumInterfaces; j++) {
			iface = &config->interface[j].altsetting[0];
			if((iface->bInterfaceClass == 0xFE) &&
			   (iface->bInterfaceSubClass = 0x01)) {
				callback( context, dev, i, j );
			}
		}
	}
}

int dfu_find_devs( void * context, dfu_enum_func callback)
{
	struct usb_bus *bus;
	struct usb_device *dev;

	int changes = 0;

	changes += usb_find_busses();
	changes += usb_find_devices();

	if(!changes) return 0;

	for(bus = usb_get_busses(); bus; bus = bus->next) 
		for(dev = bus->devices; dev; dev = dev->next) 
			dfu_find_ifaces( context, dev, callback );

	return changes;
}

dfu_device *dfu_open(struct usb_device *dev, uint8_t conf, uint16_t iface)
{
	dfu_device *d = calloc(1, sizeof(*d));
	d->dev = dev;
	d->conf = conf;
	d->iface = iface;

	d->ifdesc = d->dev->config[conf].interface[d->iface]
				.altsetting;
	d->num_altsetting = d->dev->config[conf].interface[d->iface]
				.num_altsetting;
	d->dfudesc = (void*)d->ifdesc->extra;

	d->h = usb_open(dev);
	/* FIXME: Set configuration and claim interface should happen, but
	 * don't seem to work.
	 */
	//usb_set_configuration(d->h, dev->config[conf].bConfigurationValue);
	//usb_claim_interface(d->h, iface);

	return d;
}

void dfu_close(dfu_device *d)
{
	if(d->priv_destroy)
		d->priv_destroy(d->priv);
	//usb_release_interface(d->h, d->iface);
	usb_close(d->h);
	free(d);
}

int dfu_detach(dfu_device *dev, uint16_t wTimeout)
{
	return usb_control_msg(dev->h,
			USB_ENDPOINT_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
			DFU_DETACH, wTimeout, dev->iface, NULL, 0, 
			USB_DEFAULT_TIMEOUT);
}

int dfu_dnload(dfu_device *dev, uint16_t wBlockNum, void *data, uint16_t size)
{
	return usb_control_msg(dev->h,
			USB_ENDPOINT_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
			DFU_DNLOAD, wBlockNum, dev->iface, data, size, 
			USB_DEFAULT_TIMEOUT);
}

int dfu_upload(dfu_device *dev, uint16_t wBlockNum, void *data, uint16_t size)
{
	return usb_control_msg(dev->h,
			USB_ENDPOINT_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
			DFU_DNLOAD, wBlockNum, dev->iface, data, size, 
			USB_DEFAULT_TIMEOUT);
}

int dfu_getstatus(dfu_device *dev, struct dfu_status *status)
{
	uint8_t buf[6];
	int i;

	/* The packed structure doesn't work correctly on Windows.
	 * We need to unpack the buffer ourselves. 
	 */
	i = usb_control_msg(dev->h,
			USB_ENDPOINT_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
			DFU_GETSTATUS, 0, dev->iface, buf, 
			sizeof(buf), USB_DEFAULT_TIMEOUT);

	status->bStatus = buf[0];
	status->bwPollTimeout = buf[1] | (buf[2] << 8) | (buf[3] << 16);
	status->bState = buf[4];
	status->iString = buf[5];

	return i;
}

int dfu_clrstatus(dfu_device *dev)
{
	return usb_control_msg(dev->h, 
			USB_ENDPOINT_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
			DFU_CLRSTATUS, 0, dev->iface, NULL, 0, 
			USB_DEFAULT_TIMEOUT);
}

int dfu_getstate(dfu_device *dev)
{
	int i;
	uint8_t state;
	do {
		i = usb_control_msg(dev->h, 
			USB_ENDPOINT_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
			DFU_GETSTATE, 0, dev->iface, &state, 1, 
			USB_DEFAULT_TIMEOUT);
	} while(i == 0);

	if(i > 0) 
		return state;
	else
		return i;
}

int dfu_abort(dfu_device *dev)
{
	return usb_control_msg(dev->h, 
			USB_ENDPOINT_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
			DFU_ABORT, 0, dev->iface, NULL, 0, USB_DEFAULT_TIMEOUT);
}


int dfu_makeidle(dfu_device *dev)
{
	int i;
	struct dfu_status status;

	for(i = 0; i < 3; i++) {
		if(dfu_getstatus(dev, &status) < 0) {
			dfu_clrstatus(dev);
			continue;
		}

		i--;
		
		switch(status.bState) {
		    case STATE_DFU_IDLE:
			return 0;

		    case STATE_DFU_DNLOAD_SYNC:
		    case STATE_DFU_DNLOAD_IDLE:
		    case STATE_DFU_MANIFEST_SYNC:
		    case STATE_DFU_UPLOAD_IDLE:
		    case STATE_DFU_DNBUSY:
		    case STATE_DFU_MANIFEST:
			dfu_abort(dev);
			continue;

		    case STATE_DFU_ERROR:
			dfu_clrstatus(dev);
			continue;

		    case STATE_APP_IDLE:
			dfu_detach(dev, DFU_DETACH_TIMEOUT);
			continue;

		    case STATE_APP_DETACH:
		    case STATE_DFU_MANIFEST_WAIT_RESET:
			usb_reset(dev->h);
			return -1;

		    default:
			return -1;
		}

	}

	return -1;
}

