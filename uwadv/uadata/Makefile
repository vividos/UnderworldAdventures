#
# Makefile for the "uadata" folder
#
# $Id$
#

all: uadata

# all lua scripts

lua_SCRIPTS = uw1/scripts/cutscene.lua

lua_OBJECTS = uw1/scripts/cutscene.lob


# various targets

uadata: $(lua_OBJECTS) $(lua_SCRIPTS)
	# zip -r *.lob uadata.uar

clean:
	rm -f $(lua_OBJECTS) uadata.uar


# lua object (lob) building rule

%.lob: %.lua
	./luac -o $(*).lob $(<)
