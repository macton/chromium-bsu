MAINTAINERCLEANFILES = \
	configure \
	Makefile.in \
	config.h.in \
	config.h.in~ \
	config.sub \
	config.guess \
	aclocal.m4 \
	depcomp \
	install-sh \
	missing \
	m4/acx_pthread.m4 \
	m4/ax_check_gl.m4 \
	m4/ax_check_glu.m4 \
	m4/ax_check_glut.m4 \
	m4/ax_lang_compiler_ms.m4 \
	m4/pkg.m4

EXTRA_DIST = README.install

ACLOCAL_AMFLAGS = -I m4 --install

SUBDIRS = \
        misc \
	src \
	m4 \
	data

dist-hook:
	svn2cl --authors svn2cl-authors.txt --include-rev --output $(distdir)/ChangeLog https://chromium-bsu.svn.sourceforge.net/svnroot/chromium-bsu/chromium-bsu/trunk
