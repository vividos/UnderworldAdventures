Underworld Adventures Readme File
---------------------------------

Underworld Adventures is a project to recreate Ultima Underworld 1 on modern
operating systems (e.g. Win32, Linux or MacOS), using the original game files.
It uses OpenGL for 3D graphics, the SDL library for platform specific tasks
and Lua for scripting. It is developed as Open Source and is licensed under
the GNU General Public License.

Underworld Adventures is Copyright (C) 2002 Underworld Adventures Team

Underworld Adventures is available from
   http://uwadv.sourceforge.net/


Release Notes for "0.6-rotworm stew"
------------------------------------

There are some new features in this release: 

Mouse movement mode: You can move around the levels using the mouse. Just
place the mouse cursor over the 3d window and left-click to start moving.
During using mouse movement mode you cannot leave the window, as in the
original game.

Looking at objects and walls: You can look at these by left-clicking on them.
For this, you have to be in "default" or "look" command mode. "default" mode
is when no command icons (those on the left side) are on, and "look" mode is
when the "eye" symbol is highlighted.

Level changing: To let the user explore more of the Abyss than just the first
level, there are some keyboard keys to change the levels. Press Alt + PageDown
to descend a level, and Alt + PageUp to move one level up. You are usualy
placed at different interesting spots when changing levels. Users of the
"uw_demo" can use these keys to change positions in the first level.

SPOILER WARNING: Since this is considered cheating, you might see things that
                 you should not see before ending the game. So better don't
                 descend beyond level 8!


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


In Game
-------

The game should behave just as the original. Please consult your game
manuals or reference card for instructions. A list of keyboard commands for
the original Ultima Underworld 1 is available in the file "uw1-keyboard.txt"
(for Windows users a shortcut was installed into the "Underworld Adventures"
program group.


Remarks
-------

The game currently is less than alpha, so don't expect a full game or
features the original game has. If you want to contribute to the game,
please visit the Underworld Adventures home page, at

   http://uwadv.sourceforge.net/

and look at the "development" page. If you want to send mail, be it
comments, suggestions or bug reports, just write to

   vividos@users.sourceforge.net

Include the keyword "uwadv" in the subject, when possible.
