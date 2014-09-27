#include "bassplay_threads.h"
#include "helper_funcs.h"

/***********************************
 The thread that updates the GUI
 with the playback data
 **********************************/


void 
gui_periodic_update (void *data)
{
	char *oldfilename = strdup(file_being_played);
	while(!strcmp(oldfilename, file_being_played))
	{
		usleep(500000);
		if (BASS_ChannelIsActive(music) == BASS_ACTIVE_PLAYING)
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
