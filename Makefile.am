EXTRA_DIST = \
	autogen.sh \
	data/sounds/alarm.wav \
	data/sounds/arrow.wav \
	data/sounds/click.wav \
	data/sounds/explode.wav \
	data/sounds/score.wav \
	data/sounds/select.wav \
	data/sounds/stop.wav \
	data/sounds/teleport.wav \
	data/levels/Original \
	debian/changelog \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/docs \
	debian/rules

SUBDIRS = src data

deb:	dist
	-mkdir $(top_builddir)/debian-build
	cd $(top_builddir)/debian-build && tar zxf ../$(top_builddir)/$(PACKAGE)-$(VERSION).tar.gz
	cd $(top_builddir)/debian-build/$(PACKAGE)-$(VERSION) && dpkg-buildpackage -rfakeroot
	-rm -rf $(top_builddir)/debian-build/$(PACKAGE)-$(VERSION)
