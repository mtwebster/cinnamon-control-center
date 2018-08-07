/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2014 Red Hat
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

#include "csd-device-manager-x11.h"
#include "csd-device-manager-udev.h"
#include "csd-common-enums.h"
#include "cinnamon-settings-bus.h"

typedef struct _CsdDevicePrivate CsdDevicePrivate;
typedef struct _CsdDeviceManagerPrivate CsdDeviceManagerPrivate;

struct _CsdDevicePrivate
{
	gchar *name;
	gchar *device_file;
	gchar *vendor_id;
	gchar *product_id;
	CsdDeviceType type;
	guint width;
	guint height;
};

enum {
	PROP_NAME = 1,
	PROP_DEVICE_FILE,
	PROP_VENDOR_ID,
	PROP_PRODUCT_ID,
	PROP_TYPE,
	PROP_WIDTH,
	PROP_HEIGHT
};

enum {
	DEVICE_ADDED,
	DEVICE_REMOVED,
	DEVICE_CHANGED,
	N_SIGNALS
};

static guint signals[N_SIGNALS] = { 0 };

G_DEFINE_TYPE_WITH_PRIVATE (CsdDevice, csd_device, G_TYPE_OBJECT)
G_DEFINE_TYPE (CsdDeviceManager, csd_device_manager, G_TYPE_OBJECT)

static void
csd_device_init (CsdDevice *device)
{
}

