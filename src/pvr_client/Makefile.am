LIB = ../$(ADDONNAME)

lib_LTLIBRARIES = libpvrepgstation.la

libpvrepgstation_la_SOURCES = \
	channel.cpp \
	demux.cpp \
	information.cpp \
	initialize.cpp \
	live.cpp \
	recording.cpp \
	stream.cpp \
	timer.cpp

libpvrepgstation_la_LIBADD = \
	../epgstation/libepgstation.la

libpvrepgstation_la_LDFLAGS = \
	$(LDFLAGS_EXT) \
	-avoid-version

AM_CPPFLAGS = \
	$(AM_CPPFLAGS_EXT) \
	-std=c++11 \
	-Wall \
	-I. \
	-I$(top_srcdir)/include -I$(top_srcdir)/src

EXTRA_DIST = \
	include

$(ADDONNAME): libpvrepgstation.la
	cp -f .libs/libpvrepgstation.$(LIBEXT) $(ADDONNAME)

$(LIB): $(ADDONNAME)
	cp -f $(ADDONNAME) $(LIB)

all: $(LIB)

clean-local:
	rm -f $(ADDONNAME)
