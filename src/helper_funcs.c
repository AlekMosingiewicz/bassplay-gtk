#include <sys/stat.h>
#include <pwd.h>

#include "helper_funcs.h"
#include "globals.h"
#include "callbacks.h"


/** global variables */
GtkObject *glb_song_length_adjustment;
HMUSIC glb_music;
char *glb_basedir;
pthread_t glb_update_thread;
char *glb_file_being_played;
char *glb_actual_file_name;
gboolean glb_playback_loop;
float glb_playback_volume;
char total_playback_time[256];
char current_playback_time[256];



/**********************************
 Open the file open dialog
 and get the name of the file
 we are going to open
 *********************************/

char* 
get_filename()
{
	char *filename;
	char *cpfilename = NULL;
	GtkWidget *glb_window;
	GtkFileFilter *filter = create_standard_mod_filter ();
	GtkFileFilter *generic_filter = create_generic_file_filter ();
	
	GtkWidget* filedialog = gtk_file_chooser_dialog_new
							("Choose the music module",
	                          glb_window,
	                          GTK_FILE_CHOOSER_ACTION_OPEN,
	                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	                          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
	                          NULL);

	gtk_file_chooser_add_filter(filedialog, filter);
	gtk_file_chooser_add_filter (filedialog, generic_filter);
	
	if ( glb_basedir != NULL )
		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(filedialog),
			                                     glb_basedir);
	
	if(gtk_dialog_run(GTK_DIALOG(filedialog)) == GTK_RESPONSE_ACCEPT)
	{
		if (glb_file_being_played != NULL)
			g_free(glb_file_being_played);
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(filedialog));	
		cpfilename = strdup(filename);
		dirname(cpfilename);
		glb_basedir = strdup(cpfilename);
		glb_file_being_played = strdup(filename);		
	}
		
	else filename = NULL;
	gtk_widget_destroy (filedialog);
	if(cpfilename != NULL) 
		g_free(cpfilename);
	return filename;
}

/***************************************************
   Create standard module file filter
 **************************************************/

static GtkFileFilter* 
create_standard_mod_filter()
{
	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern (filter, "*.it");
	gtk_file_filter_add_pattern (filter, "*.mo3");
	gtk_file_filter_add_pattern (filter, "*.mod");
	gtk_file_filter_add_pattern (filter, "*.s3m");
	gtk_file_filter_add_pattern (filter, "*.xm");
	gtk_file_filter_set_name(filter, "Supported mod formats.");
	return filter;
}

/*********************************************
  Create a standard file filter
 ********************************************/

static GtkFileFilter*
create_generic_file_filter ()
{
	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.*");
	gtk_file_filter_set_name(filter, "All file formats");
	return filter;
}

/***************************************************
 This function gets called whenever the user clicks
 on the open dialog OR the 'Play' button
 **************************************************/

int 
choose_and_begin_playback (char *filename)
{
	extern HMUSIC glb_music;

	if(filename == NULL)
		filename = get_filename();
	printf("Filename: %s\n", filename);
	if(filename == NULL) return 0;
	glb_actual_file_name = get_filename_from_path (strdup(filename));
	printf("Actual filename: %s\n", glb_actual_file_name);
	if( glb_music != 0 ) 
	{
		stop_playback (glb_music);
		BASS_MusicFree(glb_music);
	}
	if((glb_music = start_playback(filename)) != 0)
	{
		glb_update_thread = launch_detached_thread (NULL, gui_periodic_update);
		if(glb_playback_volume > -1)
			BASS_ChannelSetAttribute(glb_music, BASS_ATTRIB_VOL, glb_playback_volume);
		BASS_ChannelSetSync(glb_music, BASS_SYNC_END, 0, &on_music_end, NULL);
		return TRUE;
	}
	return -1;
}

/***************************************************
 This function gets called when the file is loaded
 and the GUI has to be updated with the data
 of the module
 **************************************************/

void
populate_gui_with_module_data()
{
	update_label_text (PLAYBACK_STATE_PLAYING);
	populate_module_info_window ();
	setup_music_slider ();
}

/*****************************************
 Set the value for the label that contains
 the length of the song
 ***************************************/

void
set_total_time_label (float secs)
{
	extern GtkBuilder *glb_builder;

	GtkWidget *ttimelabel = GTK_LABEL(gtk_builder_get_object 
	                                  (glb_builder, "totaltimelabel"));
	mins_from_secs (secs, total_playback_time);
	gtk_label_set_text (ttimelabel, total_playback_time);
}

/*****************************************
 Pick the filename from the command line
 instead of the GUI
 *****************************************/

