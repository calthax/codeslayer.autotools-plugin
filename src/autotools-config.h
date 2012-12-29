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

#ifndef __AUTOTOOLS_CONFIG_H__
#define	__AUTOTOOLS_CONFIG_H__

#include <gtk/gtk.h>
#include <codeslayer/codeslayer.h>

G_BEGIN_DECLS

#define AUTOTOOLS_CONFIG_TYPE            (autotools_config_get_type ())
#define AUTOTOOLS_CONFIG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), AUTOTOOLS_CONFIG_TYPE, AutotoolsConfig))
#define AUTOTOOLS_CONFIG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), AUTOTOOLS_CONFIG_TYPE, AutotoolsConfigClass))
#define IS_AUTOTOOLS_CONFIG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AUTOTOOLS_CONFIG_TYPE))
#define IS_AUTOTOOLS_CONFIG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), AUTOTOOLS_CONFIG_TYPE))

typedef struct _AutotoolsConfig AutotoolsConfig;
typedef struct _AutotoolsConfigClass AutotoolsConfigClass;

struct _AutotoolsConfig
{
  GObject parent_instance;
};

struct _AutotoolsConfigClass
{
  GObjectClass parent_class;
};

GType autotools_config_get_type (void) G_GNUC_CONST;

AutotoolsConfig*          autotools_config_new                       (void);

CodeSlayerProject*        autotools_config_get_project               (AutotoolsConfig *config);
void                      autotools_config_set_project               (AutotoolsConfig *config,
                                                                      CodeSlayerProject *project);
const gchar*              autotools_config_get_configure_file        (AutotoolsConfig *config);
void                      autotools_config_set_configure_file        (AutotoolsConfig *config,
                                                                      const gchar     *configure_file);
const gchar*              autotools_config_get_configure_parameters  (AutotoolsConfig *config);
void                      autotools_config_set_configure_parameters  (AutotoolsConfig *config,
                                                                      const gchar     *configure_parameters);
const gchar*              autotools_config_get_build_folder          (AutotoolsConfig *config);
void                      autotools_config_set_build_folder          (AutotoolsConfig *config,
                                                                      const gchar     *build_folder);

G_END_DECLS

#endif /* __AUTOTOOLS_CONFIG_H__ */
