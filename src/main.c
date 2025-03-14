/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Aleksander Mosingiewicz 2011 <almos666@gmail.com>
 * 
 * bassplay is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * bassplay is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>
#include <gdk/gdk.h>

#include <config.h>

#include <gtk/gtk.h>
#include "globals.h"



/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif



#include "callbacks.h"

/* For testing use the local (not installed) ui file */

GtkWidget *glb_window;
GtkBuilder *glb_builder;
	
GtkWidget*
create_window (void)
{
	GtkWidget *window;
	GError* error = NULL;

	glb_builder = gtk_builder_new ();
	/** Add main UI **/
	if ((!gtk_builder_add_from_file (glb_builder, UI_FILE, &error))||
	    (!gtk_builder_add_from_file (glb_builder, MODULEINFO_UI_FILE, &error)))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}


	/* This is important */
	gtk_builder_connect_signals (glb_builder, NULL);
	window = GTK_WIDGET (gtk_builder_get_object (glb_builder, "window"));

	
	return window;
}


int
main (int argc, char *argv[])
{

   BASS_Init(-1, 44100, 0, NULL, NULL); //start the BASS library
   init_variables (); //init the global variables

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	gtk_set_locale ();
	g_thread_init(NULL);
    gdk_threads_init();
	gtk_init (&argc, &argv);

	glb_window = create_window ();
	gtk_widget_show (glb_window);
	if(argc > 1) command_line_play (argv[1]);
	gtk_main ();
	BASS_Free(); //free the resources used by the BASS engine
	g_object_unref (glb_builder); //free GTK builder
	return 0;
}