void
command_line_play (char *filename)
{
	if(!choose_and_begin_playback (filename))
	{
		handle_bass_error (NULL);
		return;
	}
	populate_gui_with_module_data ();
	glb_update_thread = launch_detached_thread (NULL, gui_periodic_update);
	update_label_text (PLAYBACK_STATE_PLAYING);
}

/*****************************************
 Display error message in case of a BASS
 library error
 ****************************************/

void 
handle_bass_error (void(*handle_error_func)(int))
{
	GtkWidget *errordialog;
	extern GtkWidget *glb_window;
	if(BASS_ErrorGetCode()) 
		{
			errordialog = gtk_message_dialog_new(glb_window,
		                                     GTK_DIALOG_DESTROY_WITH_PARENT,
		                                     GTK_MESSAGE_ERROR,
		                                     GTK_BUTTONS_CLOSE,
		                                     "Something went wrong;"
		                                     "error code %d",
		                                     BASS_ErrorGetCode());
			gtk_dialog_run(GTK_DIALOG(errordialog));
			gtk_widget_destroy(errordialog);
		}
	
	if(handle_error_func!=NULL)
		handle_error_func(BASS_ErrorGetCode());
}

/*********************************
 Convert the time in
 seconds to a string containing
 minutes and seconds
 ********************************/

void 
mins_from_secs(float secs, char *ret)
{
	int mins;
	int rsecs;

	mins = secs/60;
	rsecs = (int)secs%60;
	sprintf(ret, "%02d:%02d", mins, rsecs);
}

/*********************************
 Update the position of the slider
 to match the current playback
 position
 ********************************/

void
update_position_slider ()
{
    DWORD position;
	gdouble seconds;
	extern HMUSIC glb_music;

	position =	BASS_ChannelGetPosition(glb_music, BASS_POS_BYTE);
	seconds  = (gdouble) BASS_ChannelBytes2Seconds(glb_music, position);

	gtk_adjustment_set_value(GTK_ADJUSTMENT (glb_song_length_adjustment),
		                         seconds);
}

/**************************************
 Update the text in case the playback
 state changes.
 *************************************/

void
update_label_text (char *playback_state)
{
	GtkWidget *title_label;
	char  message[1024];
	char  *song_title;
	extern GtkBuilder *glb_builder;
	extern HMUSIC glb_music;

	title_label = GTK_LABEL(gtk_builder_get_object (glb_builder,
	                                                "titlelabel"));
	song_title = BASS_ChannelGetTags(glb_music,BASS_TAG_MUSIC_NAME);
	if(song_title!=NULL)
	if((!strlen(song_title))&&(glb_actual_file_name != NULL)) 
		song_title = glb_actual_file_name;
	sprintf(message, "%s: %s", playback_state, song_title);
	gtk_label_set_label (title_label, message);	
	
}

/**************************************
 Setup the music slider after the
 module is loaded
 *************************************/

void
setup_music_slider ()
{
	GtkWidget *slider;
	DWORD length;
	float length_sec;
	extern GtkBuilder *glb_builder;
	extern HMUSIC glb_music;

	slider = (GtkWidget*) gtk_builder_get_object (glb_builder, "songslider");
	if(glb_song_length_adjustment !=NULL)
	{
		gtk_object_destroy (glb_song_length_adjustment);
	}

	length = BASS_ChannelGetLength(glb_music, BASS_POS_BYTE);
	length_sec = BASS_ChannelBytes2Seconds(glb_music, length);
	glb_song_length_adjustment = gtk_adjustment_new(0,
                                                0,
                                                length_sec,
                                                0.1,
                                                1,
                                                0);
	gtk_range_set_adjustment (GTK_RANGE(slider),
	                          GTK_ADJUSTMENT (glb_song_length_adjustment));
	set_total_time_label(length_sec);
	
}


/*************************************
 A function that does just that - 
 lanuches a detached thread and passes
 a void pointer as an argument
 *************************************/

pthread_t
launch_detached_thread (void* data, void(*thread_func)(void*))
{
	int tthread;
	pthread_t tthread_id;
	pthread_attr_t thread_attr;

	pthread_attr_init (&thread_attr);
	pthread_attr_setdetachstate (&thread_attr, PTHREAD_CREATE_DETACHED);
	tthread = pthread_create (&tthread_id, &thread_attr, thread_func, data);
	pthread_attr_destroy (&thread_attr);
	return tthread_id;
}

/**************************************
 Init the global variables
 *************************************/

void
init_variables ()
{
   glb_music = 0; //init the music handle
   glb_basedir = NULL;
   glb_song_length_adjustment = NULL;
   glb_file_being_played = NULL;
   glb_actual_file_name = NULL;
   glb_playback_loop = FALSE;
   if(setup_session () < 0)
		fprintf(stderr, "Can't find session data file;"
		        "path to resource:%s/%s", SESSION_DATA_DIR,
		        SESSION_DATA_FILE);
   glb_update_thread = 0;
   glb_playback_volume = -1;
} 

