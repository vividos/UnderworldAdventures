Underworld Adventures Readme File
---------------------------------

Underworld Adventures is a project to recreate Ultima Underworld 1 on modern
operating systems (e.g. Win32, Linux or MacOS), using the original game files.
It uses OpenGL for 3D graphics, the SDL library for platform specific tasks
and Lua for scripting. It is developed as Open Source and is licensed under
the GNU General Public License.

Underworld Adventures is Copyright (C) 2002,2003 Underworld Adventures Team

Underworld Adventures is available from
   http://uwadv.sourceforge.net/


Release Notes for "0.8-hot-chili"
---------------------------------

This release has a lot of new features inside. Picking up objects from the
underworld was built in (via the "get"-icon). Lava and water textures are now
animated, giving a nice effect; also NPC's are now animated. Added drawing of
levers, buttons and pull chains (although they lack functionality for now,
since doors are not rendered yet). Bridges are now added to allow the
adventurer to visit more of the underworld.

Finally a savegame feature was added (press CTRL+R or CTRL+S in game),
together with a quicksave (F11) and quickload (F12) functionality. The 3d
models incorporated in the last release were too small, due to a bug, which is
now fixed, too. Some of them can e.g. be found in the rat's room on the first
level, at Lakshi Longtooth's campfire on level 4 or in the abyss' library on
the 6th level. Happy hunting!


What already works
------------------

Here's a short list what features already work in Underworld Adventures:

* looking at objects in the underworld and inventory("look"-icon)
* picking up objects from the underworld ("get"-icon)
* basic conversation support ("talk"-icon)
* level changing
* basic support for texture and NPC animations
* inventory handling
* mouse-movement
* support for 3d models
* audio soundtrack
* start menu, character creation, acknowledgements
* introduction cutscene
* collision detection, gravity


Setting up Underworld Adventures
--------------------------------

To run Underworld Adventures, you need the original Ultima Underworld 1 game
files (or the files from the Ultima Underworld Demo, "uw_demo"). To let
Underworld Adventures know where the game files are, you have to edit the file
"uwadv.cfg" that was installed with Underworld Adventures. The config file
also contains other options that can be set.

For Windows, there is a config program "Underworld Adventures Config" that can
be used to set up options (it is started while installing the game, but can
also found in the Start menu, in "Programs" and then Underworld Adventures).

For Linux users (or the hardcore ones :) just open up the file "uwadv.cfg" in
a text editor and follow the instructions in this file (on Linux the file can
usually be found in "/usr/local/share/games/uwadv/" or respective path).

Linux users also can have per-user configurations that supersede the global
options. Just copy the file "uwadv.cfg" to the folder "~/.uwadv/". The folder
is usually created at the first start of "uwadv".

With Underworld Adventures you can customize the key mappings easily. Just
open the file "keymap.cfg" and follow the instructions in the file (Linux
users don't have a global "keymap.cfg", but the file that was installed to
"/usr/local/share/games/uwadv/" (or respective path) can be copied to the
folder "~/.uwadv/" to customize the key mappings).

There is a music pack with a digitally rearranged soundtrack for Ultima
Underworld 1 available. It is also available at http://uwadv.sourceforge.net/


Playing Underworld Adventures
-----------------------------

The game remake is currently under development, so don't expect all things to
be implemented and working. Underworld Adventures should be controllable just
like the original, so please consult your game manuals or the reference card
for instructions A list of keyboard commands for the original Ultima
Underworld 1 is available in the file "uw1-keyboard.txt" (for Windows users a
shortcut was installed into the "Underworld Adventures" program group.


Remarks
-------

If you want to contribute to the game remake project, please visit the
Underworld Adventures home page, at

   http://uwadv.sourceforge.net/

and look at the "development" page. If you want to send mail, be it
comments, suggestions or bug reports, just write to

   vividos@users.sourceforge.net

Include the keyword "uwadv" in the subject, when possible.
