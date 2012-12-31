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
#include "autotools-engine.h"
#include "autotools-project-properties.h"
#include "autotools-config.h"
#include "autotools-notebook.h"
#include "autotools-output.h"

typedef struct
{
  AutotoolsOutput *output;
  gchar           *text;
} OutputContext;

typedef struct
{
  AutotoolsOutput *output;
  gint id;  
  gchar *text;
} Process;

#define MAIN "main"
#define CONFIGURE_FILE "configure_file"
#define CONFIGURE_PARAMETERS "configure_parameters"
#define BUILD_FOLDER "build_folder"
#define AUTOTOOLS_CONF "autotools.conf"

static void autotools_engine_class_init              (AutotoolsEngineClass *klass);
static void autotools_engine_init                    (AutotoolsEngine      *engine);
static void autotools_engine_finalize                (AutotoolsEngine      *engine);

static void make_action                              (AutotoolsEngine      *engine);
static void project_make_action                      (AutotoolsEngine      *engine, 
                                                      GList                *selections);

static void make_install_action                      (AutotoolsEngine      *engine);
static void project_make_install_action              (AutotoolsEngine      *engine, 
                                                      GList                *selections);

static void make_clean_action                        (AutotoolsEngine      *engine);
static void project_make_clean_action                (AutotoolsEngine      *engine, 
                                                      GList                *selections);

static void project_configure_action                 (AutotoolsEngine      *engine, 
                                                      GList                *selections);

static void project_autoreconf_action                (AutotoolsEngine      *engine, 
                                                      GList                *selections);

static void execute_make                             (AutotoolsOutput      *output);
static void execute_make_clean                       (AutotoolsOutput      *output);
static void execute_make_install                     (AutotoolsOutput      *output);
static void execute_configure                        (AutotoolsOutput      *output);
static void execute_autoreconf                       (AutotoolsOutput      *output);

static void run_command                              (AutotoolsOutput      *output,
                                                      gchar                *command);

static AutotoolsConfig* get_config_by_project        (AutotoolsEngine      *engine, 
                                                      CodeSlayerProject    *project);
static void execute_configure                        (AutotoolsOutput      *output);
static AutotoolsOutput* get_output_by_active_editor  (AutotoolsEngine      *engine);

static AutotoolsOutput* get_output_by_project        (AutotoolsEngine      *engine, 
                                                      CodeSlayerProject    *project);

static void project_properties_opened_action         (AutotoolsEngine      *engine,
                                                      CodeSlayerProject    *project);
static void project_properties_saved_action          (AutotoolsEngine      *engine,
                                                      CodeSlayerProject    *project);
                                                                        
static void save_config_action                       (AutotoolsEngine      *engine,
                                                      AutotoolsConfig      *config);
                                                                  
static CodeSlayerProject* get_selections_project     (GList                *selections);

static gboolean clear_text                           (AutotoolsOutput      *output);
static gboolean append_text                          (OutputContext        *context);
static void     destroy_text                         (OutputContext        *context);
static gboolean create_links                         (AutotoolsOutput      *output);
static gboolean start_process                        (Process              *process);
static gboolean stop_process                         (Process              *process);
static void destroy_process                          (Process              *process);                                  
                                                   
#define AUTOTOOLS_ENGINE_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), AUTOTOOLS_ENGINE_TYPE, AutotoolsEnginePrivate))

typedef struct _AutotoolsEnginePrivate AutotoolsEnginePrivate;

struct _AutotoolsEnginePrivate
{
  CodeSlayer *codeslayer;
  GtkWidget  *menu;
  GtkWidget  *project_properties;
  GtkWidget  *projects_menu;
  GtkWidget  *notebook;
  GList      *configs;
  gulong      properties_opened_id;
  gulong      properties_saved_id;
};

G_DEFINE_TYPE (AutotoolsEngine, autotools_engine, G_TYPE_OBJECT)

static void
autotools_engine_class_init (AutotoolsEngineClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = (GObjectFinalizeFunc) autotools_engine_finalize;
  g_type_class_add_private (klass, sizeof (AutotoolsEnginePrivate));
}

