#
# Makefile.am for uadata resources
#
# $Id$
#

LUAC = ../source/lua/luac

# this defines luaSCRIPTS, luaOBJECTS and uarFILES
include Makefile.common

uadata_DATA = uadata00.uar uw1/keymap.cfg
uadatadir = $(datadir)/games/uwadv

EXTRA_DIST = \
	$(uarFILES) \
	Makefile.common

CLEANFILES = \
	uadata00.uar \
	$(luaOBJECTS)

uadata00.uar: $(uarFILES)
	@mkdir -p $(@D)
	zip $@ $+

%.lob : %.lua
	@mkdir -p $(@D)
	$(LUAC) -o $@ $<
