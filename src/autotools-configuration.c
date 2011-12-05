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

#include "autotools-configuration.h"

static void autotools_configuration_class_init    (AutotoolsConfigurationClass *klass);
static void autotools_configuration_init          (AutotoolsConfiguration      *configuration);
static void autotools_configuration_finalize      (AutotoolsConfiguration      *configuration);
static void autotools_configuration_get_property  (GObject                     *object, 
                                                   guint                        prop_id,
                                                   GValue                      *value,
                                                   GParamSpec                  *pspec);
static void autotools_configuration_set_property  (GObject                     *object, 
                                                   guint                        prop_id,
                                                   const GValue                *value,
                                                   GParamSpec                  *pspec);

#define AUTOTOOLS_CONFIGURATION_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), AUTOTOOLS_CONFIGURATION_TYPE, AutotoolsConfigurationPrivate))

typedef struct _AutotoolsConfigurationPrivate AutotoolsConfigurationPrivate;

struct _AutotoolsConfigurationPrivate
{
  gchar *project_key;
  gchar *configure_file;
  gchar *configure_parameters;
  gchar *build_directory;
};

enum
{
  PROP_0,
  PROP_PROJECT_KEY,
  PROP_CONFIGURE_FILE,
  PROP_CONFIGURE_PARAMETERS,
  PROP_BUILD_DIRECTORY
};

G_DEFINE_TYPE (AutotoolsConfiguration, autotools_configuration, G_TYPE_OBJECT)
     
