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

#ifndef __AUTOTOOLS_CONFIGURATION_H__
#define	__AUTOTOOLS_CONFIGURATION_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AUTOTOOLS_CONFIGURATION_TYPE            (autotools_configuration_get_type ())
#define AUTOTOOLS_CONFIGURATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), AUTOTOOLS_CONFIGURATION_TYPE, AutotoolsConfiguration))
#define AUTOTOOLS_CONFIGURATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), AUTOTOOLS_CONFIGURATION_TYPE, AutotoolsConfigurationClass))
#define IS_AUTOTOOLS_CONFIGURATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AUTOTOOLS_CONFIGURATION_TYPE))
#define IS_AUTOTOOLS_CONFIGURATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), AUTOTOOLS_CONFIGURATION_TYPE))

typedef struct _AutotoolsConfiguration AutotoolsConfiguration;
typedef struct _AutotoolsConfigurationClass AutotoolsConfigurationClass;

struct _AutotoolsConfiguration
{
  GObject parent_instance;
};

struct _AutotoolsConfigurationClass
{
  GObjectClass parent_class;
};

GType autotools_configuration_get_type (void) G_GNUC_CONST;

AutotoolsConfiguration*  autotools_configuration_new                       (void);

const gchar*             autotools_configuration_get_project_key           (AutotoolsConfiguration *configuration);
void                     autotools_configuration_set_project_key           (AutotoolsConfiguration *configuration,
                                                                            const gchar            *project_key);
const gchar*             autotools_configuration_get_configure_file        (AutotoolsConfiguration *configuration);
void                     autotools_configuration_set_configure_file        (AutotoolsConfiguration *configuration,
                                                                            const gchar            *configure_file);
const gchar*             autotools_configuration_get_configure_parameters  (AutotoolsConfiguration *configuration);
void                     autotools_configuration_set_configure_parameters  (AutotoolsConfiguration *configuration,
                                                                            const gchar            *configure_parameters);
const gchar*             autotools_configuration_get_build_folder_path     (AutotoolsConfiguration *configuration);
void                     autotools_configuration_set_build_folder_path     (AutotoolsConfiguration *configuration,
                                                                            const gchar            *build_folder_path);

G_END_DECLS

#endif /* __AUTOTOOLS_CONFIGURATION_H__ */
