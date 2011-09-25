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

#ifndef __AUTOTOOLS_PROJECTS_POPUP_H__
#define	__AUTOTOOLS_PROJECTS_POPUP_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AUTOTOOLS_PROJECTS_POPUP_TYPE            (autotools_projects_popup_get_type ())
#define AUTOTOOLS_PROJECTS_POPUP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), AUTOTOOLS_PROJECTS_POPUP_TYPE, AutotoolsProjectsPopup))
#define AUTOTOOLS_PROJECTS_POPUP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), AUTOTOOLS_PROJECTS_POPUP_TYPE, AutotoolsProjectsPopupClass))
#define IS_AUTOTOOLS_PROJECTS_POPUP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AUTOTOOLS_PROJECTS_POPUP_TYPE))
#define IS_AUTOTOOLS_PROJECTS_POPUP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), AUTOTOOLS_PROJECTS_POPUP_TYPE))

typedef struct _AutotoolsProjectsPopup AutotoolsProjectsPopup;
typedef struct _AutotoolsProjectsPopupClass AutotoolsProjectsPopupClass;

struct _AutotoolsProjectsPopup
{
  GtkMenuItem parent_instance;
};

struct _AutotoolsProjectsPopupClass
{
  GtkMenuItemClass parent_class;

  void (*make) (AutotoolsProjectsPopup *menu);
  void (*make_install) (AutotoolsProjectsPopup *menu);
  void (*make_clean) (AutotoolsProjectsPopup *menu);
  void (*configure) (AutotoolsProjectsPopup *menu);
  void (*autoreconf) (AutotoolsProjectsPopup *menu);
};

GType autotools_projects_popup_get_type (void) G_GNUC_CONST;
  
GtkWidget*  autotools_projects_popup_new  (void);
                                            
G_END_DECLS

#endif /* __AUTOTOOLS_PROJECTS_POPUP_H__ */
