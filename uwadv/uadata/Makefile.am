LUAC = ../source/lua/luac

# this defines luaSCRIPTS, luaOBJECTS and uarFILES
include Makefile.common

uadata_DATA = uadata00.uar
uadatadir = $(datadir)/games/uwadv

EXTRA_DIST = \
	$(luaSCRIPTS) \
	uw1/scripts/uwinit.txt \
	Makefile.common

CLEANFILES = \
	uadata00.uar \
	$(luaOBJECTS)

uadata00.uar: $(uarFILES)
	zip uadata00.uar $(uarFILES)

%.lob : %.lua
	$(LUAC) -o $@ $<