static void
autotools_engine_init (AutotoolsEngine *engine) 
{
  AutotoolsEnginePrivate *priv;
  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);
  priv->configs = NULL;
}

static void
autotools_engine_finalize (AutotoolsEngine *engine)
{
  AutotoolsEnginePrivate *priv;
  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);
  if (priv->configs != NULL)
    {
      g_list_foreach (priv->configs, (GFunc) g_object_unref, NULL);
      g_list_free (priv->configs);
      priv->configs = NULL;    
    }
  
  g_signal_handler_disconnect (priv->codeslayer, priv->properties_opened_id);
  g_signal_handler_disconnect (priv->codeslayer, priv->properties_saved_id);

  G_OBJECT_CLASS (autotools_engine_parent_class)->finalize (G_OBJECT(engine));
}

AutotoolsEngine*
autotools_engine_new (CodeSlayer *codeslayer,
                      GtkWidget  *menu, 
                      GtkWidget  *project_properties,
                      GtkWidget  *projects_menu,
                      GtkWidget  *notebook)
{
  AutotoolsEnginePrivate *priv;
  AutotoolsEngine *engine;

  engine = AUTOTOOLS_ENGINE (g_object_new (autotools_engine_get_type (), NULL));
  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);

  priv->codeslayer = codeslayer;
  priv->menu = menu;
  priv->project_properties = project_properties;
  priv->projects_menu = projects_menu;
  priv->notebook = notebook;
  
  g_signal_connect_swapped (G_OBJECT (menu), "make",
                            G_CALLBACK (make_action), engine);

  g_signal_connect_swapped (G_OBJECT (projects_menu), "make",
                            G_CALLBACK (project_make_action), engine);

  g_signal_connect_swapped (G_OBJECT (menu), "make-install",
                            G_CALLBACK (make_install_action), engine);

  g_signal_connect_swapped (G_OBJECT (projects_menu), "make-install",
                            G_CALLBACK (project_make_install_action), engine);

  g_signal_connect_swapped (G_OBJECT (menu), "make-clean",
                            G_CALLBACK (make_clean_action), engine);

  g_signal_connect_swapped (G_OBJECT (projects_menu), "make-clean",
                            G_CALLBACK (project_make_clean_action), engine);

  g_signal_connect_swapped (G_OBJECT (projects_menu), "configure",
                            G_CALLBACK (project_configure_action), engine);

  g_signal_connect_swapped (G_OBJECT (projects_menu), "autoreconf",
                            G_CALLBACK (project_autoreconf_action), engine);

  priv->properties_opened_id =  g_signal_connect_swapped (G_OBJECT (codeslayer), "project-properties-opened",
                                                          G_CALLBACK (project_properties_opened_action), engine);

  priv->properties_saved_id = g_signal_connect_swapped (G_OBJECT (codeslayer), "project-properties-saved",
                                                        G_CALLBACK (project_properties_saved_action), engine);

  g_signal_connect_swapped (G_OBJECT (project_properties), "save-config",
                            G_CALLBACK (save_config_action), engine);

  return engine;
}

static AutotoolsConfig*
get_config_by_project (AutotoolsEngine   *engine, 
                       CodeSlayerProject *project)
{
  AutotoolsEnginePrivate *priv;
  AutotoolsConfig *config;
  GKeyFile *keyfile;
  gchar *folder_path;
  gchar *file_path;
  gchar *configure_file;
  gchar *configure_parameters;
  gchar *build_folder;
  
  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);

  folder_path = codeslayer_get_project_config_folder_path (priv->codeslayer, project);
  file_path = g_build_filename (folder_path, AUTOTOOLS_CONF, NULL);
  
  if (!codeslayer_utils_file_exists (file_path))
    {
      g_free (folder_path);
      g_free (file_path);
      return NULL;
    }

  keyfile = codeslayer_utils_get_keyfile (file_path);
  configure_file = g_key_file_get_string (keyfile, MAIN, CONFIGURE_FILE, NULL);
  configure_parameters = g_key_file_get_string (keyfile, MAIN, CONFIGURE_PARAMETERS, NULL);
  build_folder = g_key_file_get_string (keyfile, MAIN, BUILD_FOLDER, NULL);
  
  config = autotools_config_new ();
  autotools_config_set_project (config, project);
  autotools_config_set_configure_file (config, configure_file);
  autotools_config_set_configure_parameters (config, configure_parameters);
  autotools_config_set_build_folder (config, build_folder);
  
  g_free (folder_path);
  g_free (file_path);
  g_free (configure_file);
  g_free (configure_parameters);
  g_free (build_folder);
  g_key_file_free (keyfile);
  
  return config;
}

