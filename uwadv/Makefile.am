#
# Makefile.am for Underworld Adventures
#
# $Id$
#

AUTOMAKE_OPTIONS = foreign

SUBDIRS = hacking source uadata

bin_SCRIPTS = linux/ua-lowercase-all.pl

EXTRA_DIST =  \
	autogen.sh \
	Copying \
	uwadv.spec \
	Makefile.mingw \
	uwadv.dsw \
	docs/Authors.txt \
	docs/README.dev.txt \
	docs/README.Linux.txt \
	docs/README.Lua.txt \
	docs/README.mingw32.txt \
	docs/README.music.txt \
	docs/README.tools.txt \
	docs/README.uwadv.txt \
	docs/ua-diffs.txt \
	docs/ua-doxygen.cfg \
	docs/ua-todo-list.txt \
	docs/uw1-keyboard.txt \
	docs/uw1-walkthrough.txt \
	docs/uw2-walkthrough.txt \
	docs/uw-formats.txt \
	linux/uwadv.spec.in \
	linux/uwadv.cfg.in \
	linux/ua-lowercase-all.pl \
	linux/uwadv.6.in \
	linux/ua-lowercase-all.pl.6 \
	misc/anmformt.zip \
	misc/ua-objtex00.png \
	misc/ua-objtex01.png \
	misc/uw1-combinations.txt \
	misc/uw1-common-obj-prop.txt \
	misc/uw1-levelobjs-level1.txt \
	misc/uw1-object-classes.txt \
	misc/uw1-objects.png \
	misc/uw1-palette0.png

cfgdir = $(datadir)/games/uwadv
cfg_DATA = linux/uwadv.cfg

man6_MANS = linux/uwadv.6 \
	linux/ua-lowercase-all.pl.6

linux/uwadv.cfg linux/uwadv.6: %: %.in
	rm -f $@
	sed -e 's,@UW1PATH\@,@UW1PATH@,g' \
            -e 's,@UADATAPATH\@,@UADATAPATH@,g' \
            -e 's,@UADOCPATH\@,@datadir@/doc/uwadv,g' $< >$@
