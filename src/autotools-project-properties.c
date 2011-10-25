/*
 * Copyright (C) 2010 - Jeff Johnston
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <codeslayer/codeslayer-utils.h>
#include "autotools-project-properties.h"

static void autotools_project_properties_class_init  (AutotoolsProjectPropertiesClass *klass);
static void autotools_project_properties_init        (AutotoolsProjectProperties      *project_properties);
static void autotools_project_properties_finalize    (AutotoolsProjectProperties      *project_properties);

static void add_form                                 (AutotoolsProjectProperties      *project_properties);

static void build_folder_icon_action                 (GtkEntry                         *build_folder_entry,
                                                      GtkEntryIconPosition              icon_pos,
                                                      GdkEvent                         *event,
                                                      AutotoolsProjectProperties       *project_properties);

#define AUTOTOOLS_PROJECT_PROPERTIES_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), AUTOTOOLS_PROJECT_PROPERTIES_TYPE, AutotoolsProjectPropertiesPrivate))

typedef struct _AutotoolsProjectPropertiesPrivate AutotoolsProjectPropertiesPrivate;

struct _AutotoolsProjectPropertiesPrivate
{
  CodeSlayerProject *project;
  GtkWidget         *configure_parameters_entry;
  GtkWidget         *build_folder_entry;
};

enum
{
  SAVE_CONFIGURATION,
  LAST_SIGNAL
};

static guint autotools_project_properties_signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (AutotoolsProjectProperties, autotools_project_properties, GTK_TYPE_VBOX)

static void
autotools_project_properties_class_init (AutotoolsProjectPropertiesClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  autotools_project_properties_signals[SAVE_CONFIGURATION] =
    g_signal_new ("save-configuration", 
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                  G_STRUCT_OFFSET (AutotoolsProjectPropertiesClass, save_configuration), 
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE, 1, AUTOTOOLS_CONFIGURATION_TYPE);

  gobject_class->finalize = (GObjectFinalizeFunc) autotools_project_properties_finalize;
  g_type_class_add_private (klass, sizeof (AutotoolsProjectPropertiesPrivate));
}

static void
autotools_project_properties_init (AutotoolsProjectProperties *project_properties) {}

static void
autotools_project_properties_finalize (AutotoolsProjectProperties *project_properties)
{
  G_OBJECT_CLASS (autotools_project_properties_parent_class)->finalize (G_OBJECT(project_properties));
}

GtkWidget*
autotools_project_properties_new (void)
{
  GtkWidget *project_properties;
  project_properties = g_object_new (autotools_project_properties_get_type (), NULL);
  add_form (AUTOTOOLS_PROJECT_PROPERTIES (project_properties));
  return project_properties;
}

static void 
add_form (AutotoolsProjectProperties *project_properties)
{
  AutotoolsProjectPropertiesPrivate *priv;
  GtkWidget *table;

  GtkWidget *configure_file_label;

  GtkWidget *configure_parameters_label;
  GtkWidget *configure_parameters_entry;

  GtkWidget *build_folder_label;
  GtkWidget *build_folder_entry;

  priv = AUTOTOOLS_PROJECT_PROPERTIES_GET_PRIVATE (project_properties);

  table = gtk_table_new (3, 2, FALSE);

  configure_parameters_label = gtk_label_new ("Configure Parameters:");
  gtk_misc_set_alignment (GTK_MISC (configure_parameters_label), 1, .5);
  gtk_table_attach (GTK_TABLE (table), configure_parameters_label, 
                    0, 1, 0, 1, GTK_FILL, GTK_SHRINK, 4, 0);
  
  configure_parameters_entry = gtk_entry_new ();
  priv->configure_parameters_entry = configure_parameters_entry;
  gtk_entry_set_width_chars (GTK_ENTRY (configure_parameters_entry), 50);
  gtk_table_attach (GTK_TABLE (table), configure_parameters_entry, \
                    1, 2, 0, 1, GTK_FILL | GTK_EXPAND | GTK_SHRINK, GTK_FILL, 4, 1);
                      
  build_folder_label = gtk_label_new ("Build Folder:");
  gtk_misc_set_alignment (GTK_MISC (build_folder_label), 1, .5);
  gtk_table_attach (GTK_TABLE (table), build_folder_label, 
                    0, 1, 1, 2, GTK_FILL, GTK_SHRINK, 4, 0);
  
  build_folder_entry = gtk_entry_new ();
  priv->build_folder_entry = build_folder_entry;
  gtk_entry_set_width_chars (GTK_ENTRY (build_folder_entry), 50);
  gtk_entry_set_icon_from_stock (GTK_ENTRY (build_folder_entry), 
                                 GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_DIRECTORY);
  gtk_table_attach (GTK_TABLE (table), build_folder_entry, 
                    1, 2, 1, 2, GTK_FILL | GTK_EXPAND | GTK_SHRINK, GTK_FILL, 4, 1);
                      
  gtk_box_pack_start (GTK_BOX (project_properties), table, FALSE, FALSE, 2);
  

  configure_file_label = gtk_label_new ("");
  gtk_label_set_markup (GTK_LABEL (configure_file_label),
                        "<i><b>Note:</b> be sure to select the configure.ac file with the build file field in the Projects tab.</i>");
  gtk_misc_set_alignment (GTK_MISC (configure_file_label), 0, 1);
  gtk_misc_set_padding (GTK_MISC (configure_file_label), 5, 7);
  gtk_table_attach (GTK_TABLE (table), configure_file_label, 
                    0, 2, 2, 3, GTK_FILL, GTK_EXPAND, 4, 0);
                    

  g_signal_connect (G_OBJECT (build_folder_entry), "icon-press",
                    G_CALLBACK (build_folder_icon_action), project_properties);
}

static void 
build_folder_icon_action (GtkEntry                   *build_folder_entry,
                          GtkEntryIconPosition        icon_pos,
                          GdkEvent                   *event,
                          AutotoolsProjectProperties *project_properties)
{
  AutotoolsProjectPropertiesPrivate *priv;
  GtkWidget *dialog;
  gint response;
  const gchar *folder_path;
  
  priv = AUTOTOOLS_PROJECT_PROPERTIES_GET_PRIVATE (project_properties);
  
  dialog = gtk_file_chooser_dialog_new ("Select Build Directory",
                                        NULL,
                                        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                        GTK_STOCK_CANCEL,
                                        GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OPEN,
                                        GTK_RESPONSE_OK, 
                                        NULL);

  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
  
  folder_path = codeslayer_project_get_folder_path (priv->project);
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(dialog), folder_path);

  response = gtk_dialog_run (GTK_DIALOG (dialog));
  if (response == GTK_RESPONSE_OK)
    {
      GFile *file;
      char *file_path;
      file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));
      file_path = g_file_get_path (file);
      gtk_entry_set_text (build_folder_entry, file_path);
      g_free (file_path);
      g_object_unref (file);
    }

  gtk_widget_destroy (GTK_WIDGET (dialog));
}

void 
autotools_project_properties_opened (AutotoolsProjectProperties *project_properties,
                                     AutotoolsConfiguration     *configuration, 
                                     CodeSlayerProject          *project)
{
  AutotoolsProjectPropertiesPrivate *priv;

  priv = AUTOTOOLS_PROJECT_PROPERTIES_GET_PRIVATE (project_properties);  
  priv->project = project;
  
  if (configuration)
    {
      const gchar *configure_parameters;
      const gchar *build_folder_path;
    
      configure_parameters = autotools_configuration_get_configure_parameters (configuration);
      build_folder_path = autotools_configuration_get_build_folder_path (configuration);

      gtk_entry_set_text (GTK_ENTRY (priv->configure_parameters_entry), configure_parameters);
      gtk_entry_set_text (GTK_ENTRY (priv->build_folder_entry), build_folder_path);
    }
  else
    {
      gtk_entry_set_text (GTK_ENTRY (priv->configure_parameters_entry), "");
      gtk_entry_set_text (GTK_ENTRY (priv->build_folder_entry), "");
    }
}

void 
autotools_project_properties_saved (AutotoolsProjectProperties *project_properties,
                                    AutotoolsConfiguration     *configuration, 
                                    CodeSlayerProject          *project)
{
  AutotoolsProjectPropertiesPrivate *priv;
  gchar *configure_parameters;
  gchar *build_folder_path;

  priv = AUTOTOOLS_PROJECT_PROPERTIES_GET_PRIVATE (project_properties);
  
  configure_parameters = g_strdup (gtk_entry_get_text (GTK_ENTRY (priv->configure_parameters_entry)));
  build_folder_path = g_strdup (gtk_entry_get_text (GTK_ENTRY (priv->build_folder_entry)));
  
  g_strstrip (configure_parameters);
  g_strstrip (build_folder_path);
  
  if (configuration != NULL)
    {
      if (g_strcmp0 (configure_parameters, autotools_configuration_get_configure_parameters (configuration)) != 0 ||
          g_strcmp0 (build_folder_path, autotools_configuration_get_build_folder_path (configuration)) != 0)
        {
          autotools_configuration_set_configure_parameters (configuration, configure_parameters);
          autotools_configuration_set_build_folder_path (configuration, build_folder_path);
          g_signal_emit_by_name((gpointer)project_properties, "save-configuration", NULL);
        }
    }
  else
    {
      AutotoolsConfiguration *configuration;
      const gchar *project_key;
      configuration = autotools_configuration_new ();
      project_key = codeslayer_project_get_key (project);
      autotools_configuration_set_project_key (configuration, project_key);
      autotools_configuration_set_configure_parameters (configuration, configure_parameters);
      autotools_configuration_set_build_folder_path (configuration, build_folder_path);
      g_signal_emit_by_name((gpointer)project_properties, "save-configuration", configuration);
    }
    
  g_free (configure_parameters);
  g_free (build_folder_path);
}
