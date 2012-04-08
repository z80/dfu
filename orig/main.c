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
#include <gtk/gtk.h>
#include <usb.h>

#include <stdarg.h>

#include "dfu.h"
#include "stm32mem.h"

/* Defined in stm32dfu_glade.S for GtkBuilder xml data */
#ifndef G_OS_WIN32
extern void _glade_start, _glade_end;
#   define glade_start _glade_start
#   define glade_end _glade_end
#else
extern void glade_start, glade_end;
#endif

static GtkListStore *dev_list, *target_list;
static dfu_device *current_dev;

void gtk_builder_get_objects_va(GtkBuilder *builder, ...)
{
	va_list va;
	const gchar *name;
	GObject **obj;

	va_start(va, builder);
	while(name = va_arg(va, gchar *)) {
		obj = va_arg(va, GObject **);
		*obj = gtk_builder_get_object(builder, name);
	}
	va_end(va);
}

/* The following two functions update the device list shown in the GUI.
 * If we can count on the struct usb_device pointer being unchanged for the 
 * same device * between enumerations this can be made simpler, but it may 
 * not be safe... */
static void 
dfu_dev_found(struct usb_device *dev, uint8_t config, uint16_t iface)
{
	GtkTreeIter iter;

	/* Iterate over list looking for a match */
	if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(dev_list), &iter)) do {
		gchar *bus, *path;
		gtk_tree_model_get(GTK_TREE_MODEL(dev_list), &iter, 6, &bus, 
					7, &path, -1);
		if(g_str_equal(bus, dev->bus->dirname) && 
		   g_str_equal(path, dev->filename)) {
			/* Update device pointer, and mark as clean. */
			gtk_list_store_set(dev_list, &iter, 0, dev, 
						5, FALSE, -1);
			return;
		}
	} while(gtk_tree_model_iter_next(GTK_TREE_MODEL(dev_list), &iter));

	/* Device isn't in our list, so add it */
	char manu[40], prod[40];
	usb_dev_handle *h = usb_open(dev);
	gtk_list_store_append(dev_list, &iter);
	usb_get_string_simple(h, dev->descriptor.iManufacturer, manu, 40);
	usb_get_string_simple(h, dev->descriptor.iProduct, prod, 40);
	gtk_list_store_set(dev_list, &iter, 
				0, dev,
				1, config, 2, iface,
				3, manu, 4, prod, 
				6, dev->bus->dirname, 7, dev->filename, -1);
	usb_close(h);
}

static gboolean dev_update(gpointer data)
{
	GtkTreeIter iter;

	/* Mark all nodes as dirty, callback will clean them. */
	if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(dev_list), &iter)) do {
		gtk_list_store_set(dev_list, &iter, 5, TRUE, -1);
	} while(gtk_tree_model_iter_next(GTK_TREE_MODEL(dev_list), &iter));

	if(!dfu_find_devs(dfu_dev_found))
		return TRUE;
	
	/* Delete any dirty nodes */
	if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(dev_list), &iter)) do {
		gboolean dirty;
		gtk_tree_model_get(GTK_TREE_MODEL(dev_list), &iter, 
					5, &dirty, -1);
		if(dirty) {
			if(gtk_list_store_remove(dev_list, &iter)) 
				continue;
			else
				break;
		}
	} while(gtk_tree_model_iter_next(GTK_TREE_MODEL(dev_list), &iter));

	/* Try select something in our ComboBox */
	GtkComboBox *dev_select = GTK_COMBO_BOX(gtk_builder_get_object(
					GTK_BUILDER(data), "dev_select"));
	if((gtk_combo_box_get_active(dev_select) == -1) &&
	   (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(dev_list), &iter))) 
		gtk_combo_box_set_active_iter(dev_select, &iter);

	return TRUE;
}

G_MODULE_EXPORT
void target_view_row_activated_cb(GtkTreeView *view, GtkTreePath *path,
				GtkTreeViewColumn *col, GtkBuilder *builder)
{
	GtkTreeIter iter;
	gint id, num;
	int i;
	GtkWindow *window;
	GtkDialog *sector_info;
	GtkListStore *sector_list;

	gtk_tree_model_get_iter(GTK_TREE_MODEL(target_list), &iter, path);
	gtk_tree_model_get(GTK_TREE_MODEL(target_list), &iter,
				0, &id, 3, &num, -1);

	gtk_builder_get_objects_va(builder, 
				"window", &window,
				"sector_info", &sector_info,
				"sector_list", &sector_list, NULL);

	gtk_list_store_clear(sector_list);
	for(i = 0; i < num; i++) {
		struct mem_sector sect;
		char start[11], end[11];
		stm32mem_get_sector(current_dev, id, i, &sect);


		sprintf(start, "0x%08X", sect.addr);
		sprintf(end, "0x%08X", sect.addr + sect.size - 1);
		gtk_list_store_append(sector_list, &iter);
		gtk_list_store_set(sector_list, &iter, 0, i,
				1, start, 2, end, 3, sect.size, 
				4, sect.mode & MEM_MODE_READ,
				5, sect.mode & MEM_MODE_WRITE,
				6, sect.mode & MEM_MODE_ERASE,
				-1);
	}

	/* Can't we set the transient window in Glade? */
	gtk_window_set_transient_for(GTK_WINDOW(sector_info), window);
	gtk_dialog_run(sector_info);
	gtk_widget_hide(GTK_WIDGET(sector_info));
}

