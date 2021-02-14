## Process this file with automake to produce Makefile.in

docdir = $(datadir)/doc/@PACKAGE@-@VERSION@
appdir = $(datadir)/applications

SUBDIRS = src

doc_DATA = \
  AUTHORS \
  COPYING \
  ChangeLog \
  README \
  $(NULL)

EXTRA_DIST = $(doc_DATA)

squeaky :
	$(MAKE) distclean
	rm -rf \
	Output \
	Makefile Makefile.in aclocal.m4 autom4te.cache \
	compile config.log config.status configure depcomp \
	install-sh missing mkinstalldirs \
	config.guess config.sub \
	src/Makefile.in \
	docs/Makefile.in \
	pixmaps/Makefile.in \
	QCADesigner.desktop \
	graph_dialog.desktop \
	QCADesigner-win32.iss \
	po/Makefile po/Makefile.in po/Makefile.in.in \
	po/POTFILES po/*.gmo `find . | grep '.#'` *~ gtk-doc.make \
	docs/reference/Makefile docs/reference/Makefile.in \
	COPYING INSTALL \
	$(NULL)
