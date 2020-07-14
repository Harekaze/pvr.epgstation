noinst_LTLIBRARIES = libepgstation.la

libepgstation_la_SOURCES = \
	api.cpp \
	api.h \
	epgstation.h \
	recorded.cpp \
	recorded.h \
	reserves.cpp \
	reserves.h \
	rules.cpp \
	rules.h \
	schedule.cpp \
	schedule.h

libepgstation_la_LDFLAGS = \
	-avoid-version

AM_CPPFLAGS = \
	$(AM_CPPFLAGS_EXT) \
	-std=c++11 \
	-I. \
	-I$(top_srcdir)/include -I$(top_srcdir)/src

EXTRA_DIST = \
	include

all: libepgstation.la
