/***************************************************************************
 *            history.h
 *
 *  nie listopad 02 13:27:18 2014
 *  Copyright  2014  Alek
 *  <user@host>
 ****************************************************************************/
/*
 * history.h
 *
 * Copyright (C) 2014 - Alek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "globals.h"

#ifndef __HISTORY_H__
#define __HISTORY_H__

/********************
 * Song entry structure
 *******************/

typedef struct
{
	char *name;
	char *path;
} song_entry;

typedef song_entry* psong_entry;

/***************
 * Vector for storing song entries
 **************/ 
typedef struct 
{
	int capacity;
	int count;
	psong_entry *entries;
} song_list;

/**************************
 * Data allocation and storage
 **************************/
song_entry* song_entry_new(const char *name, const char *path);
void song_entry_destroy(song_entry *entry);

song_list* song_list_new(int initial_count);
void song_list_append(song_list *list, song_entry *entry);
static void song_list_resize_if_needed(song_list *list);
static void song_list_double_capacity(song_list *list);
void song_list_shuffle(song_list *list);
void song_list_append_front(song_list *list, song_entry *entry);
void song_list_destroy(song_list *list);


/****************************
 * Data navigation
 * **************************/

song_entry* song_list_first(song_list *list);
song_entry* song_list_last(song_list *list);

/***************************
 Data serialization
 **************************/

char* song_entry_to_string(song_entry *entry, char *string);
char* song_list_to_string(song_list *list, char *output_string, int forward);
song_list* song_list_from_string(char *input_string);

#endif