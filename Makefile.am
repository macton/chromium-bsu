MAINTAINERCLEANFILES = \
	configure \
	Makefile.in \
	config.h.in \
	config.h.in~ \
	chromium-bsu-config.h.in \
	chromium-bsu-config.h.in~ \
	config.sub \
	config.guess \
	aclocal.m4 \
	depcomp \
	install-sh \
	missing \
	ABOUT-NLS \
	config.rpath \
	mkinstalldirs \
	mkinstalldirs~ \
	po/Makefile.in.in \
	po/Makevars.template \
	po/Rules-quot \
	po/boldquot.sed \
	po/en@boldquot.header \
	po/en@quot.header \
	po/insert-header.sin \
	po/quot.sed \
	po/remove-potcdate.sin \
	po/*.pot \
	m4/acx_pthread.m4 \
	m4/ax_check_gl.m4 \
	m4/ax_check_glu.m4 \
	m4/ax_check_glut.m4 \
	m4/ax_lang_compiler_ms.m4 \
	m4/codeset.m4 \
	m4/gettext.m4 \
	m4/glibc2.m4 \
	m4/glibc21.m4 \
	m4/iconv.m4 \
	m4/intdiv0.m4 \
	m4/intlmacosx.m4 \
	m4/intmax.m4 \
	m4/inttypes-pri.m4 \
	m4/inttypes.m4 \
	m4/inttypes_h.m4 \
	m4/inttypes-h.m4 \
	m4/isc-posix.m4 \
	m4/lcmessage.m4 \
	m4/lib-ld.m4 \
	m4/lib-link.m4 \
	m4/lib-prefix.m4 \
	m4/lock.m4 \
	m4/longdouble.m4 \
	m4/longlong.m4 \
	m4/nls.m4 \
	m4/po.m4 \
	m4/printf-posix.m4 \
	m4/progtest.m4 \
	m4/signed.m4 \
	m4/size_max.m4 \
	m4/stdint_h.m4 \
	m4/uintmax_t.m4 \
	m4/ulonglong.m4 \
	m4/visibility.m4 \
	m4/wchar_t.m4 \
	m4/wint_t.m4 \
	m4/xsize.m4 \
	m4/pkg.m4

EXTRA_DIST = \
	config.rpath \
	README.install

ACLOCAL_AMFLAGS = -I m4 --install

SUBDIRS = \
	po \
	misc \
	src \
	m4 \
	data

dist-hook:
	LC_ALL=C git log --pretty --numstat --summary $(VERSION) | git2cl > $(distdir)/ChangeLog
