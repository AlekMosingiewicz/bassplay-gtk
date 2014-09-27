#ifndef _BASSPLAY_THREADS_H_
#define _BASSPLAY_THREADS_H_

/*****************************
 Threads launched by the 
 bassplay
 ****************************/
#include <gtk/gtk.h>
#include <bass/bass.h>
#include <pthread.h>
#include "globals.h"

void gui_periodic_update(void*);

#endif