#ifndef gettext_h
#define gettext_h

#if ENABLE_NLS
	#include <libintl.h>
	#define _(str) gettext(str)
	#define N_(str) gettext_noop(str)
#else
	#define _(str) str
	#define N_(str) str
	#define gettext(str) str
	#define gettext_noop(str) str
#endif

#endif /* gettext_h */
