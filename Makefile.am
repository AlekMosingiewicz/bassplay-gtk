## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po

bassplay_gtkdocdir = ${prefix}/doc/bassplay_gtk
bassplay_gtkdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = $(bassplay_gtkdoc_DATA) \
	$(INTLTOOL_FILES)

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(bassplay_gtkdocdir)