static void
project_properties_opened_action (AutotoolsEngine   *engine,
                                  CodeSlayerProject *project)
{
  AutotoolsEnginePrivate *priv;
  AutotoolsConfig *config;

  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);
  
  config = get_config_by_project (engine, project);
  autotools_project_properties_opened (AUTOTOOLS_PROJECT_PROPERTIES (priv->project_properties),
                                       config, project);
  if (config != NULL)
    g_object_unref (config);                                       
}

static void
project_properties_saved_action (AutotoolsEngine   *engine,
                                 CodeSlayerProject *project)
{
  AutotoolsEnginePrivate *priv;
  AutotoolsConfig *config;
  
  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);

  config = get_config_by_project (engine, project);
  autotools_project_properties_saved (AUTOTOOLS_PROJECT_PROPERTIES (priv->project_properties),
                                      config, project);
  if (config != NULL)
    g_object_unref (config);                                       
}

static void
save_config_action (AutotoolsEngine *engine,
                    AutotoolsConfig *config)
{
  AutotoolsEnginePrivate *priv;
  CodeSlayerProject *project;
  gchar *folder_path;
  gchar *file_path;
  const gchar *configure_file;
  const gchar *configure_parameters;
  const gchar *build_folder;
  GKeyFile *keyfile;
 
  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);

  project = autotools_config_get_project (config);  
  folder_path = codeslayer_get_project_config_folder_path (priv->codeslayer, project);
  file_path = codeslayer_utils_get_file_path (folder_path, AUTOTOOLS_CONF);
  keyfile = codeslayer_utils_get_keyfile (file_path);

  configure_file = autotools_config_get_configure_file (config);
  configure_parameters = autotools_config_get_configure_parameters (config);
  build_folder = autotools_config_get_build_folder (config);
  g_key_file_set_string (keyfile, MAIN, CONFIGURE_FILE, configure_file);
  g_key_file_set_string (keyfile, MAIN, CONFIGURE_PARAMETERS, configure_parameters);
  g_key_file_set_string (keyfile, MAIN, BUILD_FOLDER, build_folder);

  codeslayer_utils_save_keyfile (keyfile, file_path);  
  g_key_file_free (keyfile);
  g_free (folder_path);
  g_free (file_path); 
}

static void
make_action (AutotoolsEngine *engine)
{
  AutotoolsEnginePrivate *priv;
  AutotoolsOutput *output;  

  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);

  output =  get_output_by_active_editor (engine);
  if (output)
    {
      codeslayer_show_bottom_pane (priv->codeslayer, priv->notebook);
      autotools_notebook_select_page_by_output (AUTOTOOLS_NOTEBOOK (priv->notebook), 
                                                GTK_WIDGET (output));
      g_thread_new ("make", (GThreadFunc) execute_make, output);                                                
    }
}

static void
project_make_action (AutotoolsEngine *engine, 
                     GList           *selections)
{
  AutotoolsEnginePrivate *priv;
  AutotoolsOutput *output;
  CodeSlayerProject *project;

  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);
  
  project = get_selections_project (selections);
  output = get_output_by_project (engine, project);
  if (output)
    {
      codeslayer_show_bottom_pane (priv->codeslayer, priv->notebook);
      autotools_notebook_select_page_by_output (AUTOTOOLS_NOTEBOOK (priv->notebook), 
                                                GTK_WIDGET (output));
      g_thread_new ("project-make", (GThreadFunc) execute_make, output);                                                
    }
}