static void 
autotools_configuration_class_init (AutotoolsConfigurationClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = (GObjectFinalizeFunc) autotools_configuration_finalize;

  gobject_class->get_property = autotools_configuration_get_property;
  gobject_class->set_property = autotools_configuration_set_property;

  g_type_class_add_private (klass, sizeof (AutotoolsConfigurationPrivate));

  g_object_class_install_property (gobject_class, 
                                   PROP_PROJECT_KEY,
                                   g_param_spec_string ("project_key", 
                                                        "Project Key",
                                                        "Project Key Object", "",
                                                        G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, 
                                   PROP_CONFIGURE_FILE,
                                   g_param_spec_string ("configure_file",
                                                        "Configure File",
                                                        "Configure File Object",
                                                        "",
                                                        G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, 
                                   PROP_CONFIGURE_PARAMETERS,
                                   g_param_spec_string ("configure_parameters",
                                                        "Configure Parameters",
                                                        "Configure Parameters Object",
                                                        "",
                                                        G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, 
                                   PROP_BUILD_DIRECTORY,
                                   g_param_spec_string ("build_directory",
                                                        "Build Directory",
                                                        "Build Directory Object",
                                                        "",
                                                        G_PARAM_READWRITE));
}

static void
autotools_configuration_init (AutotoolsConfiguration *configuration)
{
  AutotoolsConfigurationPrivate *priv;
  priv = AUTOTOOLS_CONFIGURATION_GET_PRIVATE (configuration);
  priv->project_key = NULL;
  priv->configure_file = NULL;
  priv->configure_parameters = NULL;
  priv->build_directory = NULL;
}

static void
autotools_configuration_finalize (AutotoolsConfiguration *configuration)
{
  AutotoolsConfigurationPrivate *priv;
  priv = AUTOTOOLS_CONFIGURATION_GET_PRIVATE (configuration);
  if (priv->project_key)
    {
      g_free (priv->project_key);
      priv->project_key = NULL;
    }
  if (priv->configure_file)
    {
      g_free (priv->configure_file);
      priv->configure_file = NULL;
    }
  if (priv->configure_parameters)
    {
      g_free (priv->configure_parameters);
      priv->configure_parameters = NULL;
    }
  if (priv->build_directory)
    {
      g_free (priv->build_directory);
      priv->build_directory = NULL;
    }
  G_OBJECT_CLASS (autotools_configuration_parent_class)->finalize (G_OBJECT (configuration));
}

static void
autotools_configuration_get_property (GObject    *object, 
                                      guint       prop_id,
                                      GValue     *value, 
                                      GParamSpec *pspec)
{
  AutotoolsConfiguration *configuration;
  AutotoolsConfigurationPrivate *priv;
  
  configuration = AUTOTOOLS_CONFIGURATION (object);
  priv = AUTOTOOLS_CONFIGURATION_GET_PRIVATE (configuration);

  switch (prop_id)
    {
    case PROP_PROJECT_KEY:
      g_value_set_string (value, priv->project_key);
      break;
    case PROP_CONFIGURE_FILE:
      g_value_set_string (value, priv->configure_file);
      break;
    case PROP_CONFIGURE_PARAMETERS:
      g_value_set_string (value, priv->configure_parameters);
      break;
    case PROP_BUILD_DIRECTORY:
      g_value_set_string (value, priv->build_directory);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
autotools_configuration_set_property (GObject      *object, 
                                      guint         prop_id,
                                      const GValue *value, 
                                      GParamSpec   *pspec)
{
  AutotoolsConfiguration *configuration;
  configuration = AUTOTOOLS_CONFIGURATION (object);

  switch (prop_id)
    {
    case PROP_PROJECT_KEY:
      autotools_configuration_set_project_key (configuration, g_value_get_string (value));
      break;
    case PROP_CONFIGURE_FILE:
      autotools_configuration_set_configure_file (configuration, g_value_get_string (value));
      break;
    case PROP_CONFIGURE_PARAMETERS:
      autotools_configuration_set_configure_parameters (configuration, g_value_get_string (value));
      break;
    case PROP_BUILD_DIRECTORY:
      autotools_configuration_set_build_directory (configuration, g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

AutotoolsConfiguration*
autotools_configuration_new (void)
{
  return AUTOTOOLS_CONFIGURATION (g_object_new (autotools_configuration_get_type (), NULL));
}

const gchar*
autotools_configuration_get_project_key (AutotoolsConfiguration *configuration)
{
  return AUTOTOOLS_CONFIGURATION_GET_PRIVATE (configuration)->project_key;
}

void
autotools_configuration_set_project_key (AutotoolsConfiguration *configuration, 
                                         const gchar             *project_key)
{
  AutotoolsConfigurationPrivate *priv;
  priv = AUTOTOOLS_CONFIGURATION_GET_PRIVATE (configuration);
  if (priv->project_key)
    {
      g_free (priv->project_key);
      priv->project_key = NULL;
    }
  priv->project_key = g_strdup (project_key);
}

const gchar*
autotools_configuration_get_configure_file (AutotoolsConfiguration *configuration)
{
  return AUTOTOOLS_CONFIGURATION_GET_PRIVATE (configuration)->configure_file;
}

void
autotools_configuration_set_configure_file (AutotoolsConfiguration *configuration,
                                            const gchar            *configure_file)
{
  AutotoolsConfigurationPrivate *priv;
  priv = AUTOTOOLS_CONFIGURATION_GET_PRIVATE (configuration);
  if (priv->configure_file)
    {
      g_free (priv->configure_file);
      priv->configure_file = NULL;
    }
  priv->configure_file = g_strdup (configure_file);
}

const gchar*
autotools_configuration_get_configure_parameters (AutotoolsConfiguration *configuration)
{
  return AUTOTOOLS_CONFIGURATION_GET_PRIVATE (configuration)->configure_parameters;
}

void
autotools_configuration_set_configure_parameters (AutotoolsConfiguration *configuration,
                                                  const gchar            *configure_parameters)
{
  AutotoolsConfigurationPrivate *priv;
  priv = AUTOTOOLS_CONFIGURATION_GET_PRIVATE (configuration);
  if (priv->configure_parameters)
    {
      g_free (priv->configure_parameters);
      priv->configure_parameters = NULL;
    }
  priv->configure_parameters = g_strdup (configure_parameters);
}

const gchar*
autotools_configuration_get_build_directory (AutotoolsConfiguration *configuration)
{
  return AUTOTOOLS_CONFIGURATION_GET_PRIVATE (configuration)->build_directory;
}

void
autotools_configuration_set_build_directory (AutotoolsConfiguration *configuration,
                                             const gchar            *build_directory)
{
  AutotoolsConfigurationPrivate *priv;
  priv = AUTOTOOLS_CONFIGURATION_GET_PRIVATE (configuration);
  if (priv->build_directory)
    {
      g_free (priv->build_directory);
      priv->build_directory = NULL;
    }
  priv->build_directory = g_strdup (build_directory);
}