static void target_list_update(void)
{
	GtkTreeIter iter;
	struct mem_target mem;
	int i;
	char sectstr[20];

	gtk_list_store_clear(target_list);
	if(!current_dev) 
		return;

	for(i = 0; stm32mem_get_target(current_dev, i, &mem); i++) {
		gtk_list_store_append(target_list, &iter);
		sprintf(sectstr, "%d sectors...", mem.num_sectors);
		gtk_list_store_set(target_list, &iter,
				0, mem.id, 1, mem.name, 
				2, sectstr, 3, mem.num_sectors, -1);
	}
}

G_MODULE_EXPORT
void attribute_toggles_update(GtkWidget *w, GtkBuilder *builder)
{
	uint8_t att = current_dev ? current_dev->dfudesc->bmAttributes : 0;
	GtkToggleButton *upload, *dnload, *manifest, *detach, *staccel;

	gtk_builder_get_objects_va(builder, 
				"dnload", &dnload, 
				"upload", &upload, 
				"detach", &detach, 
				"manifest", &manifest,
				"staccel", &staccel, NULL);

	gtk_toggle_button_set_active(dnload, att & USB_DFU_CAN_DOWNLOAD);
	gtk_toggle_button_set_active(upload, att & USB_DFU_CAN_UPLOAD);
	gtk_toggle_button_set_active(detach, att & USB_DFU_WILL_DETACH);
	gtk_toggle_button_set_active(manifest, att & USB_DFU_MANIFEST_TOLERANT);
	gtk_toggle_button_set_active(staccel, att & 0x80);
}

G_MODULE_EXPORT
void dev_select_changed_cb(GtkComboBox *dev_select, GtkBuilder *builder)
{
	GtkTreeIter iter;

	gboolean app_mode, dfu_mode;
	gchar pid[5], vid[5], ver[5];
	uint8_t att;

	GtkEntry *pidapp, *vidapp, *verapp;
	GtkEntry *piddfu, *viddfu, *verdfu;
	GtkWidget *enterdfu, *leavedfu, *dnload_frame;
	
	if(current_dev) {
		dfu_close(current_dev);
		current_dev = NULL;
	}

	if(gtk_combo_box_get_active_iter(dev_select, &iter)) {
		/* Selection is valid: get information from the USB device */
		struct usb_device *dev;
		gint config, iface;

		gtk_tree_model_get(GTK_TREE_MODEL(dev_list), &iter, 
					0, &dev, 1, &config, 2, &iface, -1);

		current_dev = dfu_open(dev, config, iface);

		att = current_dev->dfudesc->bmAttributes;
		app_mode = current_dev->ifdesc->bInterfaceProtocol == 1;
		dfu_mode = !app_mode;

		sprintf(vid, "%04X", dev->descriptor.idVendor);
		sprintf(pid, "%04X", dev->descriptor.idProduct);
		sprintf(ver, "%04X", current_dev->dfudesc->bcdDFUVersion);
	} else {
		/* Selection is invalid, clear GUI */
		dfu_mode = app_mode = FALSE;
		vid[0] = pid[0] = ver[0] = 0;
		att = 0;
	}

	gtk_builder_get_objects_va(builder, 
				"vidapp", &vidapp, 
				"pidapp", &pidapp, 
				"verapp", &verapp, 
				"viddfu", &viddfu, 
				"piddfu", &piddfu, 
				"verdfu", &verdfu, NULL);

	gtk_entry_set_text(vidapp, app_mode ? vid : "");
	gtk_entry_set_text(pidapp, app_mode ? pid : "");
	gtk_entry_set_text(verapp, app_mode ? ver : "");
	gtk_entry_set_text(viddfu, dfu_mode ? vid : "");
	gtk_entry_set_text(piddfu, dfu_mode ? pid : "");
	gtk_entry_set_text(verdfu, dfu_mode ? ver : "");

	attribute_toggles_update(NULL, builder);

	gtk_builder_get_objects_va(builder, 
				"enterdfu", &enterdfu, 
				"leavedfu", &leavedfu, 
				"dnload_frame", &dnload_frame, NULL);

	gtk_widget_set_sensitive(enterdfu, app_mode);
	gtk_widget_set_sensitive(leavedfu, dfu_mode);
	gtk_widget_set_sensitive(dnload_frame, 
				dfu_mode && (att & USB_DFU_CAN_DOWNLOAD));

	target_list_update();
}

G_MODULE_EXPORT
void enterdfu_cb(GtkWidget *w, GtkBuilder *builder)
{
	struct dfu_status status;

	dfu_detach(current_dev, 1000);
	dfu_getstatus(current_dev, &status);
	/* Just pray that it worked...
	 * We can't reliably read status in APP mode 
	 */
	dev_update(builder);
}

