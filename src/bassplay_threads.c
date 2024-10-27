#include "bassplay_threads.h"
#include "helper_funcs.h"

/***********************************
 The thread that updates the GUI
 with the playback data
 **********************************/


void 
gui_periodic_update (void *data)
{
	extern char* glb_file_being_played;
	char *oldfilename = strdup(glb_file_being_played);
	extern HMUSIC glb_music;

	while(!strcmp(oldfilename, glb_file_being_played))
	{
		usleep(500000);
		if (BASS_ChannelIsActive(glb_music) == BASS_ACTIVE_PLAYING)
		{
			gdk_threads_enter();
			update_position_slider ();
			update_time_label (-1);
			gdk_threads_leave();
		}
	}
	g_free(oldfilename);
	pthread_exit(NULL);
}
