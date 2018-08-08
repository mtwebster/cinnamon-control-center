/*
 * Copyright © 2013 Red Hat, Inc.
 *
 * Authors: Joaquim Rocha <jrocha@redhat.com>
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

#include <config.h>
#include <glib/gi18n-lib.h>

#include "csd-wacom-key-shortcut-button.h"
#include "cc-wacom-button-row.h"

G_DEFINE_TYPE (CcWacomButtonRow, cc_wacom_button_row, GTK_TYPE_LIST_BOX_ROW)

#define ACTION_KEY            "action"
#define KEYBINDING_KEY        "keybinding"

#define WACOM_C(x) g_dpgettext2(NULL, "Wacom action-type", x)

enum {
  ACTION_NAME_COLUMN,
  ACTION_TYPE_COLUMN,
  ACTION_N_COLUMNS
};

struct _CcWacomButtonRowPrivate {
  guint button;
  GSettings *settings;
  GtkDirectionType direction;
  GtkComboBox *action_combo;
  CsdWacomKeyShortcutButton *key_shortcut_button;
};

static GtkWidget *
create_actions_combo (void)
{
  GtkListStore    *model;
  GtkTreeIter      iter;
  GtkWidget       *combo;
  GtkCellRenderer *renderer;
  gint             i;

  model = gtk_list_store_new (ACTION_N_COLUMNS, G_TYPE_STRING, G_TYPE_INT);

  for (i = 0; i < G_N_ELEMENTS (action_table); i++)
    {
      gtk_list_store_append (model, &iter);
      gtk_list_store_set (model, &iter,
                          ACTION_NAME_COLUMN, WACOM_C(action_table[i].action_name),
                          ACTION_TYPE_COLUMN, action_table[i].action_type, -1);
    }

  combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (model));

  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer,
                                  "text", ACTION_NAME_COLUMN, NULL);


  return combo;
}

static void
cc_wacom_button_row_update_shortcut (CcWacomButtonRow        *row,
                                     CDesktopPadButtonAction  action_type)
{
  CcWacomButtonRowPrivate *priv;
  guint                    keyval;
  GdkModifierType          mask;
  char                    *shortcut;

  if (action_type != C_DESKTOP_PAD_BUTTON_ACTION_KEYBINDING)
    return;

  priv = row->priv;
  shortcut = g_settings_get_string (row->priv->settings, KEYBINDING_KEY);

  if (shortcut != NULL)
    {
      gtk_accelerator_parse (shortcut, &keyval, &mask);

      g_object_set (priv->key_shortcut_button,
                    "key-value", keyval,
                    "key-mods", mask,
                    NULL);

      g_free (shortcut);
    }
}

static void
cc_wacom_button_row_update_action (CcWacomButtonRow        *row,
                                   CDesktopPadButtonAction  action_type)
{
  CcWacomButtonRowPrivate *priv;
  GtkTreeIter              iter;
  gboolean                 iter_valid;
  CDesktopPadButtonAction  current_action_type, real_action_type;
  GtkTreeModel            *model;

  priv = row->priv;

  model = gtk_combo_box_get_model (priv->action_combo);
  real_action_type = action_type;

  for (iter_valid = gtk_tree_model_get_iter_first (model, &iter); iter_valid;
       iter_valid = gtk_tree_model_iter_next (model, &iter))
    {
      gtk_tree_model_get (model, &iter,
                          ACTION_TYPE_COLUMN, &current_action_type,
                          -1);

      if (current_action_type == real_action_type)
        {
          gtk_combo_box_set_active_iter (priv->action_combo, &iter);
          break;
        }
    }
}

static void
cc_wacom_button_row_update (CcWacomButtonRow *row)
{
  CcWacomButtonRowPrivate *priv;
  CDesktopPadButtonAction current_action_type;

  priv = row->priv;

  current_action_type = g_settings_get_enum (priv->settings, ACTION_KEY);

  cc_wacom_button_row_update_shortcut (row, current_action_type);

  cc_wacom_button_row_update_action (row, current_action_type);

  gtk_widget_set_sensitive (GTK_WIDGET (row->priv->key_shortcut_button),
                            current_action_type == C_DESKTOP_PAD_BUTTON_ACTION_KEYBINDING);
}

static void
change_button_action_type (CcWacomButtonRow        *row,
                           CDesktopPadButtonAction  type)
{
  g_settings_set_enum (row->priv->settings, ACTION_KEY, type);
  gtk_widget_set_sensitive (GTK_WIDGET (row->priv->key_shortcut_button),
                            type == C_DESKTOP_PAD_BUTTON_ACTION_KEYBINDING);
}

static void
on_key_shortcut_edited (CsdWacomKeyShortcutButton *shortcut_button,
                        CcWacomButtonRow    *row)
{
  char *custom_key;
  guint keyval;
  GdkModifierType mask;

  change_button_action_type (row, C_DESKTOP_PAD_BUTTON_ACTION_KEYBINDING);

  g_object_get (row->priv->key_shortcut_button,
                "key-value", &keyval,
                "key-mods", &mask,
                NULL);

  mask &= ~GDK_LOCK_MASK;

  custom_key = gtk_accelerator_name (keyval, mask);

  g_settings_set_string (row->priv->settings, KEYBINDING_KEY, custom_key);

  g_free (custom_key);
}

static void
on_key_shortcut_cleared (CsdWacomKeyShortcutButton *key_shortcut_button,
                         CcWacomButtonRow    *row)
{
  change_button_action_type (row, C_DESKTOP_PAD_BUTTON_ACTION_NONE);
  cc_wacom_button_row_update_action (row, C_DESKTOP_PAD_BUTTON_ACTION_NONE);
}

static void
on_row_action_combo_box_changed (GtkComboBox      *combo,
                                 CcWacomButtonRow *row)
{
  CDesktopPadButtonAction type;
  GtkTreeModel *model;
  GtkListBox *list_box;
  GtkTreeIter iter;

  if (!gtk_combo_box_get_active_iter (combo, &iter))
    return;

  /* Select the row where we changed the combo box (if not yet selected) */
  list_box = GTK_LIST_BOX (gtk_widget_get_parent (GTK_WIDGET (row)));
  if (list_box && gtk_list_box_get_selected_row (list_box) != GTK_LIST_BOX_ROW (row))
    gtk_list_box_select_row (list_box, GTK_LIST_BOX_ROW (row));

  model = gtk_combo_box_get_model (combo);
  gtk_tree_model_get (model, &iter, ACTION_TYPE_COLUMN, &type, -1);

  change_button_action_type (row, type);
}

