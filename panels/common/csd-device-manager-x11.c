/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2015 Red Hat
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Author: Carlos Garnacho <carlosg@gnome.org>
 */

#include "config.h"

#include <string.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include "csd-input-helper.h"
#include "csd-device-manager-x11.h"

struct _CsdX11DeviceManager
{
	CsdDeviceManager parent_instance;
	GdkDeviceManager *device_manager;
	GHashTable *devices;
	GHashTable *gdk_devices;
};

struct _CsdX11DeviceManagerClass
{
	CsdDeviceManagerClass parent_class;
};

CsdDevice  * csd_x11_device_manager_lookup_gdk_device (CsdDeviceManager *manager,
						       GdkDevice	*gdk_device);

G_DEFINE_TYPE (CsdX11DeviceManager, csd_x11_device_manager, CSD_TYPE_DEVICE_MANAGER)

static CsdDeviceType
device_get_device_type (GdkDevice *gdk_device)
{
	GdkInputSource source;

	source = gdk_device_get_source (gdk_device);

	switch (source) {
	case GDK_SOURCE_MOUSE:
	case GDK_SOURCE_TRACKPOINT:
		return CSD_DEVICE_TYPE_MOUSE;
	case GDK_SOURCE_PEN:
	case GDK_SOURCE_ERASER:
	case GDK_SOURCE_CURSOR:
		if (strstr (gdk_device_get_name (gdk_device), "pad"))
			return CSD_DEVICE_TYPE_TABLET | CSD_DEVICE_TYPE_PAD;
		return CSD_DEVICE_TYPE_TABLET;
	case GDK_SOURCE_KEYBOARD:
		return CSD_DEVICE_TYPE_KEYBOARD;
	case GDK_SOURCE_TOUCHSCREEN:
		return CSD_DEVICE_TYPE_TOUCHSCREEN;
	case GDK_SOURCE_TOUCHPAD:
		return CSD_DEVICE_TYPE_TOUCHPAD;
	case GDK_SOURCE_TABLET_PAD:
		return CSD_DEVICE_TYPE_TABLET | CSD_DEVICE_TYPE_PAD;
	}

	g_warning ("Unhandled input source %d\n", source);
	return 0;
}

static CsdDevice *
create_device (GdkDevice   *gdk_device,
	       const gchar *device_file)
{
	guint width, height;
	CsdDevice *device;
	gint id;

	id = gdk_x11_device_get_id (gdk_device);
	xdevice_get_dimensions (id, &width, &height);

	device = g_object_new (CSD_TYPE_DEVICE,
			       "name", gdk_device_get_name (gdk_device),
			       "device-file", device_file,
			       "type", device_get_device_type (gdk_device),
			       "vendor-id", gdk_device_get_vendor_id (gdk_device),
			       "product-id", gdk_device_get_product_id (gdk_device),
			       "width", width,
			       "height", height,
			       NULL);
	return device;
}

static void
add_device (CsdX11DeviceManager *manager,
	    GdkDevice		*gdk_device)
{
	gchar *device_file;
	CsdDevice *device;
	gint id;

	if (gdk_device_get_device_type (gdk_device) == GDK_DEVICE_TYPE_MASTER)
		return;

	id = gdk_x11_device_get_id (gdk_device);
	device_file = xdevice_get_device_node (id);

	if (!device_file)
		return;

	device = g_hash_table_lookup (manager->devices, gdk_device);

	if (device) {
		g_signal_emit_by_name (manager, "device-changed", device);
	} else {
		device = create_device (gdk_device, device_file);
		g_hash_table_insert (manager->devices, gdk_device, device);
		g_signal_emit_by_name (manager, "device-added", device);
	}
}

static void
remove_device (CsdX11DeviceManager *manager,
	       GdkDevice	   *gdk_device)
{
	CsdDevice *device;

	device = g_hash_table_lookup (manager->devices, gdk_device);

	if (device) {
		g_object_ref (device);
		g_signal_emit_by_name (manager, "device-removed", device);
		g_object_unref (device);

                g_hash_table_remove (manager->devices, gdk_device);
	}
}

static void
init_devices (CsdX11DeviceManager *manager,
	      GdkDeviceType	   device_type)
{
	GList *devices, *l;

	devices = gdk_device_manager_list_devices (manager->device_manager,
						   device_type);

	for (l = devices; l; l = l->next)
		add_device (manager, l->data);

	g_list_free (devices);
}

static void
csd_x11_device_manager_init (CsdX11DeviceManager *manager)
{
	GdkDisplay *display;

	manager->devices = g_hash_table_new_full (NULL, NULL, NULL,
						  (GDestroyNotify) g_object_unref);

	display = gdk_display_get_default ();
	manager->device_manager = gdk_display_get_device_manager (display);

	g_signal_connect_swapped (manager->device_manager, "device-added",
				  G_CALLBACK (add_device), manager);
	g_signal_connect_swapped (manager->device_manager, "device-removed",
				  G_CALLBACK (remove_device), manager);

	init_devices (manager, GDK_DEVICE_TYPE_SLAVE);
	init_devices (manager, GDK_DEVICE_TYPE_FLOATING);
}

static GList *
csd_x11_device_manager_list_devices (CsdDeviceManager *manager,
				     CsdDeviceType     type)
{
	CsdX11DeviceManager *manager_x11 = CSD_X11_DEVICE_MANAGER (manager);
	CsdDeviceType device_type;
	GList *devices = NULL;
	GHashTableIter iter;
	CsdDevice *device;

	g_hash_table_iter_init (&iter, manager_x11->devices);

	while (g_hash_table_iter_next (&iter, NULL, (gpointer *) &device)) {
		device_type = csd_device_get_device_type (device);

		if ((device_type & type) == type)
			devices = g_list_prepend (devices, device);
	}

	return devices;
}

static void
csd_x11_device_manager_class_init (CsdX11DeviceManagerClass *klass)
{
	CsdDeviceManagerClass *manager_class = CSD_DEVICE_MANAGER_CLASS (klass);

	manager_class->list_devices = csd_x11_device_manager_list_devices;
	manager_class->lookup_device = csd_x11_device_manager_lookup_gdk_device;
}

CsdDevice *
csd_x11_device_manager_lookup_gdk_device (CsdDeviceManager *manager,
					  GdkDevice	   *gdk_device)
{
	CsdX11DeviceManager *manager_x11 = CSD_X11_DEVICE_MANAGER (manager);

	return g_hash_table_lookup (manager_x11->devices, gdk_device);
}
