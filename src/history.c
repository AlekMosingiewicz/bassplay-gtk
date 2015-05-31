/*
 * history.c
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
#include "history.h"

/***********************************************************
 * Create a new song entry
 **********************************************************/ 
song_entry*
song_entry_new(const char *name, const char *path)
{
	song_entry *entry = malloc(sizeof(song_entry));
	entry->name = strdup(name);
	entry->path  = strdup(path);

	return entry;
}

/************************************************************
 * Destroy a song entry and free associated resources
 ***********************************************************/ 
void
song_entry_destroy (song_entry *entry)
{
	free(entry->name);
	free(entry->path);
	free(entry);
}

/********************************************************
 * Create a new instance of a song list
 *******************************************************/ 
song_list*
song_list_new (int initial_count)
{
	song_list *list = g_malloc(sizeof(list));
	list->entries = g_malloc(initial_count * sizeof(psong_entry));
	list->capacity = initial_count;
	list->count = 0;

	return list;
}

/***************************************************
 * Check if the list neads to be resized for new entries
 **************************************************/ 
void
song_list_resize_if_needed (song_list *list)
{
	if(list->count >= (list->capacity - 1)) 
			song_list_double_capacity(list);

	return list;
}

/****************************************************
 * Append the entry to the song list
 ***************************************************/ 
void
song_list_append (song_list *list, song_entry *entry)
{
	if(list == NULL) return;
	if(entry == NULL) return;

	song_list_resize_if_needed (list);
	
	list->entries[list->count] = entry;
	list->count++;
}


void
song_list_append_front (song_list *list, song_entry *entry)
{
	if(list == NULL) return;
	if(entry == NULL) return;

	song_list_resize_if_needed (list);

	g_memmove(list->entries[1], list->entries[0], sizeof(song_entry*)*list->capacity);
	list->entries[0] = entry;
}

/**************************************************
 * Destroy the list and free associated resources
 *************************************************/ 
void
song_list_destroy (song_list *list)
{
	int i;
	song_entry *entry;

	for(i = 0; i < list->count; i++)
	{
		entry = list->entries[i];
		song_entry_destroy(entry);
	}

	free(list);
}

/********************************************************
 * Double a capacity of the list
 *******************************************************/ 
static void
song_list_double_capacity (song_list *list)
{
	list->entries = realloc(list->capacity*=2, sizeof(song_entry*));
}

/************************************************************
 * Serialize a single song entry into string
 ***********************************************************/ 
char*
song_entry_to_string(song_entry *entry, char *string)
{
	int size = strlen(entry->name)
					+ strlen(entry->path)
					+ 10;
	if(string == NULL) string = malloc(size);
	sprintf(string, "\"%s\",\"%s\"", entry->name, entry->path);
	
	return string;
}

/****************************************
 * Shuffle the list
 ***************************************/
void
song_list_shuffle(song_list *list)
{
	int new_position = 0;
	int spare_position = list->capacity + 1;
	int i;
	list->entries = realloc(spare_position, sizeof(song_entry*));
	for(i = 0; i < list->count; i++)
	{
		new_position = list->count - i;
		list->entries[spare_position] = list->entries[new_position];
		list->entries[new_position] = list->entries[i];
		list->entries[i] = list->entries[spare_position];
	}
	list->entries = realloc(list->capacity, sizeof(song_entry*));
}

song_entry*
song_list_first(song_list *list)
{
	return list->entries[0];
}

song_entry*
song_list_last(song_list *list)
{
	return list->entries[(list->count)-1];
}

char*
song_list_to_string(song_list *list, char *output_string, int forward)
{
	char entry_string[256];
	int i;
	char *position;
	
	if(output_string == NULL) 
		output_string = malloc(1024);

	position = output_string;
	if(forward)
	for(i = 0; i < list->count ; i++)
	{
		song_entry *entry = list->entries[i];
		position+=sprintf(position, "%s\n", song_entry_to_string(entry, entry_string));
	}

	else
	{
		for(i = list->count-1; i > 0; i--)
		{
			song_entry *entry = list->entries[i];
			position+=sprintf(position, "%s\n", song_entry_to_string(entry, entry_string));
		}
	}

	return output_string;
}

/************************************************
 * Deserialize the history from input string
 ***********************************************/ 
song_list*
song_list_from_string (char *input_string)
{
	song_list *list = song_list_new(MAX_HISTORY_SIZE);
	song_entry *entry = NULL;
	char *positions[MAX_HISTORY_SIZE];
	char name[256];
	char path[256];
	char* current_entry = strtok(input_string, "\n");
	int i = 0;
	int positions_count = 0;

	while(current_entry!=NULL)
	{
		if(strlen(current_entry) < 3) break;
		positions[positions_count++] = strdup(current_entry);
		current_entry = strtok(NULL, "\n");
	}

	for(i =0; i < positions_count; i++)
	{	
		sscanf(positions[i], "\"%[^\"]\",\"%[^\"]\"", name, path);
		entry = song_entry_new (name, path);
		song_list_append (list, entry);
	}
	
	return list;
}