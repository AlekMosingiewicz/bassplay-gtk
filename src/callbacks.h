/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * callbacks.h
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

#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__

#include <gtk/gtk.h>
#include "globals.h"
#include "helper_funcs.h"

/* GTK callbacks */
void destroy (GtkWidget *widget, gpointer data);
void on_menu_exit_click (GtkWidget *widget, gpointer data);
void on_menu_file_open_click (GtkWidget *widget, gpointer data);
void on_stop_button_click (GtkWidget *widget, gpointer data);
void on_play_button_click (GtkWidget *widget, gpointer data);
void on_pause_button_click (GtkWidget *widget, gpointer data);
void on_volume_control_change (GtkWidget *widget, gpointer data);
void on_about_click (GtkWidget *widget, gpointer data);
void on_slider_change (GtkRange *widget, GtkScrollType scroll, gdouble value, 
                       gpointer user_data);
gboolean on_slider_release(GtkWidget *widget, gpointer data);
void on_label_click(GtkWidget *widget,gpointer data);
void on_loop_toggled(GtkWidget *widget, gpointer data);

void on_history_item_selected(GtkWidget *widget, gpointer data);

/* BASS callbacks */

void CALLBACK on_music_end(HSYNC hmusic, DWORD channel, DWORD data, void *user);
void CALLBACK on_position_change(HSYNC synd, DWORD channel, DWORD data, void* user);

#endif