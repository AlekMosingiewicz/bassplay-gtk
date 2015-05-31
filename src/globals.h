/** Global variables and definitions for BASSPLAY **/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <bass/bass.h>
#include <gtk/gtk.h>
#include "play.h"
#include "bassplay_threads.h"
#include "history.h"

/*****************************
 UI Files
 ****************************/

//#define UI_FILE PACKAGE_DATA_DIR "/ui/bassplay_gtk.ui"
#define UI_FILE "src/bassplay_gtk.ui"
#define MODULEINFO_UI_FILE "src/module_info.ui"
//#define MODULEINFO_UI_FILE PACKAGE_DATA_DIR "/ui/module_info.ui"

/*****************************
 Playback states
 ****************************/

#define PLAYBACK_STATE_PLAYING "Playing"
#define PLAYBACK_STATE_STOPPED "Stopped"
#define PLAYBACK_STATE_PAUSED  "Paused"

/*****************************
 Default directory for saving
 session data
 ****************************/

#define SESSION_DATA_DIR "/.bassplay"
#define SESSION_DATA_FILE "session.dat"

/****************************
 Default size parameters
 for writing session data
 ***************************/

#define SESSION_BASEDIR_SIZE 256
#define SESSION_FILENAME_SIZE 256

/*******************************
 Default parameters for storing playback
 history
 ******************************/

#define MAX_HISTORY_SIZE 10
#define HISTORY_FILE "history.dat"

/*****************************
 GUI-related globals
 ****************************/
GtkBuilder *builder;
GtkWidget  *window;
GtkWidget  *tlabel;
GtkWidget  *file_menu;
GtkObject  *song_length_adjustment;

/****************************
 Thread IDs
 ***************************/

pthread_t update_thread;

/****************************
 Playback parameters
 ***************************/
float playback_volume;
float playback_speed;
int playback_in_progress;
gboolean playback_loop;

/****************************
 Playback data
 ***************************/
char total_playback_time[5];
char current_playback_time[5];

/****************************
 BASS engine-related
 variables
 ****************************/
HMUSIC music;
BASS_INFO song_info;

/**************************
 * Playback history
 *************************/
song_list *history;
GtkMenuItem *history_item_handles[MAX_HISTORY_SIZE];


/****************************
 Additional info
 ****************************/
char *basedir; //the base directory used by the player
char *file_being_played; //full path to the file being played
char *actual_file_name; //the sole file name to be used as label


#endif