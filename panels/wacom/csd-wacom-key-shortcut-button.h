/*
 * csd-wacom-key-shortcut-button.h
 *
 * Copyright © 2013 Red Hat, Inc.
 *
 * Author: Joaquim Rocha <jrocha@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __CSD_WACOM_KEY_SHORTCUT_BUTTON_H__
#define __CSD_WACOM_KEY_SHORTCUT_BUTTON_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CSD_WACOM_TYPE_KEY_SHORTCUT_BUTTON            (csd_wacom_key_shortcut_button_get_type ())
#define CSD_WACOM_KEY_SHORTCUT_BUTTON(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CSD_WACOM_TYPE_KEY_SHORTCUT_BUTTON, CsdWacomKeyShortcutButton))
#define CSD_WACOM_IS_KEY_SHORTCUT_BUTTON(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CSD_WACOM_TYPE_KEY_SHORTCUT_BUTTON))
#define CSD_WACOM_KEY_SHORTCUT_BUTTON_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CSD_WACOM_TYPE_KEY_SHORTCUT_BUTTON, CsdWacomKeyShortcutButtonClass))
#define CSD_WACOM_IS_KEY_SHORTCUT_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CSD_WACOM_TYPE_KEY_SHORTCUT_BUTTON))
#define CSD_WACOM_KEY_SHORTCUT_BUTTON_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CSD_WACOM_TYPE_KEY_SHORTCUT_BUTTON, CsdWacomKeyShortcutButtonClass))

typedef struct _CsdWacomKeyShortcutButton        CsdWacomKeyShortcutButton;
typedef struct _CsdWacomKeyShortcutButtonClass   CsdWacomKeyShortcutButtonClass;
typedef struct _CsdWacomKeyShortcutButtonPrivate CsdWacomKeyShortcutButtonPrivate;

GType csd_wacom_key_shortcut_button_mode_type (void) G_GNUC_CONST;
#define CSD_WACOM_TYPE_KEY_SHORTCUT_BUTTON_MODE (csd_wacom_key_shortcut_button_mode_type())

typedef enum
{
  CSD_WACOM_KEY_SHORTCUT_BUTTON_MODE_OTHER,
  CSD_WACOM_KEY_SHORTCUT_BUTTON_MODE_ALL
} CsdWacomKeyShortcutButtonMode;

struct _CsdWacomKeyShortcutButton
{
  GtkButton parent_instance;

  /*< private >*/
  CsdWacomKeyShortcutButtonPrivate *priv;
};

struct _CsdWacomKeyShortcutButtonClass
{
  GtkButtonClass parent_class;

  void (* key_shortcut_edited)  (CsdWacomKeyShortcutButton *key_shortcut_button);

  void (* key_shortcut_cleared) (CsdWacomKeyShortcutButton *key_shortcut_button);
};

GType          csd_wacom_key_shortcut_button_get_type        (void) G_GNUC_CONST;
GtkWidget    * csd_wacom_key_shortcut_button_new             (void);

#endif /* __CSD_WACOM_KEY_SHORTCUT_BUTTON_H__ */
