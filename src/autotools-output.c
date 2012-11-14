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

#include <stdlib.h>
#include "autotools-output.h"

typedef struct
{
  gchar *file_path;
  gint   line_number;
  gint   start_offset;
  gint   end_offset;
} Link;

static void autotools_output_class_init      (AutotoolsOutputClass *klass);
static void autotools_output_init            (AutotoolsOutput      *output);
static void autotools_output_finalize        (AutotoolsOutput      *output);

static void clear_links                      (AutotoolsOutput      *output);
static GList* mark_links                     (AutotoolsOutput      *output, 
                                              GtkTextBuffer        *buffer, 
                                              GList                *matches);
static GList* find_matches                   (gchar                *text);
static Link* create_link                     (AutotoolsOutput      *output,
                                              gchar                *text,
                                              GtkTextIter          *begin, 
                                              GtkTextIter          *end);
static gboolean select_link_action           (AutotoolsOutput      *output, 
                                              GdkEventButton       *event);
static gboolean notify_link_action           (AutotoolsOutput      *output, 
                                              GdkEventButton       *event);

#define AUTOTOOLS_OUTPUT_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), AUTOTOOLS_OUTPUT_TYPE, AutotoolsOutputPrivate))

typedef struct _AutotoolsOutputPrivate AutotoolsOutputPrivate;

struct _AutotoolsOutputPrivate
{
  AutotoolsConfiguration *configuration;
  CodeSlayer             *codeslayer;
  GList                  *links;
  GtkTextTag             *underline_tag;
};

G_DEFINE_TYPE (AutotoolsOutput, autotools_output, GTK_TYPE_TEXT_VIEW)
      
static void 
autotools_output_class_init (AutotoolsOutputClass *klass)
{
  G_OBJECT_CLASS (klass)->finalize = (GObjectFinalizeFunc) autotools_output_finalize;
  g_type_class_add_private (klass, sizeof (AutotoolsOutputPrivate));
}

static void
autotools_output_init (AutotoolsOutput *output) 
{
  AutotoolsOutputPrivate *priv;
  GtkTextBuffer *buffer;
  
  priv = AUTOTOOLS_OUTPUT_GET_PRIVATE (output);
  
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (output));
  
  priv->links = NULL;
  gtk_text_view_set_editable (GTK_TEXT_VIEW (output), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (output), GTK_WRAP_WORD);

  priv->underline_tag = gtk_text_buffer_create_tag (buffer, "underline", "underline", 
                                                    PANGO_UNDERLINE_SINGLE, NULL);
                                                    
  g_signal_connect_swapped (G_OBJECT (output), "button-press-event",
                            G_CALLBACK (select_link_action), output);
  g_signal_connect_swapped (G_OBJECT (output), "motion-notify-event",
                            G_CALLBACK (notify_link_action), output);
}

static void
autotools_output_finalize (AutotoolsOutput *output)
{
  clear_links (output);
  G_OBJECT_CLASS (autotools_output_parent_class)->finalize (G_OBJECT (output));
}

GtkWidget*
autotools_output_new (AutotoolsConfiguration *configuration, 
                      CodeSlayer             *codeslayer)
{
  GtkWidget *output;
  AutotoolsOutputPrivate *priv;
 
  output = g_object_new (autotools_output_get_type (), NULL);
  priv = AUTOTOOLS_OUTPUT_GET_PRIVATE (output);
  priv->configuration = configuration;
  priv->codeslayer = codeslayer;

  return output;
}

AutotoolsConfiguration* 
autotools_output_get_configuration (AutotoolsOutput *output)
{
  AutotoolsOutputPrivate *priv;
  priv = AUTOTOOLS_OUTPUT_GET_PRIVATE (output);
  return priv->configuration;
}

CodeSlayer* 
autotools_output_get_codeslayer (AutotoolsOutput *output)
{
  AutotoolsOutputPrivate *priv;
  priv = AUTOTOOLS_OUTPUT_GET_PRIVATE (output);
  return priv->codeslayer;
}

void
autotools_output_create_links (AutotoolsOutput *output)
{
  AutotoolsOutputPrivate *priv;
  GtkTextBuffer *buffer;
  GtkTextIter start;
  GtkTextIter end;
  gchar *text;
  GList *matches;

  priv = AUTOTOOLS_OUTPUT_GET_PRIVATE (output);
  
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (output));

  gtk_text_buffer_get_bounds (buffer, &start, &end);
  
  text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
  
  matches = find_matches (text);
  clear_links (output);
  priv->links = mark_links (output, buffer, matches);
  g_list_foreach (matches, (GFunc) g_free, NULL);
  g_free (text);
}

static void
clear_links (AutotoolsOutput *output)
{
  AutotoolsOutputPrivate *priv;
  GList *list;
  
  priv = AUTOTOOLS_OUTPUT_GET_PRIVATE (output);

  if (priv->links == NULL)
    return;
  
  list = priv->links;
  
  while (list != NULL)
    {
      Link *link = list->data;
      g_free (link->file_path);
      g_free (link);
      list = g_list_next (list);
    }
  
  g_list_free (priv->links);
  priv->links = NULL;
}

