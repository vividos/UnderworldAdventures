LUAC = ../source/lua/luac

# this defines luaSCRIPTS, luaOBJECTS and uarFILES
include Makefile.common

uadata_DATA = uadata00.uar
uadatadir = $(datadir)/games/uwadv

EXTRA_DIST = \
	$(luaSCRIPTS) \
	uw1/scripts/uwinit.txt \
	uw1/audio/music.m3u \
	uw1/keymap.cfg \
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
