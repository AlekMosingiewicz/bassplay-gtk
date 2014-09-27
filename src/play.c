/** Music module playback using BASS **/

#include <bass/bass.h>
#include "play.h"

HMUSIC start_playback(const char* filename)
{
	
	HMUSIC music = BASS_MusicLoad(
	                              FALSE,
	                              filename,
	                              0,
	                              0,
	                              BASS_MUSIC_PRESCAN,
	                              0
	                              );
	BASS_ChannelPlay(music, FALSE);
	return music;
}

void pause_playback(HMUSIC music)
{
	if(BASS_ChannelIsActive(music)==BASS_ACTIVE_PAUSED)
		BASS_ChannelPlay(music, FALSE);
	else BASS_ChannelPause(music);
}

void stop_playback(HMUSIC music)
{
	BASS_ChannelStop(music);
	BASS_ChannelSetPosition(music, MAKELONG(0,0), BASS_POS_MUSIC_ORDER|
	                        BASS_MUSIC_POSRESET);
}

void adjust_volume(HMUSIC music, int dir, float *old_volume, float amount)
{
	
	switch(dir)
	{
		case DIR_UP:
			if((*old_volume + amount) < 1)
			   (*old_volume)+=amount;
			break;
		case DIR_DOWN:
			if((*old_volume - amount) > 0)
			   (*old_volume)-=amount;
			break;
	}
	BASS_ChannelSetAttribute(music,
			                 BASS_ATTRIB_VOL,
			                 *old_volume
			                 );
}