static void
csd_device_set_property (GObject      *object,
			 guint	       prop_id,
			 const GValue *value,
			 GParamSpec   *pspec)
{
	CsdDevicePrivate *priv;

	priv = csd_device_get_instance_private (CSD_DEVICE (object));

	switch (prop_id) {
	case PROP_NAME:
		priv->name = g_value_dup_string (value);
		break;
	case PROP_DEVICE_FILE:
		priv->device_file = g_value_dup_string (value);
		break;
	case PROP_VENDOR_ID:
		priv->vendor_id = g_value_dup_string (value);
		break;
	case PROP_PRODUCT_ID:
		priv->product_id = g_value_dup_string (value);
		break;
	case PROP_TYPE:
		priv->type = g_value_get_flags (value);
		break;
	case PROP_WIDTH:
		priv->width = g_value_get_uint (value);
		break;
	case PROP_HEIGHT:
		priv->height = g_value_get_uint (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
csd_device_get_property (GObject    *object,
			 guint	     prop_id,
			 GValue	    *value,
			 GParamSpec *pspec)
{
	CsdDevicePrivate *priv;

	priv = csd_device_get_instance_private (CSD_DEVICE (object));

	switch (prop_id) {
	case PROP_NAME:
		g_value_set_string (value, priv->name);
		break;
	case PROP_DEVICE_FILE:
		g_value_set_string (value, priv->device_file);
		break;
	case PROP_VENDOR_ID:
		g_value_set_string (value, priv->vendor_id);
		break;
	case PROP_PRODUCT_ID:
		g_value_set_string (value, priv->product_id);
		break;
	case PROP_TYPE:
		g_value_set_flags (value, priv->type);
		break;
	case PROP_WIDTH:
		g_value_set_uint (value, priv->width);
		break;
	case PROP_HEIGHT:
		g_value_set_uint (value, priv->height);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
csd_device_finalize (GObject *object)
{
	CsdDevicePrivate *priv;

	priv = csd_device_get_instance_private (CSD_DEVICE (object));

	g_free (priv->name);
	g_free (priv->vendor_id);
	g_free (priv->product_id);
	g_free (priv->device_file);

	G_OBJECT_CLASS (csd_device_parent_class)->finalize (object);
}

static void
csd_device_class_init (CsdDeviceClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = csd_device_set_property;
	object_class->get_property = csd_device_get_property;
	object_class->finalize = csd_device_finalize;

	g_object_class_install_property (object_class,
					 PROP_NAME,
					 g_param_spec_string ("name",
							      "Name",
							      "Name",
							      NULL,
							      G_PARAM_READWRITE |
							      G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (object_class,
					 PROP_DEVICE_FILE,
					 g_param_spec_string ("device-file",
							      "Device file",
							      "Device file",
							      NULL,
							      G_PARAM_READWRITE |
							      G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (object_class,
					 PROP_VENDOR_ID,
					 g_param_spec_string ("vendor-id",
							      "Vendor ID",
							      "Vendor ID",
							      NULL,
							      G_PARAM_READWRITE |
							      G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (object_class,
					 PROP_PRODUCT_ID,
					 g_param_spec_string ("product-id",
							      "Product ID",
							      "Product ID",
							      NULL,
							      G_PARAM_READWRITE |
							      G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (object_class,
					 PROP_TYPE,
					 g_param_spec_flags ("type",
							     "Device type",
							     "Device type",
							     CSD_TYPE_DEVICE_TYPE, 0,
							     G_PARAM_READWRITE |
							     G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (object_class,
					 PROP_WIDTH,
					 g_param_spec_uint ("width",
							    "Width",
							    "Width",
							    0, G_MAXUINT, 0,
							    G_PARAM_READWRITE |
							    G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (object_class,
					 PROP_HEIGHT,
					 g_param_spec_uint ("height",
							    "Height",
							    "Height",
							    0, G_MAXUINT, 0,
							    G_PARAM_READWRITE |
							    G_PARAM_CONSTRUCT_ONLY));
}

static void
csd_device_manager_class_init (CsdDeviceManagerClass *klass)
{
	signals[DEVICE_ADDED] =
		g_signal_new ("device-added",
			      CSD_TYPE_DEVICE_MANAGER,
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (CsdDeviceManagerClass, device_added),
			      NULL, NULL, NULL,
			      G_TYPE_NONE, 1,
			      CSD_TYPE_DEVICE | G_SIGNAL_TYPE_STATIC_SCOPE);

	signals[DEVICE_REMOVED] =
		g_signal_new ("device-removed",
			      CSD_TYPE_DEVICE_MANAGER,
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (CsdDeviceManagerClass, device_removed),
			      NULL, NULL, NULL,
			      G_TYPE_NONE, 1,
			      CSD_TYPE_DEVICE | G_SIGNAL_TYPE_STATIC_SCOPE);

	signals[DEVICE_CHANGED] =
		g_signal_new ("device-changed",
			      CSD_TYPE_DEVICE_MANAGER,
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (CsdDeviceManagerClass, device_changed),
			      NULL, NULL, NULL,
			      G_TYPE_NONE, 1,
			      CSD_TYPE_DEVICE | G_SIGNAL_TYPE_STATIC_SCOPE);
}

static void
csd_device_manager_init (CsdDeviceManager *manager)
{
}

CsdDeviceManager *
csd_device_manager_get (void)
{
	CsdDeviceManager *manager;
	GdkScreen *screen;

	screen = gdk_screen_get_default ();
	g_return_val_if_fail (screen != NULL, NULL);

	manager = g_object_get_data (G_OBJECT (screen), "csd-device-manager-data");

	if (!manager) {
#ifdef HAVE_WAYLAND
		if (cinnamon_settings_is_wayland ()) {
			manager = g_object_new (CSD_TYPE_UDEV_DEVICE_MANAGER,
						NULL);
		} else
#endif /* HAVE_WAYLAND */
		{
			manager = g_object_new (CSD_TYPE_X11_DEVICE_MANAGER,
						NULL);
		}

		g_object_set_data_full (G_OBJECT (screen), "csd-device-manager-data",
					manager, (GDestroyNotify) g_object_unref);
	}

	return manager;
}

GList *
csd_device_manager_list_devices (CsdDeviceManager *manager,
				 CsdDeviceType	   type)
{
	g_return_val_if_fail (CSD_IS_DEVICE_MANAGER (manager), NULL);

	return CSD_DEVICE_MANAGER_GET_CLASS (manager)->list_devices (manager, type);
}

CsdDeviceType
csd_device_get_device_type (CsdDevice *device)
{
	CsdDevicePrivate *priv;

	g_return_val_if_fail (CSD_IS_DEVICE (device), 0);

	priv = csd_device_get_instance_private (device);

	return priv->type;
}

void
csd_device_get_device_ids (CsdDevice	*device,
			   const gchar **vendor,
			   const gchar **product)
{
	CsdDevicePrivate *priv;

	g_return_if_fail (CSD_IS_DEVICE (device));

	priv = csd_device_get_instance_private (device);

	if (vendor)
		*vendor = priv->vendor_id;
	if (product)
		*product = priv->product_id;
}

GSettings *
csd_device_get_settings (CsdDevice *device)
{
	const gchar *schema = NULL, *vendor, *product;
	GSettings *settings;
	CsdDeviceType type;
	gchar *path = NULL;

	g_return_val_if_fail (CSD_IS_DEVICE (device), NULL);

	type = csd_device_get_device_type (device);

	if (type & (CSD_DEVICE_TYPE_TOUCHSCREEN | CSD_DEVICE_TYPE_TABLET)) {
		csd_device_get_device_ids (device, &vendor, &product);

		if (type & CSD_DEVICE_TYPE_TOUCHSCREEN) {
			schema = "org.gnome.desktop.peripherals.touchscreen";
			path = g_strdup_printf ("/org/gnome/desktop/peripherals/touchscreens/%s:%s/",
						vendor, product);
		} else if (type & CSD_DEVICE_TYPE_TABLET) {
			schema = "org.gnome.desktop.peripherals.tablet";
			path = g_strdup_printf ("/org/gnome/desktop/peripherals/tablets/%s:%s/",
						vendor, product);
		}
	} else if (type & (CSD_DEVICE_TYPE_MOUSE | CSD_DEVICE_TYPE_TOUCHPAD)) {
		schema = "org.gnome.desktop.peripherals.mouse";
	} else if (type & CSD_DEVICE_TYPE_KEYBOARD) {
		schema = "org.gnome.desktop.peripherals.keyboard";
	} else {
		return NULL;
	}

	if (path) {
		settings = g_settings_new_with_path (schema, path);
		g_free (path);
	} else {
		settings = g_settings_new (schema);
	}

	return settings;
}

const gchar *
csd_device_get_name (CsdDevice *device)
{
	CsdDevicePrivate *priv;

	g_return_val_if_fail (CSD_IS_DEVICE (device), NULL);

	priv = csd_device_get_instance_private (device);

	return priv->name;
}

const gchar *
csd_device_get_device_file (CsdDevice *device)
{
	CsdDevicePrivate *priv;

	g_return_val_if_fail (CSD_IS_DEVICE (device), NULL);

	priv = csd_device_get_instance_private (device);

	return priv->device_file;
}

gboolean
csd_device_get_dimensions (CsdDevice *device,
			   guint     *width,
			   guint     *height)
{
	CsdDevicePrivate *priv;

	g_return_val_if_fail (CSD_IS_DEVICE (device), FALSE);

	priv = csd_device_get_instance_private (device);

	if (width)
		*width = priv->width;
	if (height)
		*height = priv->height;

	return priv->width > 0 && priv->height > 0;
}

CsdDevice *
csd_device_manager_lookup_gdk_device (CsdDeviceManager *manager,
				      GdkDevice	       *gdk_device)
{
	CsdDeviceManagerClass *klass;

	g_return_val_if_fail (CSD_IS_DEVICE_MANAGER (manager), NULL);
	g_return_val_if_fail (GDK_IS_DEVICE (gdk_device), NULL);

	klass = CSD_DEVICE_MANAGER_GET_CLASS (manager);
	if (!klass->lookup_device)
		return NULL;

	return klass->lookup_device (manager, gdk_device);
}
