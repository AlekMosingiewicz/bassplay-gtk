#ifndef __PLAY_H__
#define __PLAY_H__

#define DIR_UP 1
#define DIR_DOWN 0


HMUSIC start_playback(const char* filename);
void pause_playback(HMUSIC music);
void stop_playback(HMUSIC music);
void adjust_volume(HMUSIC music, int dir, float *old_volume, float amount);

#endif