void
populate_general_info_window ()
{
	char general_info[1024];
	char* position = general_info;
	char  length[256];
	char* filename;
	extern HMUSIC glb_music;

	if(glb_actual_file_name == NULL) filename = get_filename_from_path 
		(strdup(glb_file_being_played));
	else filename = glb_actual_file_name;
	mins_from_secs (BASS_ChannelBytes2Seconds(glb_music,
	                                          BASS_ChannelGetLength(glb_music,
	                                                                BASS_POS_BYTE)),
	                length);

	position+= sprintf(position, "Filename: %s\n", filename);
	position+= sprintf(position, "Module length: %s\n", length);

	populate_text_view ("moduleinfo_general_textview", general_info);
}

/**************************************
 List sound samples used in the module
 **************************************/

void 
populate_sample_view()
{
	char  *sample;
	char  *sample_text = malloc(1024*5); // -> allocating the sample text in the stack frame
	int    n=0;                          // caused segmentation fault errors, so I moved 
	char*  position=sample_text;        // it to the heap
	extern HMUSIC glb_music;

	printf("Populating sample window...");
	while(sample = BASS_ChannelGetTags(glb_music, BASS_TAG_MUSIC_SAMPLE+n))
	      {
			  position += sprintf(position, "%d : %s\n", n, sample);
			  n++;
		  }

	populate_text_view ("moduleinfo_instruments_textview", sample_text);
	free(sample_text); 
}

/****************************************
 Get the module info window and, if
 necessary, create it
 ***************************************/

GtkWindow*
get_info_window ()
{
	GtkWindow *info_window;
	extern GtkBuilder *glb_builder;
	printf("Getting info window...\n");

	while((info_window = gtk_builder_get_object (glb_builder,"module_info_window"))
	      == NULL)
	{
	    gtk_builder_add_from_file (glb_builder, MODULEINFO_UI_FILE, NULL);
		populate_module_info_window ();
	}
   gtk_window_deiconify (info_window);
   return info_window;
}

/****************************************
 Get the MOD message into the info window
 ****************************************/

void
populate_message_text_view ()
{
	extern HMUSIC glb_music;

	printf("Populating message window...\n");
	char *message_text = BASS_ChannelGetTags (glb_music, BASS_TAG_MUSIC_MESSAGE);
	if(message_text != NULL)
		populate_text_view ("moduleinfo_message_textview", message_text);
	else populate_text_view ("moduleinfo_message_textview", "No message.");
	
}

/**********************************
 Populate any given GTK text view
 with plain text
 *********************************/

static void
populate_text_view (char *id, char *text)
{
	extern GtkBuilder *glb_builder;
	GtkTextView *textview = GTK_TEXT_VIEW ( gtk_builder_get_object 
	                                       ( glb_builder,
	                                         id));
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (textview);
	GtkTextIter iterator;

	gtk_text_buffer_get_iter_at_offset (buffer, &iterator, 0);
	gtk_text_buffer_set_text(buffer, text, -1);
	printf("Text view populated\n");
}

/*********************************
 Populate the module info window
 with module data
 ********************************/

void
populate_module_info_window()
{
	populate_general_info_window ();
	populate_sample_view ();
	populate_message_text_view ();
}

/*************************************
 Restart the playback (for instance when
 playing in the loop)
 ************************************/
void
restart_playback()
{
	extern HMUSIC glb_music;

	BASS_ChannelSetPosition(glb_music, MAKELONG(0,0), BASS_POS_MUSIC_ORDER|
	                   BASS_MUSIC_POSRESET);
	update_position_slider ();
	update_time_label (0);
	BASS_ChannelPlay(glb_music, FALSE);
}
	

/**************************************
 Update the label which tells us
 at which point of time in the song
 we are
 *************************************/

void
update_time_label(float secs)
{
	extern GtkBuilder *glb_builder;
	extern HMUSIC glb_music;

	if(secs < 0)
	 secs = 
		BASS_ChannelBytes2Seconds(glb_music,
		                          BASS_ChannelGetPosition(glb_music, BASS_POS_BYTE));
	GtkWidget *tlabel = GTK_LABEL(gtk_builder_get_object 
	                              (glb_builder, "currtimelabel"));
	
	mins_from_secs (secs, current_playback_time);
	gtk_label_set_text(tlabel, current_playback_time);
}


/**************************************
 Wrapper for quickly displaying the
 gtk message dialog
 *************************************/