static void
make_install_action (AutotoolsEngine *engine)
{
  AutotoolsEnginePrivate *priv;
  AutotoolsOutput *output;  

  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);

  output =  get_output_by_active_editor (engine);
  if (output)
    {
      codeslayer_show_bottom_pane (priv->codeslayer, priv->notebook);
      autotools_notebook_select_page_by_output (AUTOTOOLS_NOTEBOOK (priv->notebook), 
                                                GTK_WIDGET (output));
      g_thread_new ("make-install", (GThreadFunc) execute_make_install, output);                                                
    }
}   

static void
project_make_install_action (AutotoolsEngine *engine, 
                             GList           *selections)
{
  AutotoolsEnginePrivate *priv;
  AutotoolsOutput *output;  
  CodeSlayerProject *project;

  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);
  
  project = get_selections_project (selections);
  output =  get_output_by_project (engine, project);
  if (output)
    {
      codeslayer_show_bottom_pane (priv->codeslayer, priv->notebook);
      autotools_notebook_select_page_by_output (AUTOTOOLS_NOTEBOOK (priv->notebook), 
                                                GTK_WIDGET (output));
      g_thread_new ("project-make-install", (GThreadFunc) execute_make_install, output);                                                
    }
}   

static void
make_clean_action (AutotoolsEngine *engine)
{
  AutotoolsEnginePrivate *priv;
  AutotoolsOutput *output;  

  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);

  output =  get_output_by_active_editor (engine);
  if (output)
    {
      codeslayer_show_bottom_pane (priv->codeslayer, priv->notebook);
      autotools_notebook_select_page_by_output (AUTOTOOLS_NOTEBOOK (priv->notebook), 
                                                GTK_WIDGET (output));
      g_thread_new ("make-clean", (GThreadFunc) execute_make_clean, output);                                                
    }
}

static void
project_make_clean_action (AutotoolsEngine *engine, 
                           GList           *selections)
{
  AutotoolsEnginePrivate *priv;
  AutotoolsOutput *output;  
  CodeSlayerProject *project;

  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);
  
  project = get_selections_project (selections);
  output =  get_output_by_project (engine, project);
  if (output)
    {
      codeslayer_show_bottom_pane (priv->codeslayer, priv->notebook);
      autotools_notebook_select_page_by_output (AUTOTOOLS_NOTEBOOK (priv->notebook), 
                                                GTK_WIDGET (output));
      g_thread_new ("project-make-clean", (GThreadFunc) execute_make_clean, output);                                                
    }
}

static void
project_configure_action (AutotoolsEngine *engine, 
                          GList           *selections)
{
  AutotoolsEnginePrivate *priv;
  AutotoolsOutput *output;  
  CodeSlayerProject *project;

  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);
  
  project = get_selections_project (selections);
  output =  get_output_by_project (engine, project);
  if (output)
    {
      codeslayer_show_bottom_pane (priv->codeslayer, priv->notebook);
      autotools_notebook_select_page_by_output (AUTOTOOLS_NOTEBOOK (priv->notebook), 
                                                GTK_WIDGET (output));
      g_thread_new ("project-configure", (GThreadFunc) execute_configure, output);                                                
    }
}

static void
project_autoreconf_action (AutotoolsEngine *engine, 
                           GList           *selections)
{
  AutotoolsEnginePrivate *priv;
  AutotoolsOutput *output;  
  CodeSlayerProject *project;

  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);
  
  project = get_selections_project (selections);
  output =  get_output_by_project (engine, project);
  if (output)
    {
      codeslayer_show_bottom_pane (priv->codeslayer, priv->notebook);
      autotools_notebook_select_page_by_output (AUTOTOOLS_NOTEBOOK (priv->notebook), 
                                                GTK_WIDGET (output));
      g_thread_new ("project-autoreconf", (GThreadFunc) execute_autoreconf, output);                                                
    }
}

static CodeSlayerProject*
get_selections_project (GList *selections)
{
  if (selections != NULL)
    {
      CodeSlayerProjectsSelection *selection = selections->data;
      return codeslayer_projects_selection_get_project (CODESLAYER_PROJECTS_SELECTION (selection));
    }
  return NULL;  
}

