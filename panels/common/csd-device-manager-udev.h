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

#ifndef __CSD_UDEV_DEVICE_MANAGER_H__
#define __CSD_UDEV_DEVICE_MANAGER_H__

#include "csd-device-manager.h"

G_BEGIN_DECLS

#define CSD_TYPE_UDEV_DEVICE_MANAGER         (csd_udev_device_manager_get_type ())
#define CSD_UDEV_DEVICE_MANAGER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), CSD_TYPE_UDEV_DEVICE_MANAGER, CsdUdevDeviceManager))
#define CSD_UDEV_DEVICE_MANAGER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), CSD_TYPE_UDEV_DEVICE_MANAGER, CsdUdevDeviceManagerClass))
#define CSD_IS_UDEV_DEVICE_MANAGER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), CSD_TYPE_UDEV_DEVICE_MANAGER))
#define CSD_IS_UDEV_DEVICE_MANAGER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), CSD_TYPE_UDEV_DEVICE_MANAGER))
#define CSD_UDEV_DEVICE_MANAGER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), CSD_TYPE_UDEV_DEVICE_MANAGER, CsdUdevDeviceManagerClass))

typedef struct _CsdUdevDeviceManager CsdUdevDeviceManager;
typedef struct _CsdUdevDeviceManagerClass CsdUdevDeviceManagerClass;

GType csd_udev_device_manager_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __CSD_UDEV_DEVICE_MANAGER_H__ */