void 
show_info_quick (char* message, GtkMessageType type)
{
	GtkWidget *messagedialog;
	extern GtkWidget *glb_window;

	messagedialog = gtk_message_dialog_new(glb_window,
		                                     GTK_DIALOG_DESTROY_WITH_PARENT,
		                                     type,
		                                     GTK_BUTTONS_CLOSE,
		                                     message,
		                                     BASS_ErrorGetCode());
	gtk_dialog_run(GTK_DIALOG(messagedialog));
	gtk_widget_destroy(messagedialog);
	
}

/************************************
 Check if file exitsts under a given path
 ***********************************/ 
gboolean
check_file_exists (char *filename)
{
	struct stat file;
	int status = stat(filename, &file);
	if(status == 0) return TRUE;
	return FALSE;
}
 
/**************************************
 Save session data (preferably) at exit
 *************************************/

int 
save_session_data(char *filename)
{
	FILE* ofile;
	struct stat dir;
	char  output_path[1024];
	char* home_dir = get_home_dir ();

	if(stat(home_dir, &dir)<0) mkdir(home_dir, S_IRWXU);
	
	sprintf(output_path, "%s/%s", home_dir, filename);
	if((ofile = fopen(output_path, "w")) == NULL)
		return -1;
	printf("Saving file being played: %s\n", glb_file_being_played);
	fprintf(ofile, "%s %s", glb_basedir, glb_file_being_played);
	fclose(ofile);
	g_free(home_dir);
	return 0;
}

/**********************************
 Get size of a file or input stream
 *********************************/

size_t
get_filesize(FILE *file)
{
	size_t filesize;
	
	if(file == NULL) return -1; //refusing to work with empty files
	fseek(file, 0, SEEK_END);
	filesize = ftell(file);
	rewind(file);
	
	return filesize;
}

/*****************************************
 Validate if the data stored in session is
 correct. If a pointer to error message is set,
 the error message is stored in the buffer
 *****************************************/
gboolean
validate_session_data(char *message_buffer)
{
	char fullpath[256];
	char local_msg[256] = "\0";
	
	gboolean status = TRUE;	
	sprintf(fullpath, "%s/%s", glb_basedir, glb_file_being_played);

	if(!check_file_exists (fullpath))
	{
		sprintf(local_msg, "Error opening file");
		status = FALSE;
	}

	if((status == FALSE) && (strlen(local_msg) > 0) && (message_buffer != NULL) )
	   {
		   sprintf(message_buffer, "%s", local_msg);
	   }
	
	return status;
}

/**********************************
 Restore session data from file
 *********************************/

int 
restore_session_data (char* filename)
{
	char tmp_d[1024];
	char tmp_f[256];
	FILE *ifile = fopen(filename, "r");
	if(ifile == NULL) return -1;
	fscanf(ifile, "%s", tmp_d);
	fscanf(ifile, "%s", tmp_f);
	fclose(ifile);
	glb_basedir = strdup(tmp_d);
	glb_file_being_played = strdup(tmp_f);
	return 1;
}

/*************************************
 Get home directory for saving session data
 ************************************/

char* 
get_home_dir ()
{
	struct passwd *pwd = getpwuid(getuid());
	char *home_dir = (char*)malloc(256);
	sprintf(home_dir, "%s/%s", pwd->pw_dir, SESSION_DATA_DIR);
	return home_dir;
}


/*************************************
 Setup the session at the application
 startup
 ************************************/

int
setup_session ()
{
	char path[256];
    
	sprintf(path, "%s/%s", get_home_dir (), SESSION_DATA_FILE);
	if( !restore_session_data (path) )
		return -1;
	return 1;
}

/**********************************
 Check if there's data to save
 at program exit
 *********************************/

int
check_if_session_data_is_empty ()
{
	if((glb_file_being_played == NULL) || (glb_basedir == NULL)) 
		return -1;
	return 0;
}

/**********************************
 Extract the file name from path
 **********************************/

char* 
get_filename_from_path (char *path)
{
	if(path == NULL) return NULL;
	char *actual_filename;
	char *tmp = strtok(path, "/");
	while (tmp!=NULL)
	{
		actual_filename = strdup(tmp);
		tmp = strtok(NULL, "/");
	}
	return actual_filename;
}

/******************************
 Take care of the session data
 while leaving the application
 *****************************/

void
handle_session_data_on_exit()
{
	if(check_if_session_data_is_empty () != 0) return; //no data to save
	if(save_session_data(SESSION_DATA_FILE) < 0)
	{
		char message[256];
		sprintf(message, "Unable to save session data,"
		        "check what permissions are set for the"
		        " %s directory", SESSION_DATA_DIR);
		show_info_quick (message, GTK_MESSAGE_ERROR);
	}
}