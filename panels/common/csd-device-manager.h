/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2014 Carlos Garnacho <carlosg@gnome.org>
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
 */

#ifndef __CSD_DEVICE_MANAGER_H__
#define __CSD_DEVICE_MANAGER_H__

#include <gdk/gdk.h>
#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define CSD_TYPE_DEVICE		(csd_device_get_type ())
#define CSD_DEVICE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), CSD_TYPE_DEVICE, CsdDevice))
#define CSD_DEVICE_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), CSD_TYPE_DEVICE, CsdDeviceClass))
#define CSD_IS_DEVICE(o)	(G_TYPE_CHECK_INSTANCE_TYPE ((o), CSD_TYPE_DEVICE))
#define CSD_IS_DEVICE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), CSD_TYPE_DEVICE))
#define CSD_DEVICE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), CSD_TYPE_DEVICE, CsdDeviceClass))

#define CSD_TYPE_DEVICE_MANAGER		(csd_device_manager_get_type ())
#define CSD_DEVICE_MANAGER(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), CSD_TYPE_DEVICE_MANAGER, CsdDeviceManager))
#define CSD_DEVICE_MANAGER_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), CSD_TYPE_DEVICE_MANAGER, CsdDeviceManagerClass))
#define CSD_IS_DEVICE_MANAGER(o)	(G_TYPE_CHECK_INSTANCE_TYPE ((o), CSD_TYPE_DEVICE_MANAGER))
#define CSD_IS_DEVICE_MANAGER_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), CSD_TYPE_DEVICE_MANAGER))
#define CSD_DEVICE_MANAGER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), CSD_TYPE_DEVICE_MANAGER, CsdDeviceManagerClass))

typedef struct _CsdDevice CsdDevice;
typedef struct _CsdDeviceClass CsdDeviceClass;
typedef struct _CsdDeviceManager CsdDeviceManager;
typedef struct _CsdDeviceManagerClass CsdDeviceManagerClass;

typedef enum {
	CSD_DEVICE_TYPE_MOUSE	     = 1 << 0,
	CSD_DEVICE_TYPE_KEYBOARD     = 1 << 1,
	CSD_DEVICE_TYPE_TOUCHPAD     = 1 << 2,
	CSD_DEVICE_TYPE_TABLET	     = 1 << 3,
	CSD_DEVICE_TYPE_TOUCHSCREEN  = 1 << 4,
	CSD_DEVICE_TYPE_PAD          = 1 << 5
} CsdDeviceType;

struct _CsdDevice {
	GObject parent_instance;
};

struct _CsdDeviceClass {
	GObjectClass parent_class;
};

struct _CsdDeviceManager
{
	GObject parent_instance;
};

struct _CsdDeviceManagerClass
{
	GObjectClass parent_class;

	GList * (* list_devices) (CsdDeviceManager *manager,
				  CsdDeviceType	    type);

	void (* device_added)	(CsdDeviceManager *manager,
				 CsdDevice	  *device);
	void (* device_removed) (CsdDeviceManager *manager,
				 CsdDevice	  *device);
	void (* device_changed) (CsdDeviceManager *manager,
				 CsdDevice	  *device);

	CsdDevice * (* lookup_device) (CsdDeviceManager *manager,
				       GdkDevice	*gdk_device);
};

GType		   csd_device_get_type		      (void) G_GNUC_CONST;
GType		   csd_device_manager_get_type	      (void) G_GNUC_CONST;
CsdDeviceManager * csd_device_manager_get	      (void);
GList *		   csd_device_manager_list_devices    (CsdDeviceManager *manager,
						       CsdDeviceType	 type);

const gchar *	   csd_device_get_name	      (CsdDevice  *device);
CsdDeviceType	   csd_device_get_device_type (CsdDevice  *device);
void		   csd_device_get_device_ids  (CsdDevice    *device,
					       const gchar **vendor,
					       const gchar **product);
GSettings *	   csd_device_get_settings    (CsdDevice  *device);

const gchar *	   csd_device_get_device_file (CsdDevice  *device);
gboolean	   csd_device_get_dimensions  (CsdDevice  *device,
					       guint	  *width,
					       guint	  *height);

CsdDevice *	   csd_device_manager_lookup_gdk_device (CsdDeviceManager *manager,
							 GdkDevice	  *gdk_device);

G_END_DECLS

#endif /* __CSD_DEVICE_MANAGER_H__ */
