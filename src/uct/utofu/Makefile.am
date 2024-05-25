UTOFU_LDFLAGS = -ltofucom
module_LTLIBRARIES    = libuct_utofu.la
libuct_utofu_la_CPPFLAGS = $(BASE_CPPFLAGS) $(UTOFU_CPPFLAGS)
libuct_utofu_la_CFLAGS   = $(BASE_CFLAGS)
libuct_utofu_la_LIBADD   = $(top_builddir)/src/ucs/libucs.la \
                        $(top_builddir)/src/uct/libuct.la
libuct_utofu_la_LDFLAGS  = $(UTOFU_LDFLAGS) $(NUMA_LIBS) -version-info $(SOVERSION)

noinst_HEADERS = \
	utofu_def.h

libuct_utofu_la_SOURCES = \
	utofu_iface.c \
	utofu_md.c \
	utofu_ep.c


include $(top_srcdir)/config/module.am
