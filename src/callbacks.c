/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * callbacks.c
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif


#include "callbacks.h"
#include "play.h"
#include "globals.h"

/* GTK callbacks */

void
destroy (GtkWidget *widget, gpointer data)
{
	handle_session_data_on_exit ();
	gtk_main_quit ();
}

void 
on_stop_button_click (GtkWidget *widget, gpointer data)
{
	extern HMUSIC glb_music;

	if(glb_music)
	{
		stop_playback(glb_music);
		update_label_text (PLAYBACK_STATE_STOPPED);
		update_position_slider ();
		update_time_label (0);
	}
}

void
on_menu_exit_click (GtkWidget* widget, gpointer data)
{
	handle_session_data_on_exit ();
	gtk_main_quit();
}

void
on_menu_file_open_click (GtkWidget* widget, gpointer data)
{
	
	if(!choose_and_begin_playback (NULL) )
	{
		handle_bass_error (NULL);
		return;
	}
	populate_gui_with_module_data ();
}

void
on_play_button_click (GtkWidget* widget, gpointer data)
{
	extern char* glb_file_being_played;
	extern HMUSIC glb_music;
	extern pthread_t glb_update_thread;

	if(glb_music == 0)
	{
		if(glb_file_being_played != NULL)
		{
			glb_music = start_playback(glb_file_being_played);
			populate_gui_with_module_data ();
			glb_update_thread = launch_detached_thread (NULL, gui_periodic_update);
		}
		else if(!choose_and_begin_playback (NULL) )
		{
			populate_gui_with_module_data ();
			handle_bass_error (NULL);
			glb_update_thread = launch_detached_thread (NULL, gui_periodic_update);			
			return;
		}

	}
	else if (BASS_ChannelIsActive(glb_music) == BASS_ACTIVE_PAUSED)
		BASS_ChannelPlay(glb_music, FALSE);
	else if (BASS_ChannelIsActive(glb_music) == BASS_ACTIVE_STOPPED)
		BASS_ChannelPlay(glb_music, FALSE);
	update_label_text (PLAYBACK_STATE_PLAYING);
}

void
on_pause_button_click (GtkWidget* widget, gpointer data)
{
	extern HMUSIC glb_music;

	if(glb_music!=0)
	{
		pause_playback(glb_music);
		update_label_text (PLAYBACK_STATE_PAUSED);
	}
}


void
on_volume_control_change (GtkWidget *widget, gpointer data)
{
	float volume;
	extern float glb_playback_volume;
	extern HMUSIC glb_music;
	
	if(glb_music!=0)
	{
		volume = gtk_scale_button_get_value (GTK_SCALE_BUTTON(widget));
		BASS_ChannelSetAttribute(glb_music, BASS_ATTRIB_VOL, volume);
		glb_playback_volume = volume;
	}
}

void
on_slider_change(GtkRange *widget, GtkScrollType scroll, gdouble value, 
                       gpointer user_data)
{
	extern GtkObject *glb_song_length_adjustment;
	extern HMUSIC glb_music;

	if(glb_music!=0)
	{
		if(BASS_ChannelIsActive(glb_music)==BASS_ACTIVE_PLAYING)
		pause_playback (glb_music);
		gtk_adjustment_set_value (GTK_ADJUSTMENT(glb_song_length_adjustment),value);
		update_time_label (value);
		BASS_ChannelSetPosition(glb_music, BASS_ChannelSeconds2Bytes
		                        (glb_music, value), BASS_POS_BYTE);
	}
}

gboolean
on_slider_release (GtkWidget *widget, gpointer data)
{
	extern HMUSIC glb_music;

	if(BASS_ChannelIsActive(glb_music) == BASS_ACTIVE_PAUSED)
	BASS_ChannelPlay(glb_music, FALSE);
	return TRUE;
}

void
on_about_click (GtkWidget *widget, gpointer data)
{
	show_info_quick ("Bassplay-Gtk, a simple Basslib-based mod player\n"
	                 "written by Alek Mosingiewicz", GTK_MESSAGE_INFO);
}

void
on_label_click (GtkWidget *widget, gpointer data)
{
	GtkWindow *info_window = get_info_window ();
	gtk_widget_show_all(info_window);
}

void
on_loop_toggled(GtkWidget *widget, gpointer data)
{
	extern gboolean glb_playback_loop;

	GtkCheckButton *button = (GtkCheckButton*) widget; 
	gboolean toggled = gtk_toggle_button_get_active (button);
	glb_playback_loop = toggled;
}

/* BASS callbacks */

/** This one is called whenever the playback reaches an end **/
void 
CALLBACK on_music_end(HSYNC hmusic, DWORD channel, DWORD data, void *user)
{
	extern gboolean glb_playback_loop;
	extern HMUSIC glb_music;

	if(!glb_playback_loop)
	{
			stop_playback (glb_music);
			update_position_slider ();
			update_label_text (PLAYBACK_STATE_STOPPED);
			update_time_label (0);
	}
	else
		restart_playback ();
}

/** Change the slider position **/
void 
CALLBACK on_position_change(HSYNC synd, DWORD channel, DWORD data, void* user)
{
	update_position_slider ();
}