static GList*
find_matches (gchar *text)
{
  GList *results = NULL;
  GRegex *regex;
  GMatchInfo *match_info;
  GError *error = NULL;
  
  regex = g_regex_new ("(\\/.*?\\.[ch]:\\d+)", 0, 0, NULL);
  
  g_regex_match_full (regex, text, -1, 0, 0, &match_info, &error);
  
  while (g_match_info_matches (match_info))
    {
      gchar *match_text = NULL;
      match_text = g_match_info_fetch (match_info, 1);
      
      g_print ("match %s\n", match_text);
      results = g_list_prepend (results, g_strdup (match_text));
        
      g_free (match_text);
      g_match_info_next (match_info, &error);
    }
  
  g_match_info_free (match_info);
  g_regex_unref (regex);
  
  if (error != NULL)
    {
      g_printerr ("search text for completion word error: %s\n", error->message);
      g_error_free (error);
    }

  return results;    
}

static GList*
mark_links (AutotoolsOutput *output, 
            GtkTextBuffer   *buffer, 
            GList           *matches)
{
  GList *results = NULL;

  while (matches != NULL)
    {
      gchar *match_text = matches->data;
      GtkTextIter start, begin, end;

      gtk_text_buffer_get_start_iter (buffer, &start);
      
      while (gtk_text_iter_forward_search (&start, match_text, 
                                           GTK_TEXT_SEARCH_TEXT_ONLY, 
                                           &begin, &end, NULL))
        {
          Link *link;
          gtk_text_buffer_apply_tag_by_name (buffer, "underline", &begin, &end);
          
          link = create_link (output, match_text, &begin, &end);
          if (link != NULL)
            results = g_list_prepend (results, link);
          
          start = begin;
          gtk_text_iter_forward_char (&start);
        }
      
      matches = g_list_next (matches);
    }
    
  return results;    
}

static Link*
create_link (AutotoolsOutput *output,
             gchar           *text,
             GtkTextIter     *begin, 
             GtkTextIter     *end)
{
  Link *link = NULL;
  gchar **split, **tmp;
  
  split = g_strsplit (text, ":", 0);  
  
  if (split != NULL)
    {
      tmp = split;

      link = g_malloc (sizeof (Link));
      link->file_path = g_strdup (*tmp);
      tmp++;
      link->line_number = atoi(*tmp);
      link->start_offset = gtk_text_iter_get_offset (begin);
      link->end_offset = gtk_text_iter_get_offset (end);
      
      g_strfreev(split);
    }

  return link;
}

static gboolean
select_link_action (AutotoolsOutput *output, 
                    GdkEventButton  *event)
{
  AutotoolsOutputPrivate *priv;

  priv = AUTOTOOLS_OUTPUT_GET_PRIVATE (output);

  if ((event->button == 1) && (event->type == GDK_BUTTON_PRESS))
    {
      GdkWindow *window;
      GtkTextIter iter;
      gint offset, x, y, bx, by;
      GList *list;

      window = gtk_text_view_get_window (GTK_TEXT_VIEW (output),
                                         GTK_TEXT_WINDOW_TEXT);
                                                                                                                
      gdk_window_get_device_position (window, event->device, &x, &y, NULL);      
      
      gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (output),
                                             GTK_TEXT_WINDOW_TEXT,
                                             x, y, &bx, &by);

      gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (output),
                                          &iter, bx, by);
      
      offset = gtk_text_iter_get_offset (&iter);
      
      list = priv->links;
      
      while (list != NULL)
        {
          Link *link = list->data;
          
          if (offset >= link->start_offset && offset <= link->end_offset)
            {
              CodeSlayerDocument *document;
              CodeSlayerProject *project;
              document = codeslayer_document_new ();
              codeslayer_document_set_file_path (document, link->file_path);
              codeslayer_document_set_line_number (document, link->line_number);
              
              project = codeslayer_get_project_by_file_path (priv->codeslayer, 
                                                             link->file_path);
              codeslayer_document_set_project (document, project);
              
              codeslayer_select_editor (priv->codeslayer, document);
              g_object_unref (document);
              return FALSE;
            }
          list = g_list_next (list);
        }
    }

  return FALSE;
}

static gboolean
notify_link_action (AutotoolsOutput *output, 
                    GdkEventButton  *event)
{

  AutotoolsOutputPrivate *priv;
  GdkWindow *window;
  GdkCursor *cursor;
  GtkTextIter iter;
  gint x, y, bx, by;

  priv = AUTOTOOLS_OUTPUT_GET_PRIVATE (output);

  window = gtk_text_view_get_window (GTK_TEXT_VIEW (output),
                                     GTK_TEXT_WINDOW_TEXT);
                                                                                                                
  gdk_window_get_device_position (window, event->device, &x, &y, NULL);      

  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (output),
                                         GTK_TEXT_WINDOW_TEXT,
                                         x, y, &bx, &by);
      
  gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (output),
                                      &iter, bx, by);

  cursor = gdk_window_get_cursor (window);

  if (gtk_text_iter_has_tag (&iter, priv->underline_tag))
    {
      if (cursor == NULL || gdk_cursor_get_cursor_type (cursor) != GDK_HAND1)
        {
          cursor = gdk_cursor_new (GDK_HAND1);
          gdk_window_set_cursor (window, cursor);
          g_object_unref (cursor);    
        }
    } 
  else 
    {
      if (cursor != NULL && gdk_cursor_get_cursor_type (cursor) != GDK_XTERM)
        {
          cursor = gdk_cursor_new (GDK_XTERM);
          gdk_window_set_cursor (window, cursor);
          g_object_unref (cursor);    
        }
    }
      
  return FALSE;
}
