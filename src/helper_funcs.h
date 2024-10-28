#ifndef _HELPER_FUNCS_H_
#define _HELPER_FUNCS_H_

/** Subroutines called by the callback functions **/

#include "globals.h"
#include "bassplay_threads.h"
#include <pthread.h>

/** Generic functions **/
void  command_line_play(char *filename);
char* get_filename();
int   choose_and_begin_playback(char *filename);
void  populate_gui_with_module_data();
void  handle_bass_error(void(*handle_error_func)(int));
void  update_position_slider ();
void  update_label_text (char*);
void  setup_music_slider ();
void  init_variables();
char* get_filename_from_path(char *path);
char* read_file(char *path);
size_t get_filesize(FILE *file);
void  populate_module_info_window();
void  populate_general_info_window();
void  populate_message_text_view();
void  populate_sample_view ();
GtkWindow* get_info_window();
static void  populate_text_view(char *id, char *text);
static GtkFileFilter* create_standard_mod_filter();
static GtkFileFilter* create_generic_file_filter();
gboolean check_file_exists(char *filename);

/** Playback control related functions **/
void loop_playback();

/** Playback data related functions **/
void mins_from_secs(float secs, char *ret);
void set_total_time_label(float secs);
void update_time_label(float);

/** GUI-related functions **/

void show_info_quick(char* message, GtkMessageType type); //gtk dialog wrapper
GtkWidget* get_file_menu();

/** History-related functions **/
void handle_history_on_open(HMUSIC music, const char *path);
void append_history_data(const char *name, const char *path);
void save_history_on_exit();
void init_history_on_startup();
void history_render_menu();
void history_clear_menu();

/** Functions related to saving and retrieving
    the session data **/

int           save_session_data();
int           restore_session_data(char* filename);
int           setup_session();
char*         get_home_dir();
int           check_if_session_data_is_empty();
void          handle_session_data_on_exit();
gboolean   validate_session_data(char *message_buffer);

/** Multithreading **/

pthread_t  launch_detached_thread(void* data, void(*thread_func)(void*));

#endif