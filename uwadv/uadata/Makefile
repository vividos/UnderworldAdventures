#
# Makefile for the "uadata" folder
#
# $Id$
#

all: uadata

# all lua scripts

lua_SCRIPTS = uw1/scripts/cutscene.lua

lua_OBJECTS = uw1/scripts/cutscene.lob


zip_DATA = $(lua_OBJECTS)

# various targets

uadata: uadata00.uar

uadata00.uar: $(lua_OBJECTS) $(lua_SCRIPTS)
	zip uadata00.uar $(zip_DATA)

clean:
	rm -f $(zip_DATA) uadata00.uar


# lua object (.lob) building rule

%.lob: %.lua
	./luac -o $(*).lob $(<)
