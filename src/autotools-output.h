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

#ifndef __AUTOTOOLS_OUTPUT_H__
#define	__AUTOTOOLS_OUTPUT_H__

#include <gtk/gtk.h>
#include <codeslayer/codeslayer.h>
#include "autotools-config.h"

G_BEGIN_DECLS

#define AUTOTOOLS_OUTPUT_TYPE            (autotools_output_get_type ())
#define AUTOTOOLS_OUTPUT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), AUTOTOOLS_OUTPUT_TYPE, AutotoolsOutput))
#define AUTOTOOLS_OUTPUT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), AUTOTOOLS_OUTPUT_TYPE, AutotoolsOutputClass))
#define IS_AUTOTOOLS_OUTPUT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AUTOTOOLS_OUTPUT_TYPE))
#define IS_AUTOTOOLS_OUTPUT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), AUTOTOOLS_OUTPUT_TYPE))

typedef struct _AutotoolsOutput AutotoolsOutput;
typedef struct _AutotoolsOutputClass AutotoolsOutputClass;

struct _AutotoolsOutput
{
  GtkTextView parent_instance;
};

struct _AutotoolsOutputClass
{
  GtkTextViewClass parent_class;
};

GType autotools_output_get_type (void) G_GNUC_CONST;

GtkWidget*               autotools_output_new               (AutotoolsConfig *config, 
                                                             CodeSlayer             *codeslayer);

AutotoolsConfig*  autotools_output_get_config (AutotoolsOutput        *output);
CodeSlayer*              autotools_output_get_codeslayer    (AutotoolsOutput        *output);
void                     autotools_output_create_links      (AutotoolsOutput        *output);

G_END_DECLS

#endif /* __AUTOTOOLS_OUTPUT_H__ */
