## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = audio sidplay-base.spec sidplay.xpm POINTER

BUILT_SOURCES = audiodrv.cpp audiodrv.h

DISTCLEANFILES = $(BUILT_SOURCES)

bin_PROGRAMS = sidplay sid2wav sidcon

sidplay_SOURCES = sidplay.cpp
sidplay_LDADD = audiodrv.o @SIDPLAY_LDFLAGS@ -lsidplay $(LIBAUDIO)

sid2wav_SOURCES = sid2wav.cpp
sid2wav_LDADD = @SIDPLAY_LDFLAGS@ -lsidplay

sidcon_SOURCES = sidcon.cpp
sidcon_LDADD = @SIDPLAY_LDFLAGS@ -lsidplay

INCLUDES = @SIDPLAY_INCLUDES@
