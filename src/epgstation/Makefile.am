noinst_LTLIBRARIES = libepgstation.la

libepgstation_la_SOURCES = \
	../base64/base64.cpp \
	api.cpp \
	recorded.cpp \
	reserves.cpp \
	rules.cpp \
	schedule.cpp \
	storage.cpp

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
