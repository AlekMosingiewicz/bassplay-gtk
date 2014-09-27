## Process this file with automake to produce Makefile.in

## Created by Anjuta

uidir = $(datadir)/bassplay_gtk/ui
ui_DATA = bassplay_gtk.ui module_info.ui

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(BASSPLAY_GTK_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = bassplay_gtk

bassplay_gtk_SOURCES = \
	helper_funcs.c\
	helper_funcs.h\
	play.c\
	play.h\
	callbacks.c \
	callbacks.h \
	globals.h\
	bassplay_threads.c\
	bassplay_threads.h\
	main.c \
	module_info.ui

bassplay_gtk_LDFLAGS = \
	-Wl,--export-dynamic

bassplay_gtk_LDADD = $(BASSPLAY_GTK_LIBS)

EXTRA_DIST = $(ui_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(datadir)/bassplay_gtk
