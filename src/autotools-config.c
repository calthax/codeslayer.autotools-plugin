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

#include "autotools-config.h"

static void autotools_config_class_init  (AutotoolsConfigClass *klass);
static void autotools_config_init        (AutotoolsConfig      *config);
static void autotools_config_finalize    (AutotoolsConfig      *config);

#define AUTOTOOLS_CONFIG_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), AUTOTOOLS_CONFIG_TYPE, AutotoolsConfigPrivate))

typedef struct _AutotoolsConfigPrivate AutotoolsConfigPrivate;

struct _AutotoolsConfigPrivate
{
  CodeSlayerProject *project;
  gchar             *configure_file;
  gchar             *configure_parameters;
  gchar             *build_folder;
};

enum
{
  PROP_0,
  PROP_PROJECT_KEY,
  PROP_CONFIGURE_FILE,
  PROP_CONFIGURE_PARAMETERS,
  PROP_BUILD_DIRECTORY
};

G_DEFINE_TYPE (AutotoolsConfig, autotools_config, G_TYPE_OBJECT)
     
static void 
autotools_config_class_init (AutotoolsConfigClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = (GObjectFinalizeFunc) autotools_config_finalize;
  g_type_class_add_private (klass, sizeof (AutotoolsConfigPrivate));
}

static void
autotools_config_init (AutotoolsConfig *config)
{
  AutotoolsConfigPrivate *priv;
  priv = AUTOTOOLS_CONFIG_GET_PRIVATE (config);
  priv->project = NULL;
  priv->configure_file = NULL;
  priv->configure_parameters = NULL;
  priv->build_folder = NULL;
}

static void
autotools_config_finalize (AutotoolsConfig *config)
{
  AutotoolsConfigPrivate *priv;
  priv = AUTOTOOLS_CONFIG_GET_PRIVATE (config);
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
  if (priv->build_folder)
    {
      g_free (priv->build_folder);
      priv->build_folder = NULL;
    }
  G_OBJECT_CLASS (autotools_config_parent_class)->finalize (G_OBJECT (config));
}

AutotoolsConfig*
autotools_config_new (void)
{
  return AUTOTOOLS_CONFIG (g_object_new (autotools_config_get_type (), NULL));
}

CodeSlayerProject*  
autotools_config_get_project (AutotoolsConfig *config)
{
  AutotoolsConfigPrivate *priv;
  priv = AUTOTOOLS_CONFIG_GET_PRIVATE (config);
  return priv->project;
}

void
autotools_config_set_project (AutotoolsConfig *config,
                              CodeSlayerProject *project)
{
  AutotoolsConfigPrivate *priv;
  priv = AUTOTOOLS_CONFIG_GET_PRIVATE (config);
  priv->project = project;
}                              

const gchar*
autotools_config_get_configure_file (AutotoolsConfig *config)
{
  return AUTOTOOLS_CONFIG_GET_PRIVATE (config)->configure_file;
}

void
autotools_config_set_configure_file (AutotoolsConfig *config,
                                            const gchar            *configure_file)
{
  AutotoolsConfigPrivate *priv;
  priv = AUTOTOOLS_CONFIG_GET_PRIVATE (config);
  if (priv->configure_file)
    {
      g_free (priv->configure_file);
      priv->configure_file = NULL;
    }
  priv->configure_file = g_strdup (configure_file);
}

const gchar*
autotools_config_get_configure_parameters (AutotoolsConfig *config)
{
  return AUTOTOOLS_CONFIG_GET_PRIVATE (config)->configure_parameters;
}

void
autotools_config_set_configure_parameters (AutotoolsConfig *config,
                                                  const gchar            *configure_parameters)
{
  AutotoolsConfigPrivate *priv;
  priv = AUTOTOOLS_CONFIG_GET_PRIVATE (config);
  if (priv->configure_parameters)
    {
      g_free (priv->configure_parameters);
      priv->configure_parameters = NULL;
    }
  priv->configure_parameters = g_strdup (configure_parameters);
}

const gchar*
autotools_config_get_build_folder (AutotoolsConfig *config)
{
  return AUTOTOOLS_CONFIG_GET_PRIVATE (config)->build_folder;
}

void
autotools_config_set_build_folder (AutotoolsConfig *config,
                                             const gchar            *build_folder)
{
  AutotoolsConfigPrivate *priv;
  priv = AUTOTOOLS_CONFIG_GET_PRIVATE (config);
  if (priv->build_folder)
    {
      g_free (priv->build_folder);
      priv->build_folder = NULL;
    }
  priv->build_folder = g_strdup (build_folder);
}
