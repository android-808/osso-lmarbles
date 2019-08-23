# Makefile.am for OSSO LMarbles

INCLUDES = $(GDEPS_CFLAGS) $(SDL_CFLAGS) $(SDLMIX_CFLAGS) $(SDLIMAGE_CFLAGS)

#INCLUDES = @GDEPS_CFLAGS@ @SDL_CFLAGS@

bin_PROGRAMS = osso_lmarbles
launcherdir=@wrapperdir@/games
launcher_PROGRAMS = osso_lmarbles.l

osso_lmarbles_CFLAGS= $(INCLUDES) \
	-DLMARBLESLOCALEDIR=\"$(lmarbleslocaledir)\"\
	-DLMARBLESGFXDIR=\"$(lmarblesgfxdir)\"\
	-DLMARBLESSOUNDSDIR=\"$(lmarblessoundsdir)\"\
	-DLMARBLESLEVELSDIR=\"$(lmarbleslevelsdir)\"\
	-DLMARBLESPROFILEDIR=\"$(lmarblesprofiledir)\"\
	-DLMARBLESSRCDIR=\"$(top_srcdir)\"\
	-DPREFIX=\"$(prefix)\" -I$(top_srcdir)\
	-DSAPP_FUNC=1 $(OSSO_CFLAGS)

osso_lmarbles_SOURCES = \
	main.c i18n.h \
	callbacks.c callbacks.h \
	cfg.c cfg.h \
        file.c file.h \
	levels.c levels.h \
	profile.c profile.h \
	timer.c timer.h \
	dynlist.c dynlist.h \
        game.c game.h \
	sdl.c sdl.h \
	audio.c audio.h \
	ft.c ft.h \
	stack.c stack.h \
	engine_main.c engine_main.h  

osso_lmarbles_LDADD = $(GDEPS_LIBS) $(SDL_LIBS) -lhgw $(SDLMIX_LIBS) $(SDLIMAGE_LIBS) $(OSSO_LIBS)

osso_lmarbles_l_SOURCES = \
        launcher/lmarbles.c

plugin_PROGRAMS = plugin.so
plugindir = @prefix@/lib/osso-lmarbles

plugin_so_CFLAGS = $(GTK_CFLAGS) $(GCONF_CFLAGS) $(OSSO_CFLAGS) -fPIC

plugin_so_LDFLAGS = -shared
plugin_so_LDADD = $(OSSO_LIBS)

plugin_so_SOURCES = plugin/plugin.c
