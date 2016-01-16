
feniceincludedir = $(top_srcdir)/include/fenice

noinst_LTLIBRARIES = liblog.la
liblog_la_SOURCES = log.c
			
EXTRA_DIST = log.h

liblog_la_LDFLAGS = -static
INCLUDES = -I$(feniceincludedir) -I$(top_srcdir)
