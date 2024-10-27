/** Global variables and definitions for BASSPLAY **/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <bass/bass.h>
#include <gtk/gtk.h>
#include "play.h"
#include "bassplay_threads.h"

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

#endif