static void
execute_make (AutotoolsOutput *output)
{
  AutotoolsConfig *config;
  const gchar *build_folder;             
  gchar *command;
  Process *process;

  process = g_malloc (sizeof (Process));
  process->output = output;
  process->text = g_strdup (_("Make..."));

  g_idle_add ((GSourceFunc) start_process, process);
  
  config = autotools_output_get_config (output);
  build_folder = autotools_config_get_build_folder (config);
  
  command = g_strconcat ("cd ", build_folder, ";make 2>&1", NULL);
  run_command (output, command);
  g_free (command);

  g_idle_add_full (G_PRIORITY_DEFAULT_IDLE, (GSourceFunc) stop_process, process, (GDestroyNotify) destroy_process);
}

static void
execute_make_install (AutotoolsOutput *output)
{
  AutotoolsConfig *config;
  const gchar *build_folder;             
  gchar *command;  
  Process *process;

  process = g_malloc (sizeof (Process));
  process->output = output;
  process->text = g_strdup (_("Make Install..."));

  g_idle_add ((GSourceFunc) start_process, process);
  
  config = autotools_output_get_config (output);
  build_folder = autotools_config_get_build_folder (config);
  
  command = g_strconcat ("cd ", build_folder, ";make install 2>&1", NULL);
  run_command (output, command);
  g_free (command);   
  
  g_idle_add_full (G_PRIORITY_DEFAULT_IDLE, (GSourceFunc) stop_process, process, (GDestroyNotify) destroy_process); 
}

static void
execute_make_clean (AutotoolsOutput *output)
{
  AutotoolsConfig *config;
  const gchar *build_folder;             
  gchar *command;
  Process *process;

  process = g_malloc (sizeof (Process));
  process->output = output;
  process->text = g_strdup (_("Make Clean..."));

  g_idle_add ((GSourceFunc) start_process, process);
  
  config = autotools_output_get_config (output);
  build_folder = autotools_config_get_build_folder (config);
  
  command = g_strconcat ("cd ", build_folder, ";make clean 2>&1", NULL);
  run_command (output, command);
  g_free (command);
  
  g_idle_add_full (G_PRIORITY_DEFAULT_IDLE, (GSourceFunc) stop_process, process, (GDestroyNotify) destroy_process); 
}

static void
execute_configure (AutotoolsOutput *output)
{
  AutotoolsConfig *config;
  const gchar *build_folder;
  const gchar *configure_file;             
  const gchar *configure_parameters;             
  gchar *configure_file_path;             
  gchar *command;
  
  config = autotools_output_get_config (output);
  build_folder = autotools_config_get_build_folder (config);
  configure_file = autotools_config_get_configure_file (config);
  configure_file_path = g_path_get_dirname (configure_file);
  configure_parameters = autotools_config_get_configure_parameters (config);
  
  command = g_strconcat ("cd ", build_folder, ";", configure_file_path, 
                         G_DIR_SEPARATOR_S, "configure ", configure_parameters, " 2>&1", NULL);
  g_free (configure_file_path);    

  run_command (output, command);
  g_free (command);    
}

static void
execute_autoreconf (AutotoolsOutput *output)
{
  AutotoolsConfig *config;
  const gchar *configure_file;             
  gchar *configure_file_path;             
  gchar *command;
  OutputContext *context;
  
  config = autotools_output_get_config (output);
  configure_file = autotools_config_get_configure_file (config);
  configure_file_path = g_path_get_dirname (configure_file);
  
  command = g_strconcat ("cd ", configure_file_path, ";autoreconf 2>&1", NULL);
  g_free (configure_file_path);

  run_command (output, command);
  g_free (command);
  
  context = g_malloc (sizeof (OutputContext));
  context->output = output;
  context->text = g_strdup ("autoreconf finished\n");
  g_idle_add_full (G_PRIORITY_DEFAULT_IDLE, (GSourceFunc) append_text, context, (GDestroyNotify)destroy_text);
}

