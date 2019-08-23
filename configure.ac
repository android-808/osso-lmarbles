# configure.ac for OSSO Lmarbles

# Mandatory, inits autoconf
AC_INIT([osso-lmarbles], [1.4.34])

# Tests that source dir exists
AC_CONFIG_SRCDIR([src/main.c])

# For automake: generate rules to reproduce config.h, if needed
AM_CONFIG_HEADER([config.h])

# Initialize automake, takes package name and verison form AC_INIT
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_HEADER_STDC

CFLAGS="$CFLAGS -W -Wall -O2 -Wmissing-prototypes -Wmissing-declarations -Wno-unused-parameter"

# Check module depenencies, will automatically include all needed
# compiler options to DEPS-variables, see src/Makefile.am for details
PKG_CHECK_MODULES(GDEPS, glib-2.0 >= 2.2 freetype2 >= 0.15 gconf-2.0 >= 2.2.0)
AC_SUBST(GDEPS_CFLAGS)
AC_SUBST(GDEPS_LIBS)

PKG_CHECK_MODULES(OSSO, libosso )
AC_SUBST(OSSO_CFLAGS)
AC_SUBST(OSSO_LIBS)

SDL_CFLAGS=`sdl-config --cflags`
SDL_LIBS=`sdl-config --libs`
AC_SUBST(SDL_CFLAGS)
AC_SUBST(SDL_LIBS)

SDLMIX_LIBS=""
SDLMIX_CFLAGS=""
AC_CHECK_LIB(SDL_mixer, Mix_OpenAudio, SDLMIX_LIBS="-lSDL_mixer" SDLMIX_CFLAGS="-DSOUND",, ${SDLmix_PRE_LIBS})
AC_SUBST(SDLMIX_LIBS)
AC_SUBST(SDLMIX_CFLAGS)


SDLIMAGE_LIBS=""
SDLIMAGE_CFLAGS=""
AC_CHECK_LIB(SDL_image, IMG_Load, SDLIMAGE_LIBS="-lSDL_image" SDLIMAGE_CFLAGS="-DUSE_SDL_IMAGE",, ${SDLimage_PRE_LIBS})
AC_SUBST(SDLIMAGE_LIBS)
AC_SUBST(SDLIMAGE_CFLAGS)



PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.2 hildon-1 >= 0.8.1)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(GCONF, gconf-2.0 >= 2.2.0)
AC_SUBST(GCONF_CFLAGS)
AC_SUBST(GCONF_LIBS)

#Localization-related
GETTEXT_PACKAGE=osso-games
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED( GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Name of gettext package] )
ALL_LINGUAS="de_DE en_GB en_US es_ES es_MX fi_FI fr_CA fr_FR it_IT pt_BR ru_RU"
AM_GLIB_GNU_GETTEXT

wrapperdir=/usr/games/wrapper
AC_SUBST(wrapperdir)

lmarblesgfxdir=/usr/games/lmarbles/gfx
AC_SUBST(lmarblesgfxdir)

lmarblessoundsdir=/usr/share/osso-lmarbles/sounds
AC_SUBST(lmarblessoundsdir)

lmarbleslevelsdir=/usr/share/osso-lmarbles/levels
AC_SUBST(lmarbleslevelsdir)

lmarblesprofiledir=/usr/share/osso-lmarbles/profile
AC_SUBST(lmarblesprofiledir)

lmarbleslocaledir=${datadir}/locale
AC_SUBST(lmarbleslocaledir)

desktopentrydir=${datadir}/applications
AC_SUBST(desktopentrydir)

hildondesktopentrydir=/usr/share/applications/hildon
AC_SUBST(hildondesktopentrydir)

hildoncpdesktopentrydir='$PKG_CONFIG --variable=desktopentrydir hildon-control-panel'
hildonpixmapdir='$PKG_CONFIG --variable=hildonpixmapdir hildon-base'
hildonstatusbarlibdir='$PKG_CONFIG --variable=hildonstatusbarlibdir hildon-base'
AC_SUBST(hildoncpdesktopentrydir)
AC_SUBST(hildonpixmapdir)
AC_SUBST(hildonstatusbarlibdir)

dbuslibpathdir=`pkg-config --variable=libdir dbus-1`
dbusconfdir=/etc/dbus-1/system.d
dbusservdir=${dbuslibpathdir}/dbus-1.0/services
AC_SUBST(dbuslibpathdir)
AC_SUBST(dbusconfdir)
AC_SUBST(dbusservdir)

AC_CONFIG_FILES([
Makefile
src/Makefile
src/launcher/lmarbles.c
data/Makefile
data/osso_lmarbles.desktop
data/osso_lmarbles.game
data/com.nokia.osso_lmarbles.service
data/com.nokia.osso_lmarbles.startup.service
])


# Mandatory, produce otput
AC_OUTPUT