G_MODULE_EXPORT
void leavedfu_cb(GtkWidget *w, GtkBuilder *builder)
{
	struct dfu_status status;

	dfu_makeidle(current_dev);
	dfu_dnload(current_dev, 0, NULL, 0);
	do {
		dfu_getstatus(current_dev, &status);
		/* FIXME: process GTK+ events while we wait. */
		usleep(status.bwPollTimeout * 1000);
	} while(status.bState != STATE_DFU_MANIFEST);
	dev_update(builder);
}

G_MODULE_EXPORT
void dnload_check_enable_cb(gpointer *unused, GtkBuilder *builder)
{
	GtkWidget *buttons, *dnload_file;
	GtkNotebook *nb;
	GtkEntry *loadaddr;
	GtkTreeView *target_view;
	GtkTreePath *path;
	gchar *file;
	gboolean filecheck;

	gtk_builder_get_objects_va(builder,
				"dnload_buttons", &buttons,
				"dnload_file", &dnload_file,
				"loadaddr", &loadaddr,
				"target_view", &target_view,
				"dlnotebook", &nb,
				NULL);

	file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dnload_file));
	gtk_tree_view_get_cursor(target_view, &path, NULL);

	if(!hexfile_parse(file, NULL)) {
		gtk_notebook_set_current_page(nb, 1);
		filecheck = TRUE;
	} else {
		const gchar *addr; 
		gchar *tmp;
		gtk_notebook_set_current_page(nb, 0);
		addr = gtk_entry_get_text(loadaddr);
		strtol(addr, &tmp, 0);	  
		filecheck = (*addr != 0) && (*tmp == 0);
	}

	gtk_widget_set_sensitive(buttons, (file != NULL) && (path != NULL) && 
				filecheck);

	g_free(file);
	gtk_tree_path_free(path);
}

static void progress_cb(double progress, void *data)
{
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(data), progress); 

	while(gtk_events_pending())
		gtk_main_iteration_do(FALSE);
}

G_MODULE_EXPORT
void upgrade_clicked_cb(GtkButton *upgrade, GtkBuilder *builder)
{
	GtkWidget *dnload_file;
	GtkTreeView *target_view;
	GtkTreePath *path;
	GtkTreeIter iter;
	GtkEntry *loadaddr;
	GtkProgressBar *progress;
	GtkWidget *window;
	GtkNotebook *nb;
	gchar *file;
	uint32_t addr;
	gint id;
	void *flash;

	gtk_builder_get_objects_va(builder,
			"dnload_file", &dnload_file,
			"target_view", &target_view,
			"loadaddr", &loadaddr,
			"progressbar", &progress,
			"window", &window,
			"dlnotebook", &nb,
			NULL);

	file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dnload_file));

	gtk_tree_view_get_cursor(target_view, &path, NULL);
	g_return_if_fail(path != NULL);
	gtk_tree_model_get_iter(GTK_TREE_MODEL(target_list), &iter, path);
	gtk_tree_model_get(GTK_TREE_MODEL(target_list), &iter, 0, &id, -1);
	gtk_tree_path_free(path);

	flash = stm32mem_flash_prepare(current_dev, id);
	switch(gtk_notebook_get_current_page(nb)) {
	case 0: {
		uint32_t addr = strtol(gtk_entry_get_text(loadaddr), NULL, 0);
		gchar *buf;
		gsize len;
		g_file_get_contents(file, &buf, &len, NULL);
		g_return_if_fail(buf != NULL);
		stm32mem_flash_write(flash, addr, buf, len);
		}
	case 1:
		hexfile_parse(file, stm32mem_flash_write, flash);
	}

	gtk_widget_set_sensitive(window, FALSE);
	stm32mem_flash_commit(flash, progress_cb, progress);
	gtk_widget_set_sensitive(window, TRUE);

	g_free(file);
}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	usb_init();

	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_string(builder, &glade_start, 
			(guint)(&glade_end - &glade_start), NULL);
	gtk_builder_connect_signals(builder, builder);
	gtk_builder_get_objects_va(builder,
				"dev_list", &dev_list,
				"target_list", &target_list,
				NULL);

	dev_update(builder);
	g_timeout_add(1000, dev_update, builder);

	gtk_main();

	return 0;
}

G_MODULE_EXPORT
void help_about(void)
{
	const gchar *authors[] = 
		{"Gareth McMullin <gareth@blacksphere.co.nz>", NULL};

        gtk_show_about_dialog(NULL,
                "program_name", PACKAGE_NAME,
                "version", VERSION,
                "comments", "A USB DFU Host utility for programming STM32 Microcontrollers",
                "copyright", "Copyright (C) 2010  Black Sphere Technologies",
                "license", 
                        "This program is free software: you can redistribute it and/or modify\n"
                        "it under the terms of the GNU General Public License as published by\n"
                        "the Free Software Foundation, either version 3 of the License, or\n"
                        "(at your option) any later version.\n\n"

                        "This program is distributed in the hope that it will be useful,\n"
                        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                        "GNU General Public License for more details.\n\n"

                        "You should have received a copy of the GNU General Public License\n"
                        "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n",
                "website", "http://sourceforge.net/projects/stm32dfu",
		"authors", authors,
                NULL);
}