static AutotoolsOutput*
get_output_by_active_editor (AutotoolsEngine *engine)
{
  AutotoolsEnginePrivate *priv;
  CodeSlayer *codeslayer;
  CodeSlayerProject *project;
  
  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);
  codeslayer = priv->codeslayer;
  
  project = codeslayer_get_active_editor_project (codeslayer);
  if (project == NULL)
    {
      GtkWidget *dialog;
      dialog =  gtk_message_dialog_new (NULL, 
                                        GTK_DIALOG_MODAL,
                                        GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                        "There are no open editors. Not able to determine what project to execute command against.");
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      return NULL;
    }

  return get_output_by_project (engine, project);
}

static AutotoolsOutput*
get_output_by_project (AutotoolsEngine   *engine, 
                       CodeSlayerProject *project)
{
  AutotoolsEnginePrivate *priv;
  GtkWidget  *notebook;
  const gchar *project_name;
  AutotoolsConfig *config;
  GtkWidget *output;
  
  priv = AUTOTOOLS_ENGINE_GET_PRIVATE (engine);
  notebook = priv->notebook;
  
  project_name = codeslayer_project_get_name (project);
  config = get_config_by_project (engine, project);
  
  if (config == NULL)
    {
      GtkWidget *dialog;
      gchar *msg;
      msg = g_strconcat ("There is no config for project ", 
                         project_name, ".", NULL);      
      dialog =  gtk_message_dialog_new (NULL, 
                                        GTK_DIALOG_MODAL,
                                        GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                        msg, 
                                        NULL);
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      g_free (msg);
      return NULL;
    }
  
  output = autotools_notebook_get_output_by_config (AUTOTOOLS_NOTEBOOK (notebook), 
                                                    config);
  if (output == NULL)
    {
      output = autotools_output_new (config, priv->codeslayer);
      autotools_notebook_add_output (AUTOTOOLS_NOTEBOOK (priv->notebook), output, 
                                                         project_name);
    }                                                           

  return AUTOTOOLS_OUTPUT (output);
}

static void
run_command (AutotoolsOutput *output,
             gchar           *command)
{
  char out[BUFSIZ];
  FILE *file;
  
  g_idle_add ((GSourceFunc) clear_text, output);
  
  file = popen (command, "r");
  if (file != NULL)
    {
      while (fgets (out, BUFSIZ, file))
        {
          OutputContext *context;
          context = g_malloc (sizeof (OutputContext));
          context->output = output;
          context->text = g_strdup (out);
          g_idle_add_full (G_PRIORITY_DEFAULT_IDLE, (GSourceFunc) append_text, 
                           context, (GDestroyNotify)destroy_text);
        }
      pclose (file);
    }
    
  g_idle_add ((GSourceFunc) create_links, output);
}

static gboolean 
start_process (Process *process)
{
  gint id;
  CodeSlayer *codeslayer; 
  codeslayer = autotools_output_get_codeslayer (process->output);
  id = codeslayer_add_to_process_bar (codeslayer, process->text, NULL, NULL);
  process->id = id;
  return FALSE;
}

static gboolean 
stop_process (Process *process)
{
  CodeSlayer *codeslayer; 
  codeslayer = autotools_output_get_codeslayer (process->output);
  codeslayer_remove_from_process_bar (codeslayer, process->id);
  return FALSE;
}

static void 
destroy_process (Process *process)
{
  g_free (process->text);
  g_free (process);
}

static gboolean 
clear_text (AutotoolsOutput *output)
{
  GtkTextBuffer *buffer;
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (output));
  gtk_text_buffer_set_text (buffer, "", -1);
  return FALSE;
}

static gboolean 
append_text (OutputContext *context)
{
  GtkTextBuffer *buffer;
  GtkTextIter iter;
  GtkTextMark *text_mark;
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (context->output));
  gtk_text_buffer_get_end_iter (buffer, &iter);
  gtk_text_buffer_insert (buffer, &iter, context->text, -1);
  text_mark = gtk_text_buffer_create_mark (buffer, NULL, &iter, TRUE);
  gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (context->output), text_mark, 0.0, FALSE, 0, 0);
  return FALSE;
}

static void 
destroy_text (OutputContext *context)
{
  g_free (context->text);
  g_free (context);
}

static gboolean 
create_links (AutotoolsOutput *output)
{
  autotools_output_create_links (output);
  return FALSE;
}
