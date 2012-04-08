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

#ifndef __DFU_H
#define __DFU_H

#include <usb.h>
#include <stdint.h>

enum dfu_req {
	DFU_DETACH,
	DFU_DNLOAD,
	DFU_UPLOAD,
	DFU_GETSTATUS,
	DFU_CLRSTATUS,
	DFU_GETSTATE,
	DFU_ABORT
};

enum dfu_status_e {
	DFU_STATUS_OK,
	DFU_STATUS_ERR_TARGET,
	DFU_STATUS_ERR_FILE,
	DFU_STATUS_ERR_WRITE,
	DFU_STATUS_ERR_ERASE,
	DFU_STATUS_ERR_CHECK_ERASED,
	DFU_STATUS_ERR_PROG,
	DFU_STATUS_ERR_VERIFY,
	DFU_STATUS_ERR_ADDRESS,
	DFU_STATUS_ERR_NOTDONE,
	DFU_STATUS_ERR_FIRMWARE,
	DFU_STATUS_ERR_VENDOR,
	DFU_STATUS_ERR_USBR,
	DFU_STATUS_ERR_POR,
	DFU_STATUS_ERR_UNKNOWN,
	DFU_STATUS_ERR_STALLEDPKT,
};

enum dfu_state {
	STATE_APP_IDLE,
	STATE_APP_DETACH,
	STATE_DFU_IDLE,
	STATE_DFU_DNLOAD_SYNC,
	STATE_DFU_DNBUSY,
	STATE_DFU_DNLOAD_IDLE,
	STATE_DFU_MANIFEST_SYNC,
	STATE_DFU_MANIFEST,
	STATE_DFU_MANIFEST_WAIT_RESET,
	STATE_DFU_UPLOAD_IDLE,
	STATE_DFU_ERROR,
};

#define USB_DT_DFU			0x21
struct usb_dfu_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bmAttributes;
#define USB_DFU_CAN_DOWNLOAD		0x01
#define USB_DFU_CAN_UPLOAD		0x02
#define USB_DFU_MANIFEST_TOLERANT	0x04
#define USB_DFU_WILL_DETACH		0x08

	uint16_t wDetachTimeout;
	uint16_t wTransferSize;
	uint16_t bcdDFUVersion;	
} __attribute__((packed));

struct dfu_status {
        uint8_t bStatus;
        uint32_t bwPollTimeout:24;
        uint8_t bState;
        uint8_t iString;
} __attribute__((packed));


typedef struct dfu_device {
	struct usb_device *dev;
	usb_dev_handle *h;
	uint8_t conf;
	uint16_t iface;
	int num_altsetting;
	struct usb_interface_descriptor *ifdesc;
	struct usb_dfu_descriptor *dfudesc;

	/* These fields are used by memory driver */
	void *priv;
	void (*priv_destroy)(void *);
} dfu_device;

typedef void (*dfu_enum_func)( void * context, struct usb_device *dev, 
		uint8_t config, uint16_t iface);

/* Enumerate DFU capable devs */
int dfu_find_devs( void * context, dfu_enum_func callback );

dfu_device *dfu_open(struct usb_device *dev, uint8_t config, 
			uint16_t iface);
void dfu_close(dfu_device *dev); 

int dfu_detach(dfu_device *dev, uint16_t wTimeout);
int dfu_dnload(dfu_device *dev, uint16_t wBlockNum, void *data, uint16_t size);
int dfu_upload(dfu_device *dev, uint16_t wBlockNum, void *data, uint16_t size);
int dfu_getstatus(dfu_device *dev, struct dfu_status *status);
int dfu_clrstatus(dfu_device *dev);
int dfu_getstate(dfu_device *dev);
int dfu_abort(dfu_device *dev);

int dfu_makeidle(dfu_device *dev);

#endif

