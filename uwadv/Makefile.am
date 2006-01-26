#
# Underworld Adventures - an Ultima Underworld remake project
# Copyright (c) 2002,2003,2004,2005,2006 Underworld Adventures Team
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Makefile.am for Underworld Adventures
#
# $Id$
#

AUTOMAKE_OPTIONS = foreign

SUBDIRS = hacking source uadata

bin_SCRIPTS = linux/ua-lowercase-all.pl

docbookdir = $(prefix)/share/doc/uwadv
if have_xsltproc
docbook_html = docs/docbook/uwadv-manual.html
DOCBOOKXSL=http://docbook.sourceforge.net/release/xsl/current/html/docbook.xsl

docs/docbook/uwadv-manual.html: ${top_srcdir}/docs/docbook/uwadv-manual.xml
	$(XSLTPROC) $(XSLTPROC_FLAGS) -o $@ $(DOCBOOKXSL) $<
else
docbook_html =
endif

EXTRA_DIST =  \
	autogen.sh \
	Copying \
	uwadv.spec \
	Makefile.mingw \
	uwadv.dsw \
	docs/Authors.txt \
	docs/Changes \
	docs/License.installer.txt \
	docs/README.dev.txt \
	docs/README.Linux.txt \
	docs/README.Lua.txt \
	docs/README.mingw32.txt \
	docs/README.music.txt \
	docs/README.tools.txt \
	docs/README.uwadv.txt \
	docs/ua-doxygen.cfg \
	docs/ua-todo-list.txt \
	docs/uw1-keyboard.txt \
	docs/uw1-walkthrough.txt \
	docs/uw2-walkthrough.txt \
	docs/uw-formats.txt \
	docs/docbook/uwadv-manual.xml $(docbook_html) \
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
	misc/uw1-object-classes.txt \
	misc/uw1-objects.png \
	misc/uw1-palette0.png \
	win32/animview.dsp \
	win32/cnvdbg.dsp \
	win32/map3ds.dsp \
	win32/mapdisp.dsp \
	win32/strpak.dsp \
	win32/tools.dsw \
	win32/toolslib.dsp \
	win32/uaconfig.dsp \
	win32/uadebug.def \
	win32/uadebug.dsp \
	win32/uaopt.dsp \
	win32/uwadv.cfg \
	win32/uwadv.dsp \
	win32/uwadv.nsi \
	win32/wrlview.dsp \
	win32/xmi2mid.dsp


cfgdir = $(datadir)/games/uwadv
cfg_DATA = linux/uwadv.cfg

man6_MANS = linux/uwadv.6 \
	linux/ua-lowercase-all.pl.6

linux/uwadv.cfg linux/uwadv.6: %: %.in
	rm -f $@
	sed -e 's,@UW1PATH\@,@UW1PATH@,g' \
       -e 's,@UW2PATH\@,@UW2PATH@,g' \
       -e 's,@UADATAPATH\@,@UADATAPATH@,g' \
       -e 's,@UADOCPATH\@,@datadir@/doc/uwadv,g' $< >$@
