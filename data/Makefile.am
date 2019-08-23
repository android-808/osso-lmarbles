dbusdir=$(prefix)/share/dbus-1/services
dbus_DATA=com.nokia.osso_lmarbles.service \
	  com.nokia.osso_lmarbles.startup.service

desktopdir=$(datadir)/applications/hildon
desktop_DATA = osso_lmarbles.desktop

configdir = $(datadir)/osso-lmarbles
config_DATA = osso_lmarbles.conf

dbuswrapperdir=$(datadir)/osso-lmarbles
dbuswrapper_DATA = osso_lmarbles.game

lmarbleslevelsdir =$(datadir)/osso-lmarbles/levels
lmarbleslevels_DATA = levels/Original 

lmarblesprofiledir=$(datadir)/osso-lmarbles/profile
lmarblesprofile_DATA = profile/lmarbles_profile

lmarblessoundsdir =$(datadir)/osso-lmarbles/sounds
lmarblessounds_DATA = sounds/alarm.wav \
sounds/arrow.wav \
sounds/click.wav \
sounds/explode.wav \
sounds/score.wav \
sounds/select.wav \
sounds/stop.wav \
sounds/teleport.wav

edit = sed \
       -e 's,@datadir\@,$(datadir),g' \
       -e 's,@prefix\@,$(prefix),g'   \
       -e 's,@VERSION\@,$(VERSION),g'
	   
osso_lmarbles.conf: osso_lmarbles.conf.in
	        @$(edit) $< > $@

osso_lmarbles.sh: osso_lmarbles.sh.in
	        @$(edit) $< > $@
			
backupconfigurationdir=/etc/osso-backup/applications
backupconfiguration_DATA=osso-lmarbles.conf

EXTRA_DIST = $(dbus_DATA) $(desktop_DATA) $(dbuswrapper_DATA) $(lmarbleslevels_DATA) $(lmarblessounds_DATA) \
	     $(lmarblesprofile_DATA) $(config_DATA) $(backupconfiguration_DATA)