static gboolean
on_key_shortcut_button_press_event (CsdWacomKeyShortcutButton  *button,
                                    GdkEventButton       *event,
                                    GtkListBoxRow        *row)
{
  GtkListBox *list_box;

  /* Select the row where we pressed the button (if not yet selected) */
  list_box = GTK_LIST_BOX (gtk_widget_get_parent (GTK_WIDGET (row)));
  if (list_box && gtk_list_box_get_selected_row (list_box) != row)
    gtk_list_box_select_row (list_box, row);

  return FALSE;
}

static void
cc_wacom_button_row_class_init (CcWacomButtonRowClass *button_row_class)
{
  g_type_class_add_private (button_row_class, sizeof (CcWacomButtonRowPrivate));
}

static void
cc_wacom_button_row_init (CcWacomButtonRow *button_row)
{
  button_row->priv = G_TYPE_INSTANCE_GET_PRIVATE (button_row,
                                                  CC_WACOM_TYPE_BUTTON_ROW,
                                                  CcWacomButtonRowPrivate);
}

GtkWidget *
cc_wacom_button_row_new (guint      button,
			 GSettings *settings)
{
  GtkWidget               *row;
  GtkWidget               *grid, *combo, *label, *shortcut_button;
  CcWacomButtonRowPrivate *priv;
  char *name = NULL;

  row = g_object_new (CC_WACOM_TYPE_BUTTON_ROW, NULL);
  priv = CC_WACOM_BUTTON_ROW (row)->priv;

  priv->button = button;
  priv->settings = g_object_ref (settings);

  grid = gtk_grid_new ();
  gtk_widget_show (grid);
  gtk_grid_set_row_homogeneous (GTK_GRID (grid), TRUE);
  gtk_grid_set_column_homogeneous (GTK_GRID (grid), TRUE);

  name = g_strdup_printf (_("Button %d"), button);
  label = gtk_label_new (name);
  g_object_set (label, "halign", GTK_ALIGN_START, NULL);
  gtk_grid_attach (GTK_GRID (grid), label, 0, 0, 1, 1);
  gtk_widget_show (label);

  combo = create_actions_combo ();
  gtk_grid_attach (GTK_GRID (grid), combo, 1, 0, 1, 1);
  gtk_widget_show (combo);
  priv->action_combo = GTK_COMBO_BOX (combo);
  g_signal_connect (combo, "changed",
                    G_CALLBACK (on_row_action_combo_box_changed), row);

  shortcut_button = csd_wacom_key_shortcut_button_new ();
  g_object_set (shortcut_button, "mode", CSD_WACOM_KEY_SHORTCUT_BUTTON_MODE_ALL, NULL);
  gtk_grid_attach (GTK_GRID (grid), shortcut_button, 2, 0, 1, 1);
  gtk_widget_show (shortcut_button);
  priv->key_shortcut_button = CSD_WACOM_KEY_SHORTCUT_BUTTON (shortcut_button);
  g_signal_connect (shortcut_button, "key-shortcut-cleared",
                    G_CALLBACK (on_key_shortcut_cleared),
                    row);
  g_signal_connect (shortcut_button, "key-shortcut-edited",
                    G_CALLBACK (on_key_shortcut_edited),
                    row);
  g_signal_connect (shortcut_button, "button-press-event",
                    G_CALLBACK (on_key_shortcut_button_press_event),
                    row);

  gtk_container_add (GTK_CONTAINER (row), grid);

  cc_wacom_button_row_update (CC_WACOM_BUTTON_ROW (row));

  g_free (name);

  return row;
}
