<h2>uwadv dev log</h2>

<p>
The dev log is the place where I write about the daily ongoings of the project. It is updated occasionally.
</p>

<span class="devlogHeading">2004-04-07</span>
<p class="devlogBody">
Fixed most doxygen warnings and updated copyright string on modified files. Added uw-formats.txt contribution from Max Gilead. Already released the mingw32 setup packages for compiling and building the installer. And released the music pack. Currently we have two separate packages "uwadv" and "uwadv-src" to release files; maybe we should just use "uwadv"?
</p>

<span class="devlogHeading">2004-04-07</span>
<p class="devlogBody">
Fixed some bugs found through valgrind warnings. My mingw32 compile crashed unexpected when first saving a quicksave savegame and then entering the savegame screen. The error was initialisation order; the info area accessed the ua_savegames_list before it was initialized. Fixed another bug when exiting savegames screen via "exit" button and immediately pressing another button. Updated Underworld Adventures manual. Added lift-off to physics code; should fix walking slopes; it's better now but still a bit jerky. wjp fixed rpm .spec file; libxslt and docbook-xsl is needed for building RPMs. Updated some texts. Fixed debug symbols for mingw32 builds when WITH_DEBUGGING is on (hopefully). My gdb still told me that no debug symbols were found. Added wish from Dominus: the manuals are now only built when doing "make installer".
</p>

<span class="devlogHeading">2004-04-06</span>
<p class="devlogBody">
Updated version info; new version is codenamed <a href="http://www.webtender.com/db/drink/1435" target="_blank">"mojito"</a>. wjp added docbook building to automake Makefiles.
</p>

<span class="devlogHeading">2004-04-01</span>
<p class="devlogBody">
Fixed conversation screen and enabled it. Fixed taking screenshots; also increased screenshot size to 160x100. Removed "save" button on savegame screen when entering with Ctrl+R (restore game). Fixed printing on textscroll when output is longer than a line. Savegames are now made up a bit differently. All tilemaps and objlists now have their own section, with a "tilemaps" section at start that tells how many tilemaps we have. Mapnotes are per-level now. Added notification of underworld callback about level change (to prepare new textures and stuff). Fixed player start position, now set per Lua script. Fixed traps. wjp fixed make dist. And sorry, no april fools this year :-)
</p>

<span class="devlogHeading">2004-03-30</span>
<p class="devlogBody">
Moved setting 3d window viewport into ua_renderer class. Fixed text scroll printing and scrolling. Fixed looking at walls.
</p>

<span class="devlogHeading">2004-03-25</span>
<p class="devlogBody">
Implemented taking screenshots for savegames. Lua: implemented most Lua C functions; they are now callable like C++ objects: "tilemap.get_info()" or "uw.print()". The trick is to put the C functions into global tables. Implemented objects "uw", "player", "tilemap", "objlist", "runes", "conv", "quest", "prop". Updated Lua code to use the new functions.
</p>

<span class="devlogHeading">2004-03-22</span>
<p class="devlogBody">
Removed #if'ed-out savegame code that was once there to save unpacked savegames. The savegame file can be unpacked using gunzip anyhow. Fixed loading level maps. Updated physics code to use the stuff found in Telemachos' new <a href="http://www.peroxide.dk/papers/collision/collision.pdf" target="_blank">paper</a> (well, he released it in July, but I only found it now). Walking slopes still hangs on some occasions. Also added a test suite for physics code. Changed Lua scripting interface a bit to support looking at and using objects.
</p>

<span class="devlogHeading">2004-03-22</span>
<p class="devlogBody">
Fixed savegame dialog under uw_demo; the character creation bitmap I'm using isn't available there, so I only draw some edged boxes that should do. Added Alt-key shortcuts to all controls on uaconfig.
</p>

<span class="devlogHeading">2004-03-18</span>
<p class="devlogBody">
Scheduled new release date to be on 2004-04-10. There also will be a new musicpack, since I've found the "wanderer" and the "maps & legends" tracks on my hard disc, done by QQits long ago (in 2002). I don't know if the tracks are finished, but surely worth listening!
</p>

<span class="devlogHeading">2004-03-17</span>
<p class="devlogBody">
Started to refactor physics code. Telemachos has published a new version of his paper on collision detection and response. So far it seems he didn't change the whole concept, so it should be rather easy to implement his new ideas about swept-sphere stuff.
</p>

<span class="devlogHeading">2004-03-16</span>
<p class="devlogBody">
Finished mouse action code on inventory panel. Renamed script.hpp to scripting.hpp. Fixed getting item category and ua_inventory::is_container() implementation. Added unittest.hpp/.cpp that implements a ua_unittest_run() function. It is used to run all unit tests written using CppUnit and that are registered with the test suite registry. That way we can place test classes all over the source folder (in "test" subfolders) that should test different components of uwadv. Tests should be written for every new class and/or functionality. If a bug occurs, a test that fails should be written, then the bug is fixed and the test has to succeed then. This way we can assure our code always performs correctly, event when a class implementation changes.
</p>

<span class="devlogHeading">2004-03-15</span>
<p class="devlogBody">
Added is_container flag to common object properties; it indicates if an object can contain other items; the 3d barrel also has this flag, but cannot be picked up, but it can be emptied. Added armour and wearable properties. Fixed ua_inventory class by using the is_container flag, the item categories and the scripting method to combine items. Added inventory panel drawing and started to work on the mouse action code.
</p>

<span class="devlogHeading">2004-03-14</span>
<p class="devlogBody">
Tested uwadv on an older system using a 3dfx voodoo banshee card; fixed drawing textures bigger than 256x256 pixels.
</p>

<span class="devlogHeading">2004-03-11</span>
<p class="devlogBody">
Added text edit window ua_textedit_view. Added game events for text edit window to signal aborted or finished editing. Added entering savegame description and saving; screenshot taking has still to be done. Fixed drawing info area. Readded the transparent flag to ua_image::paste_image() to more simplify drawing. Since it's only an inline method, it should be optimized away. Implemented up/down buttons on player stats view.
</p>

<span class="devlogHeading">2004-03-10</span>
<p class="devlogBody">
Fixed bug on text scroll when trying to render an empty line. Added version info resource to uwadv.exe, uaconfig.exe and uadebug.dll; when later using .msi packages we have proper version info. Fixed a bug found in STLdebug mode while initializing savegame screen.
</p>

<span class="devlogHeading">2004-03-09</span>
<p class="devlogBody">
Fixed ua_debug_server_impl::get_message() implementation; A std::vector<char> is now used as temporary buffer to get the message string (and only when it's available at all). Added printing STLport version (when used). Removed code to load savegame format 0, since there are 2 different versions that we cannot distinguish from. Added usage of msxsl for xsl transformation used in docbook creation. Added usage of STLport's STLdebug mode when WITH_DEBUGGING is enabled. Ah, and wxWindows now is wxWidgets.
</p>

<span class="devlogHeading">2004-03-08</span>
<p class="devlogBody">
Implemented updating runeshelf window from data in ua_runes. Fixed some valgrind warnings reported by wjp. Fixed bug in critter frames loading; an array was allocated with new and put in an ua_smart_ptr<>, which deletes the array with delete, not delete[] -> memory leaks or undefined behavior! Added experimentally omitting 1st pass when loading, bit it's still buggy and only saves about 20% loading time. Enabled with define OMIT_1ST_PASS. Added on-the-fly critter frame texture uploading; hlps greatly in cutting time while preparing a level. Modified ua_smart_ptr to have a pointer to an internal struct that holds the managed pointer and the reference count. The smart pointer now has the same size as a normal pointer. Webpage: put html template code into own file that is included where needed.
</p>

<span class="devlogHeading">2004-03-04</span>
<p class="devlogBody">
Added message queue to debugger implementation. Implemented shutdown for debugger on debug server side. Client should then check for messages occasionally, either in the idle loop or via a timer. Other messages from possible code-debuggers are passed via the message queue, too. Implemented via an STL deque, where new messages get pushed at the back, and the debug client gets messages from the front.
</p>

<span class="devlogHeading">2004-03-03</span>
<p class="devlogBody">
Prevented mipmap texture generation for object textures, removing the black squares in object sprites. Added preparing of all critter textures for now. In the future texture preparation will be done on-the-fly, to speed up things. Fixed render_sprite() method and added critter rendering. Finally fixed the savegame preview image rendering on savegame screen. Also added the "smooth ui" option to uaconfig. And added the website to CVS repository.
</p>

<p class="devlogBody">
I'm very sad to tell that <a href="http://www.samandmax.net/" target="_blank">www.samandmax.net</a> reported today that Sam &amp; Max 2 - Freelance Police was cancelled. Darn! That could have been the next game I've bought and played (I'm not playing much games, except for uwadv and uw1, and a bit of Diablo 2). Well, have to start up <a href="http://www.scummvm.org/" target="_blank">ScummVM</a> with the classic Sam &amp; Max then.
</p>

<span class="devlogHeading">2004-03-02</span>
<p class="devlogBody">
Reimplemented text scroll; it now supports color codes used in conversation strings, e.g. \1 or \0. Added some more colors that were not used in the original games. Also added an offset value for text scroll generation. This should help in implementing a text scroll with scroll paper edges, also used in conversations.
</p>

<span class="devlogHeading">2004-03-01</span>
Added new ua_runes class that manages runeshelf and runebag contents. Added move arrows (below compass) handling. Movement is done by simulated key presses.
<p class="devlogBody">
</p>

<span class="devlogHeading">2004-02-29</span>
<p class="devlogBody">
Fixed compiling under linux. Bumped savegame version from 0 to 1 to differentiate between version 0.8 and 0.9. ADded listing skills value on stats panel. Added building manual using xsltproc and docbook-xsl, for mingw32.
</p>

<span class="devlogHeading">2004-02-10</span>
<p class="devlogBody">
Added runebag panel drawing and checking for mouse clicks on runes. Added a panel interface class that is used in ingame screen, and later in the conversation screen as well. Scripting object is now cleaned up in the proper place. Fixed bug with black button background on the character creation screen. Fixed some other minor things. Added some more hotspots to the hotspot window in the debugger.
</p>

<span class="devlogHeading">2004-02-05</span>
<p class="devlogBody">
Updated .nsi script for windows installer; NSIS is soon releasing the 2.0 version.
</p>

<span class="devlogHeading">2004-01-29</span>
<p class="devlogBody">
Fixed ua_screen class bug where right mouse button clicks weren't recognized properly. They weren't used till now so this bug went undetected. Added "power gem", and added attack code so that the gem lights up properly while attacking. The ua_underworld object manages the hit power internally. Added combat keys. Fixed palette usage in save game screen, relative coordinate calculation in 3d view window, fixed loading Lua scripts in create character and cutscene screen.
</p>

<span class="devlogHeading">2004-01-28</span>
<p class="devlogBody">
Added a set_palette() method to ua_image; this way it's more obviously to set the palette than assigning to the reference of the smart pointer. Fixed loading "panels.gr"; special case for panels wasn't recognized. Started reworking the panel window. And the tick() method in ingame screen is now called for every subwindow; needed for animated windows later on.
</p>

<span class="devlogHeading">2004-01-26</span>
<p class="devlogBody">
Added tooltips for all controls on the underworld config utility. Tried to fix syncmail script in CVS.
</p>

<span class="devlogHeading">2004-01-25</span>
<p class="devlogBody">
Added invisible 3d view class that manages mouse movement mode. Implemented mouse movement mode, too. Changed all ua_screen-derived classes to have a ref to ua_game_interface as first parameter in every ctor. The game interface reference is passed on from screen to screen then. Added user action enum values needed for object interaction, e.g. looking, using, talking to objects or getting items. For now all user actions have loose ends into the scripting class.
</p>

<span class="devlogHeading">2004-01-20</span>
<p class="devlogBody">
Removed pointer to texture manager for ua_texture objects; not needed anymore. It was used for checking if the texture is already active, and glBindTexture() doesn't have to be called twice. Pretty obsolete, since OpenGL probably checks this for itself. Because of this the ua_texture::init() method and all ua_texture users were changed. Added method tick() to ua_window, to allow e.g. for animating (sub)windows. And another major change: All screens get a reference to ua_game_interface in the ctor now, since all screens have to be created with new. Because of this all ua_screen classes now have a reference to ua_game_interface as member, and not a pointer anymore. Added invisible 3d view class that handles mouse actions like mouse movement (and limiting cursor to the 3d view when mouse-moving) and icon setting and such.
</p>

<span class="devlogHeading">2004-01-15</span>
<p class="devlogBody">
Added missing key for pressing buttons in menus (the return key). Fixed all sorts of bugs with the command menu, especially with keyboard usage. Added scripting pointer to ua_underworld class, so it can call scripting for various functionality. Added some more scripting interface methods and their dummy implementation for Lua. The method init_new_game() is now called when a new game is started, to initialize player, etc. Added "init-scripting" and "check-files" keywords to game.cfg parsing, but with no implementation. The first one should allow dynamic selection of scripting language, e.g. when someone (in the far future) wants to write his/her scripts in Ruby or so. Of course he/she also has to write the scripting bindings then ;-). The second keyword should specify a config file with a list of files that have to be there to successfully run the game.
</p>

<span class="devlogHeading">2004-01-14</span>
<p class="devlogBody">
Applied block-comment patch from <a href="http://lua-users.org/wiki/LuaPowerPatches" target="_blank">Lua Power Patches</a>. Block comments start with --[[ and end with ]]--.
</p>

<span class="devlogHeading">2004-01-08</span>
<p class="devlogBody">
Built libzzip.a and liblua.a in mingw now, to save some include paths in the uwadv compile command. Added docbook source files for uwadv and uadebug documentation. Reenabled tga texture loading. Added splash screen while init'ing the game; it reads "Underworld Adventures is loading...". Renamed "lua-script" command in game.cfg to "load-script".  Scripting is now initialized with the static ua_scripting::create_scripting() method that takes an enum with the scripting language to load. Added some ua_scripting abstract methods. Some other small changes.
</p>

<span class="devlogHeading">2004-01-05</span>
<p class="devlogBody">
Added starting the debugger and related functionality. Redone and simplified debug interfacing as debug server / debug client class. OS-related stuff now goes into a debug_impl.cpp file (only for win32 for now), Linux stuff doesn't work right now (and never has, probably).
</p>

<span class="devlogHeading">2004-01-01</span>
<p class="devlogBody">
ua_renderer::tick() takes tickrate, not a timespan. Added some doxygen comments.
</p>

<span class="devlogHeading">2003-12-31</span>
<p class="devlogBody">
Fixed some stuff in code and documentation. Added setting texture parameter for tile and object textures.
</p>

<span class="devlogHeading">2003-12-30</span>
<p class="devlogBody">
Enabled master object list loading. Added critter frames loading and moved stuff to renderer subfolder. Added preparing the critter frames as textures. Added critter frames manager (formerly critter pool). Moved 3d model manager to renderer subfolder, too. Added resetting time counting in main game loop when a frame took longer than 5 seconds. Chances are high that I just debugged the code and wanted to continue running the game (only in debug builds). Fixed stock texture loading; an off-by-one error. Changed quadtree class to have a callback function that is called when a visible tile is encountered. Fixed a bug in quadtree class where tiles at the frustum edges were not collected properly. Started to rework object and critter rendering.
</p>

<span class="devlogHeading">2003-12-29</span>
<p class="devlogBody">
During winter holiday I chose to examine uw.exe further. It's interesting how the Looking Glass team built Ultima Underworld. They used Turbo C++ 1.01 (or 1.0) (available for free now at the <a href="http://community.borland.com/museum/" target="_blank">borland museum</a> for programming, but no C++ features. Some modules were compiled for overlaying (switch -Y); Turbo C++ uses Borland's VROOM (?) engine for swapping in/out code segments when code calls some functions in other overlaid modules. This time the uw team could save much conventional memory space. The segments seem to be stored in EMS memory. I played around with decompiling uw.exe with <a href="http://www.itee.uq.edu.au/~cristina/dcc.html" target="_blank">dcc</a>, but because of the calls "outside" the normal memory space into the overlay manager it obviously failed. Additionally the engine loaded the wrong library sigs (Borland 3 was identified). The decompiler engine looks promising, though, but isn't actively developed anymore.
</p>

<span class="devlogHeading">2003-12-27</span>
<p class="devlogBody">
Added imports for properties, player, tilemap and map objects. Added user actions and notify calls to underworld and added method to register callback. Enabled savegame loading. Added setting game mode per command buttons.
</p>

<span class="devlogHeading">2003-12-18</span>
<p class="devlogBody">
Added "smooth-ui" setting in uwadv.cfg; it can be used to get a "pixely" appearance of the user interface. Fixed crash bug in ua_screen where associated screen keymap was NULL but was accessed anyway.
</p>

<span class="devlogHeading">2003-12-14</span>
<p class="devlogBody">
More or less fixed compiling the character generation screen; that one surely needs a redo. Most buttons don't redraw their background properly.
</p>

<span class="devlogHeading">2003-12-10</span>
<p class="devlogBody">
Introduced new class ua_debug_server; no more work on the debugger yet - the changes were only done to complete the uwadv game class code (I don't expect changes here for some time; all code should be pretty encapsulated).
</p>

<span class="devlogHeading">2003-12-03</span>
<p class="devlogBody">
Revised the README.dev.txt document; it now has only startup information; the one and only source code documentation is the doxygen docs now. Improved docs on the "base" module.
</p>

<span class="devlogHeading">2003-11-26</span>
<p class="devlogBody">
Ingame zpos is now in range [0; 128] as the objects are, too. They just didn't use that much precision in the uw1 code for tile positions. Enabled slight fog, which somewhat hides the far clipping plane. Added vitality and mana flasks, gargoyle eyes, text scroll, runeshelf window and active spell area. All window work, but some "update" code and actions has to be written.
</p>

<p class="devlogBody">Added starting music when (re-)entering the screen. Fixed playlist loading (had audio turned off during development, so I didn't notice this until now). Audio tracks can now referenced with an easier-to-remember enum.
</p>

<span class="devlogHeading">2003-11-25</span>
<p class="devlogBody">
Added base class for all ingame_orig windows. Added compass window. Hopefully finally fixed ua_ingame_quad drawing; there still were errors with some window sizes. Nicely documented the class.
</p>

<span class="devlogHeading">2003-11-20</span>
<p class="devlogBody">
Started reimplementing original ingame screen. Revised ua_keymap and removed unneeded functions; the mapping of keymod values and key enums were reversed. Also fixed keymod search: the fact that the user usually only presses one ATL or CTRL key wasn't recognized when searching keys. Some other minor changes. The dot tool is now used with doxygen (if installed) to generate some nice include and collaboration graphs.
</p>

<span class="devlogHeading">2003-11-10</span>
<p class="devlogBody">
Disabled tools by default; they surely won't build :). Added geometry provider class, which is responsible to generate textured triangles for the collision detection/response and the renderer class. An idea would be to use OpenGL display lists for tile triangles (that can be updated, e.g. for moving tiles) that can be stored as tile "metadata"; when rendering the display list indices can be collected, sorted by texture id and thrown at OpenGL in an efficient manner then. Fixed floor texture loading (passed wrong filename to loader function). Moved texture manager to implementation class and added a prepare_level() function that prepares textures and could to the above mentioned display list preparation.
</p>

<span class="devlogHeading">2003-11-06</span>
<p class="devlogBody">
Added a find_key() method to ua_keymap to search for the key enum for a given pressed key. Maybe this is now the most important function and the map association can be reversed. The function is used in ua_screen::process_event() to handle key events and finally call overloaded ua_screen::key_event() methods. The rest of keyboard handling is just a big switch statement with the key enum value.
</p>

<p class="devlogBody">
I'm also implementing an ua_ingame_new screen to test some ui/render classes and concepts. The screen might finally become the new user interface screen.
</p>

<span class="devlogHeading">2003-11-03</span>
<p class="devlogBody">
Added some convenience functions to ua_window. More work on the savegames list window.
</p>

<span class="devlogHeading">2003-11-02</span>
<p class="devlogBody">
Some work on uw2 critter animation files decoding.
</p>

<span class="devlogHeading">2003-10-22</span>
<p class="devlogBody">
Fixed cutscene view screen and re-enabled the introduction. GL_BLEND is now used when rendering mouse cursor. The GL_BLEND state is restored after rendering again.
</p>

<span class="devlogHeading">2003-10-21</span>
<p class="devlogBody">
Fixed acknowledgements screen; it now uses one cutscene image quad and a "fadeout" image quad with the image to fade out, which saves one image quad and a 512x256 texture.
</p>

<span class="devlogHeading">2003-10-20</span>
<p class="devlogBody">
Added border "feature" to ua_image_quad to get seamless sub-windows. When using split-textures (only on older graphic cards) there still is a small seam.
</p>

<span class="devlogHeading">2003-10-01</span>
<p class="devlogBody">
Fixed image quad rendering, and two split-textures are now only used when the OpenGL driver reports a max. texture size of 256 or less.
</p>

<span class="devlogHeading">2003-09-26</span>
<p class="devlogBody">
Changed the ua_palette256 type to be a class and not a typedef anymore. This beautifies the code to create, clone and rotate palettes a lot. Fixed mouse cursor rendering.
</p>

<span class="devlogHeading">2003-09-24</span>
<p class="devlogBody">
Try to fix image quad class; textures were only drawn white, since no texture params were set via glTexParameteri(). Updated splash screen and start menu to use the new ua_image_quad and ua_cutscene implementation. Added check for wxWindows, later used for uadebug. Mostly fixed linking.
</p>

<span class="devlogHeading">2003-09-23</span>
<p class="devlogBody">
Try to fix palette typedef usage. Fixed mouse cursor class, which now is also derived from ua_window. Other small changes.
</p>

<span class="devlogHeading">2003-09-18</span>
<p class="devlogBody">
Object images are now put into stock textures; they previously were hold in an image list, and before that they were copied into two large 256x256 textures. Moved texture loading, cutscene loading and frame extraction to import folder.
</p>

<span class="devlogHeading">2003-09-17</span>
<p class="devlogBody">
Redesigned the ua_image class, replaced ua_image_list with a STL vector of images, rewritten image loading. Redesigned ua_texture and ua_texture_manager class, and fixed texture loading. Added image manager that manages loading and holds game palettes (they previously were managed by the ua_texture_manager). Changed base class of ua_image_quad to ua_window, since this should become our base window "control" for basic image rendering. Specialized windows such as scrolls and the like can be derived from this class then. Other small changes.
</p>

<span class="devlogHeading">2003-09-15</span>
<p class="devlogBody">
wjp fixed the autoconf/automake files for the new directory structure, since the Lua sources were moved by SF staff. Moved files only need to be updated in Makefile.common anyway.
</p>

<span class="devlogHeading">2003-09-10</span>
<p class="devlogBody">
Improved source documentation.
</p>

<span class="devlogHeading">2003-09-09</span>
<p class="devlogBody">
Opened a support request to move the lua source to a subfolder of the "script" folder. Re-added custom language pack loading. Re-enabled cutscene, acknowledgements and savegame screen. A lot more small changes and updates.
</p>

<span class="devlogHeading">2003-09-06</span>
<p class="devlogBody">
Added Lua script class, derived from a scripting base class. Added doxygen grouping to categorize class documentation into different "modules".
</p>

<span class="devlogHeading">2003-09-05</span>
<p class="devlogBody">
Added fading helper class and used it in start splash and start menu screen. Fixed acknowledgements screen. Character creation and cutscene view will be harder to fix, since they use Lua, which is going to go into the scripting folder.
</p>

<span class="devlogHeading">2003-09-04</span>
<p class="devlogBody">
Redone mouse cursor class to be derived from ua_window. Added handling of mouse events via the new ua_window member function mouse_event(). The function is only called if a mouse event occurs in the window the mouse is over. The mouse cursor class is used in start menu screen.
</p>

<span class="devlogHeading">2003-09-02</span>
<p class="devlogBody">
Converted uwadv menu screen and start splash screen to use the new ua_screen base class. Fixed some errors and added a texture manager object to the renderer class to have access to the texture manager. wjp sort of fixed some autotools related files.
</p>

<span class="devlogHeading">2003-09-01</span>
<p class="devlogBody">
Added game interface class that is similar to the old game core interface, only better :). Added some more main game loop stuff and added back game strings loader to fix linking (at least for me, hehe).
</p>

<span class="devlogHeading">2003-08-28</span>
<p class="devlogBody">
Moved most files to their new places; only did header and comment changes and commented out stuff that refuses to work. Thought about an game interface class similar to the core class that glues together game objects and the underworld objects. Scripting will become a separate component and the Lua stuff should be capsulated so that in principle one could choose what scripting language to use. The game interface would also manage the scripting - game objects interfacing.
</p>

<span class="devlogHeading">2003-08-25</span>
<p class="devlogBody">
Moved font class to ui folder and changed the way stuff from uw1 and uw2 are loaded now. There is a general import class (for loading common formats) and two specialized classes for uw1 and uw2 that can be used to load the various resources.
</p>

<span class="devlogHeading">2003-08-24</span>
<p class="devlogBody">
Removed some -Wall warnings, added some GPL headers to automake files and some other small changes.
</p>

<span class="devlogHeading">2003-08-22</span>
<p class="devlogBody">
Continued working on the ua_window and ua_screen base classes. Set up SDL window creation and main game loop again; some stuff is still commented out.
</p>

<span class="devlogHeading">2003-08-21</span>
<p class="devlogBody">
Redone the main() function, which is now os-independent. Commented out most of the ua_uwadv_game (previously ua_game) object and fixed win32 implementation. os-specific objects are created using the ua_game_create() function.
</p>

<span class="devlogHeading">2003-08-20</span>
<p class="devlogBody">
Started redoing the user interface classes, having a ua_window and ua_screen base class. ua_window is a rectangular area on screen, can draw itself and can process messages. ua_screen can do all that and additionally manage ua_window subwindows. drawing and event messages are routed to the subwindows, too.
</p>

<span class="devlogHeading">2003-08-19</span>
<p class="devlogBody">
Started to reorganize the audio folder a. Renamed some classes, files and added comments where needed. Introduced (or better, renamed the ua_audio_interface to) the ua_audio_manager class that now handles all audio stuff.
</p>

<span class="devlogHeading">2003-08-16</span>
<p class="devlogBody">
Committed some left-over files to the repository and set a tag "refactoring_start_20030815". Started to reorganize the whole source tree into more logical components that shouldn't depend on other components, or at least only on one or two. There also could be unit tests for each of the components. Also improving comments and code style while moving around files.
</p>

<p class="devlogBody">
Moved the config file base class, files manager, savegame class, settings, math classes and the utils to a "base" folder. Made some classes independent of all other classes, e.g. Lua script loading in the files manager or savegame info filling from ua_player objects. At the moment the project doesn't compile :)
</p>

<span class="devlogHeading">2003-06-30</span>
<p class="devlogBody">
Done some work on the critter workbench test project. There now is a red dot representing the currently selected (and later evaluated) critter. Adding pathfinding should be the next task, and also a mode to check the pathfinding results. Should be able to borrow some code from Exult or so :)
</p>

<span class="devlogHeading">2003-06-24</span>
<p class="devlogBody">
Added a pathfinding skeleton class to have an overview what the pathfinding algorithm interface should look like. Also started a test project, a "critter AI workbench", a 2d top-down look on a level that can be used to evaluate AI and pathfinding code.
</p>

<span class="devlogHeading">2003-06-23</span>
<p class="devlogBody">
Thought about a new set of Lua functions to simplify critter AI, and to help John McCawley getting into coding some AI. In the last days I tried around creating a msi package (windows installer service) for Underworld Adventures, but hand-editing the many tasks is pretty complicated. Plus I have to download almost the entire Platform SDK, which comes as whopping > 300 MB. <a href="http://mwi2xml.sourceforge.net/" target="_blank">msi2xml</a> would definitely simplify a bit of the work.
</p>

<span class="devlogHeading">2003-06-08</span>
<p class="devlogBody">
More or less fixed the huge loading times for critter animation frames. Frames are loaded into a pre-allocated memory space instead of inserting thousands of ua_image objects into std::vector containers. Also changed the ua_critter code to support animation frames with different texture sizes, but this isn't used yet (code was committed on 2003-06-23). Uploading all animation frames (about 36 MB worth of 32-bit texture data) also takes pretty long. Should be enough when only the current set of frames for an animation currently used has to be uploaded to OpenGL. Worth a try.
</p>

<span class="devlogHeading">2003-05-20</span>
<p class="devlogBody">
Added some more screenshots showing 3d objects and wall decals.
</p>

<span class="devlogHeading">2003-05-15</span>
<p class="devlogBody">
Things might get slower a bit (I got a job). On the other hand, no :) I'm playing around with several things: found a Lua debugger (<a href="http://www.xtgsystems.com/linux/lua/titmouse.php" target="_blank">Titmouse</a>) with source that shows me how to do the Lua debugger in the Underworld Adventures debugger. Although uwadv has grown too big with time to start it to quickly verify some things. A solution would be a mini-uwadv that just calls the uadebug and implements some callback commands needed to get it running.
</p>

<p class="devlogBody">
Another thing I'm looking forward to do is to improve critter animation frames loading. Currently it takes almost forever compared to the last release. The main reason for that is that a ua_image object is constructed and copied twice for every frame loaded, which could be improved. And finally Servus did a door texture, which could be plugged into the texture manager easily by having another config file in the game's uadata folder.
</p>

<span class="devlogHeading">2003-05-14</span>
<p class="devlogBody">
Rewrote the quadtree tile culling stuff using the ua_vetor2d class and throwing out all unneeded stuff. Added another render() function to ua_renderer that receives all player dependent variables; the ua_renderer class could also be used for map viewer now. Changes were made to support the uadebug tilemap view.
</p>

<span class="devlogHeading">2003-05-13</span>
<p class="devlogBody">
Invented a new system to process messages, especially mouse messages, based on areas, implemented in the template class ua_message_processor. The user of the class can register member function pointers to handler functions, together with the area definition. Processing for keys in a ua_keymap is planned, too. The new system should make the whole process of treating mouse messages much easier.
</p>

<p class="devlogBody">
Reimplemented start menu dialog using the new message processor as a proof of concept. Should improve handling all those mouse events in ingame screen, too.
</p>

<span class="devlogHeading">2003-05-11</span>
<p class="devlogBody">
radekg2 added all files needed for debian package building. Cool thing!
</p>

<span class="devlogHeading">2003-05-05</span>
<p class="devlogBody">
Added ua_vertex3d struct and changed how ua_triangle3d_textured is implemented. Added a polygon tessellator class based on gluNewTess(), gluDeleteTess() and gluTess*() functions. Changed loading and rendering builtin models to use the polygon tessellator. Some models still look strange, though.
</p>

<span class="devlogHeading">2003-05-03</span>
<p class="devlogBody">
Added experimental "tilemap view" to underworld debugger. It uses the same rendering functions as the game but a seperate ua_renderer object. Currently resizing the frame window results in lockups. Added mutex implementation for locking the underworld object (lock/unlock was an empty call 'till now), but it doesn't help either. Have to investigate further.
</p>

<span class="devlogHeading">2003-04-30</span>
<p class="devlogBody">
Added Radoslaw Grzanka "radekg2" to the project; he is responsible for debian packaging. Released his .deb package for the 0.8a-hot-chili release. Started discussion with John McCawley about AI implementation.
</p>

<p class="devlogBody">
Added dumping to the builtin model loader, to be used in uwdump. Removed the viewer part of the modeldecode.cpp, which is now implemented in uwmdlview. Added support for uw2 level dumping to uwdump.
</p>

<span class="devlogHeading">2003-04-29</span>
<p class="devlogBody">
Opening container can now be done in any game mode, not just default. Fixed a bug with the priority cursor code, fixing all 3 bugs uwadv currently has. Done a win32 binary release 0.8a-hot-chili.
</p>

<span class="devlogHeading">2003-04-27</span>
<p class="devlogBody">
Fixed bug <a href="http://sourceforge.net/tracker/index.php?func=detail&amp;aid=722818&amp;group_id=50987&amp;atid=461662">#722818</a> submitted by Radoslaw Grzanka (radekg2).
</p>

<span class="devlogHeading">2003-04-24</span>
<p class="devlogBody">
Implemented "a_delete object trap". Added "can be picked up" flag from common object properties, preventing the user from picking up some objects. Bridges rendering are fixed temporarily by just drawing the bridge square. Also fixes collision detection. Have to find a way to subdivide the concave polygons delivered by the builtin models decoder. Omitted rendering of some objects not 16x16 and thus not available in the object images list.
</p>

<p class="devlogBody">
Documents were updated for the release. Added some more hotspot points to the debugger's hotspot list. And a lot more small fixes. As I tested the release with uw_demo, the game crashed because no builtin models could be loaded; added the model positions for the uw_demo.exe build I have. Fixed crash when changing to next level in uw_demo, e.g.  by teleport trap in the gray goblin's area. An exception is now generated. Fixed priority cursor functionality in ingame screen. Fixed textscroll when input line gets too long. Phew, a pretty long list o' stuff.
</p>

<span class="devlogHeading">2003-04-16</span>
<p class="devlogBody">
Added an underworld builtin models viewer and rewrote the models decoder to not subdivide the (often concave) polygons. Disabled collision detection for bridges temporarily. Moved the ua_model3d implementation classes to own header file. Fixed the luac Lua compiler; it had some stub functions to not pull in all liblua functions.
</p>

<p class="devlogBody">
Fixed the #undef main part of most tools; the reason for most undefined _main macros was the fact that the main function has to have the exact signature "int main(int argc, char* argv[])".
</p>

<span class="devlogHeading">2003-04-15</span>
<p class="devlogBody">
Added Lua functions to access the inventory. Added getting objects from the underworld, without checks for distance and get-able objects for now. Added inventory/container weight calculation. Some containers have space limits.
</p>

<span class="devlogHeading">2003-04-13</span>
<p class="devlogBody">
Fixed object list view in uadebug and added a hotspot list feature to immediately and conveniently jump to important spots in the underworld. Updated doxygen config file for the new 1.3 release.
</p>

<span class="devlogHeading">2003-04-10</span>
<p class="devlogBody">
Added map notes class to store map annotations. Added texture animation for lava and water textures, which is now a simple thing due to the new texture manager implementation. Extended the flags to 16 bit, since it may have to hold bridge texture id's. Added support for trigger-trap-trigger-trap-like effect chains.
</p>

<p class="devlogBody">
Modified the Lua library in such a way that it can parse hex digits, like 0x016e, which greatly simplifies work with item_id's throughout the code. Maybe I should send the <a href="http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/uwadv/uwadv/source/lua/lua-hexdigit-patch.diff?rev=HEAD&amp;content-type=text/vnd.viewcvs-markup">hexdigit patch</a> to the lua-users list or so. Modified all Lua scripts to use the new format when possible and adequate. Moved the Lua and zziplib library files to the uaopt project to get some space in msvc and optimized code.
</p>

<span class="devlogHeading">2003-04-09</span>
<p class="devlogBody">
Moved "look at" and dump related functions to own .lua file. Moved looking at walls to Lua function. Rewrote texture manager to better handle animated textures and further stock textures for switches, levers, doors, bridges, etc. Additionally it isn't required anymore to call ua_texture::use() before upload()ing textures to OpenGL. All stock textures used in rendering must be prepare() when resetting wall/floor textures. For switches/levers/pull chains and wall writings the correct texture is now used. Added plaque reading.
</p>

<span class="devlogHeading">2003-04-08</span>
<p class="devlogBody">
Fixed "look at" description for all objects that can have a "belongs to ..." description printed; it is determined via the common object properties table in Lua.
</p>

<span class="devlogHeading">2003-04-06</span>
<p class="devlogBody">
Rewrote map import, as SDL_RWFromFP() doesn't work in debug builds, since it tries to use the FILE* handle from the release runtime library (MSVCRT), but it was opened in the debug one (MSVCRTD). The rewrite is cleaner now anyway.</p>

<p class="devlogBody">
As textures have to be "absolute" in the uwadv engine (don't have the mapping available ingame), I should add texture resolving for all items that need it; "special tmap object" is already done. Mostly implemented "special tmap object" rendering, too.
</p>

<span class="devlogHeading">2003-04-04</span>
<p class="devlogBody">
My "Mysteries of the Abyss" clue book for Ultima Underworld 1 arrived from US today! It has a fairly comprehensive description of all important spots of all the places in the underworld, as well as more technical infos about the gameplay. It really helps developing uwadv further. (thanks Beavis!)
</p>

<span class="devlogHeading">2003-04-01</span>
<a id="april1st" name="april1st"></a>
<p class="devlogBody">
Did an april fools joke today. I announced this text on the main news page:
</p>

<p class="devlogBody">
<i>
2003-04-01 - Fantastic news!<br/>
I'm very proud to announce some fantastic news today! The Underworld Adventures Team got the original sourcecode to Ultima Underworld 1 and 2 today! Doug Church himself, the lead developer for both games, sent a zipped file with the game's source code. And the best is: we, the team, offer you the download, too (see below).
</i>
</p>

<p class="devlogBody">
<i>
With these fantastic new happenings, I estimate that the Underworld Adventures project can be finished in about one or two months, filling in all the missing parts from the original games. Please visit this page in the next few days for another announcement of detailed plans how to accomplish this. Meanwhile, get the original Ultima Underworld source code <u>here!</u>.
</i>
</p>

<p class="devlogBody">
The link pointed to a page with this text:<br/>
<i>
Happy April Fools Day to everyone!<br/>
Sorry for that :-) but expect a release with some (not all, heh) new stuff in about one or two weeks. Really!</i>
</p>

<p class="devlogBody">
Added "toolsinstall" target for Dominus to install the tools.
</p>

<span class="devlogHeading">2003-03-31</span>
<p class="devlogBody">
Added uw2 .ark files decoding using a SDL_RWops struct. Level tilemap and object list importing already works. Disabled some things for uw2 games to get it up and running, but I don't intend to work on this for now. Added "used" flags; used in "empty" map slots in uw2. mapdisp and map3ds do support uw2 levels from here on. Added loading of a language specific strings file, from %prefix%/lang.pak, to support text string translations in uwadv. Added Lua functions to access the common object properties.
</p>

<span class="devlogHeading">2003-03-27</span>
<p class="devlogBody">
Jammet contacted me today and asked about the screenshots with the translated conversation texts and offered help in translating more of the game. Supporting custom-loaded game strings .pak files should be fairly easy.
</p>

<span class="devlogHeading">2003-03-26</span>
<p class="devlogBody">
Added common object properties decoding to uwadv and uwdump.
</p>

<span class="devlogHeading">2003-03-25</span>
<p class="devlogBody">
Implemented the move trigger and the teleport and text string traps; level changing works now. Strings are printed a bit too often (each time the trap is set off :) The is_quantity flag is always set for triggers (seems to be a bug/not necessary); uwadv's object list loader fixes this. Added "using" of "world" objects to get use triggers to work. Added look trigger handling, too.
</p>

<p class="devlogBody">
Disabled rendering of invisible flagged objects in release mode. For all objects the zpos field is used; bats, etc. and other objects are in proper height now. Added rendering of switch/button/pull chain/etc. decals. Quest flags are now saved in savegames. Fixed selection/picking for decals.
</p>

<span class="devlogHeading">2003-03-22</span>
<p class="devlogBody">
Sir Cabirus also sent a bunch of saved games from the original game that start at each level of the underworld. Good to have it around, in case I have to verify game-related stuff.
</p>

<span class="devlogHeading">2003-03-20</span>
<p class="devlogBody">
wjp fixed compiling of the uwdump tool for Linux systems.
</p>

<span class="devlogHeading">2003-03-19</span>
<p class="devlogBody">
Finally I can go online (with a dialup, at least) again. Updated uwadv to have the object coordinate fields in integer again (some traps do need the field's value) and added the newly found npc fields to the object class and the Lua table (objlist_get_info). Found a new remake project, <a href="http://nuvie.sourceforge.net/" target="_blank">Nuvie</a>, a remake similar to Exult, but to play U6 engine-like games (U6, Martian Drams (yeah!) and Savage Empire). After hacking the mingw32 makefile away, it even compiled and linked. Have to try out later when I have a martian savegame somewhere.
</p>

<span class="devlogHeading">2003-03-18</span>
<p class="devlogBody">
<a href="http://www.sircabirus.com/" target="_blank">Sir Cabirus</a> made a special offline version of his walkthrough and sent it to me, to always have it available on hard dist. Nice thing!
</p>

<span class="devlogHeading">2003-03-17</span>
<p class="devlogBody">
Finally figured out the place where npc variables are stored in the conversation memory for use as "imported globals". Could guess quite some npc_* variables in the npc data area. I recently modified uwadv that the "zpos" field is stored as a double - seems that I have to change this field back to int, since some traps uses the field. The same with "xpos" and "ypos" fields. And it seems that tilemaps store the floor height in a lower bit resolution than used in the game, only using 5 bits. They are shifted left to use 7 bits ingame, just as the normal item's "zpos" field also has.
</p>

<span class="devlogHeading">2003-03-15</span>
<p class="devlogBody">
Found out more stuff using uwdump and the decoded uw.exe:<br/>
* the speaking door in level 6 (tile coords=04/02) is hard-wired in the uw.exe; a temporary npc object is allocated and the npc_whoami field is filled with the conv. slot of the door<br/>
* a common function for link field extraction for tilemap index, normal chain links and special links are used, since the fields all have the same bit positions (bits 6..15 of a 16-bit integer value)<br/>
* uw seems to use a simple random number generator (don't know if it's the runtime library's one) with the equation rand = (rand_last * 0x015a4e35)+1
</p>

<span class="devlogHeading">2003-03-12</span>
<p class="devlogBody">
Started to write an "uwdump" program to extract and dump all infos from uw1/2 in text form. It currently features level map and master object list decoding.
</p>

<span class="devlogHeading">2003-03-10</span>
<p class="devlogBody">
started disassembling and commenting the uw.exe source using a freeware version of IDA Pro, 4.1 (a bit older than the current freeware version, but it supports non-PE executables, too). Found out quite some interesting things:
</p>

<p class="devlogBody">
* the game is written mostly in C, even though Turbo C++ 2.1 was used<br/>
* game strings needed during gameplay are loaded/unpacked on-the-fly, the strings.pak is always open<br/>
* a temporary lev.ark and bglobals.dat is stored in Save0 folder during gameplay<br/>
* internal underworld level indices are 1-based; 0 means "current level"<br/>
* pit and combination traps are not implemented<br/>
* ward and tell trap seems to do the same<br/>
* there's a "debug printf" function, but it has no implementation (release build)<br/>
much more things ...
</p>

<span class="devlogHeading">2003-03-03</span>
<p class="devlogBody">
I'll be unavailable online for some days, as I'm relocating to another city. I'll continue doing stuff for uwadv in the meantime.
</p>

<span class="devlogHeading">2003-02-25</span>
<p class="devlogBody">
Modified the master object list in the underworld debugger to hold all values in a string array, in spite of getting every value from the game itself when the list control has to be updated. The user has to "update" the list to be the current one when editing it. Reordered columns. Unfortunately I cannot catch a double-click on a particular list cell (e.g. to jump to a linked object). Added a AddFrameMenus() method to the main frame window to set common menus to child frames. Added menu items for the master object list frame to save object list to a .csv file, and added the possibility to change the level of which the object list is shown.
</p>

<p class="devlogBody">
I changed back the return types of all handle_event() and mouse_action() methods, since it didn't really make sense to have the info if the event was handled. Also added saving of ua_object objects to also store inventory objects easily. In Lua, the real npc variable names (e.g. "npc_xhome", etc.) are set instead of using a "data" array. For this I have to find out most of the important variables in the npc info, though. Disabled the inventory items placed into inventory for release builds. Added inventory saving. Fixed conversations, not that we don't have a "data" array. Fixed Lua dump_objinfo_table() function. Wow, done a lot this day!
</p>


<span class="devlogHeading">2003-02-24</span>
<p class="devlogBody">
Added experimental "special tmap object" decal rendering using glPolygonOffset(), but selection doesn't work quite right with it (as mentioned in the manual). should probably render the decal differently when using selection mode. Object zpos is now in tilemap z coordinate range for better coordinate handling.
</p>

<span class="devlogHeading">2003-02-21</span>
<p class="devlogBody">
Added "separate mesh" functionality to map3ds, for the Morrowind Underworld project.
</p>

<span class="devlogHeading">2003-02-19</span>
<p class="devlogBody">
Found out what skills.dat contain; it lists the skills set up during character creation. Still not sure about chrgen.dat
</p>

<span class="devlogHeading">2003-02-18</span>
<p class="devlogBody">
Modified the screens base class to have a base class for controls, too. The base class does the mouse action event stuff now, as well as the mouse-on-area functionality. Improved trace output of most screens, too. OpenGL window is cleared when doing screen init, suspend, resume or done. Modified replacing screens (resume and suspend of the last screen was called, which is unneeded an time-consuming).
</p>

<p class="devlogBody">Factored out the panel on the right of the user interface - the inventory/stats/runebag control. Added hack to get bridges to work, rendering and collision detection wise. Had to use an ugly hack to access the model manager from the physics code (static pointer to mgr). Also started to rewrite the uw-formats.txt file in a more organized manner.
</p>

<span class="devlogHeading">2003-02-16</span>
<p class="devlogBody">
Paths are not required anymore to have slashes at the end (proposed by Copernica). yot reported that the last release freezes at him when clicking on "Introduction". Couldn't reproduce the bug here.
</p>

<span class="devlogHeading">2003-02-15</span>
<p class="devlogBody">
The UW1 Morrowind mod people (they use some tools from uwadv for resource extraction) announced their <a href="http://users.tpg.com.au/ron3sun/main.htm" target="_blank">homepage</a> to me today. Looks pretty!
</p>

<span class="devlogHeading">2003-02-14</span>
<p class="devlogBody">
Finished savegame screen today, although the user cannot scroll when more than 17 savegames are displayed (hmm, maybe the up/down buttons from the inventory could be reused). I've put up a screenshot (also one with the debugger shown). Also introduced usage of a "game.cfg" file that contains all per-game configuration settings, etc. Currently we only have a "uw1" game, but adding "uw2" or custom games will be much easier later.
</p>

<p class="devlogBody">
The debugger interface and underworld access API classes now get tick() calls every tick to process debug stuff on the main game loop thread. This was necessary to reload all textures when the map level is changed from the debugger. OpenGL can only work with the resource context when it's used from the same thread that it created. Other uses would be a "notify" system that "sends" messages back to the debugger, e.g. when a breakpoint is released. The debugger then could "query" a stack or deque of "events" from the main game thread.
</p>

<p class="devlogBody">Ah, and added pausing the underworld. Oh, and fixed the lua_savegame_* functionality. And disabled the "level up" and "level down" keys for release builds. I hope level changing by triggers/traps is possible 'till the next release.
</p>

<p class="devlogBody">
Darkwarrior from <a href="http://www.stygianabyss.com/" target="_blank">StygianAbyss</a> sent word that the TTLG network currently is down due to hardware issues. Hope everything is up soon again. Here's a <a href="http://www.wingedspirit.net/ttlgstatus/" target="_blank">status page</a>.
</p>

<span class="devlogHeading">2003-02-13</span>
<p class="devlogBody">
Finally Gamespydaily and RPGPlanet figured out that uwadv has a new release. Unfortunately they write that uwadv would be an "unauthorized" remake of ultima underworld 1. I don't know who should authorize me to write a game and engine that happens to look the same as uu1 and imports all game resources by chance, as I nowhere use the Ultima trademark (which Origin or EA owns) other to describe the project's meaning.
</p>

<p class="devlogBody">
The people at <a href="http://www.rpgcodex.com/" target="_blank">RPGCodex</a> also discovered uwadv. They even added the screenshot with the "abyss cubus" :) And yes, people seem to still have Voodoo3's :) Another <a href="http://www.jokerbone.com/archives/2002/11/08/000766.html#000309" target="_blank">site</a> (a blog writer) even titled his article "Mages get the chicks..." :)
</p>

<span class="devlogHeading">2003-02-12</span>
<p class="devlogBody">
Continued to write and integrate the savegame screen into uwadv. Also wrote code to do a screenshot from the game, to use either as screenshot to save to disk or in the savegame preview, to show off where the player was when he/she saved.
</p>

<p class="devlogBody">Also played with mapdisp a bit, adding an option to import all levelmaps at once, showing the "borg cubus" from the image some days below this. Also added a screenshot option to take hi-resolution screenshots. The idea cam from a <a href="http://www.delphi3d.net/listfiles.php?category=6" target="_blank">Delphi3d</a> <a href="http://www.delphi3d.net/download/hiresshot.zip" target="_blank">tutorial</a> about hires render screenshots. The projection matrix isn't that correct from what the user sees in the image, but it produces nice results. One could even print out the 20MB of 4x4 tiled (2560x1920) underworld levels as poster or something :)
</p>

<span class="devlogHeading">2003-02-11</span>
<p class="devlogBody">
Released the "Muffin" today. Only posted news to freshmeat.net for now. Also started working on the unified savegame load/save screen, using most UI elements from the character creation screen.
</p>

<span class="devlogHeading">2003-02-10</span>
<p class="devlogBody">
Fixed "make dist" today and added a configure option --enable-uadebug (which currently doesn't do anything) to later enable building the Underworld Debugger.
</p>

<span class="devlogHeading">2003-02-04</span>
<p class="devlogBody">
Fixed texture paths inside the .wrl files, since file names are case-sensitive when opening from the resource file (and on Linux).
</p>

<span class="devlogHeading">2003-02-03</span>
<p class="devlogBody">
Had the chance to try uwadv on a Diamond FireGL 1000 Pro card (seems to be manufactured 1998 or so). It's an AGP card and has 8MB memory. In windowed modes textures are very blurry and tiled textures have very obvious seams. According to the OpenGL stats it only supports 128x128 sized textures in this mode (but 16 light sources - my current card even can't do that). In Fullscreen mode things are slightly better - textures up to 512x512. Frame rate is still bad. 5 fps at the animated underworld logo and about 6 fps in the first hallway. Well, it was a good card for its time.
</p>

<span class="devlogHeading">2003-02-02</span>
<p class="devlogBody">
Added a menu item to the underworld debugger to disable underworld processing (since most edit actions only make sense when the game is halted).
</p>

<span class="devlogHeading">2003-01-30</span>
<p class="devlogBody">
Added a child frame to show the "master object list" in the debugger. Should be easy to do in wx.
</p>

<span class="devlogHeading">2003-01-29</span>
<p class="devlogBody">
Replaced the way the debugger accessed infos of the underworld object with a new one. The old one was done using a class, and that wasn't that portable between msvc and mingw. The new system uses a single callback "command" function that the debugger can call to get infos. It can be given a command, a command type and two in/out-parameters using a union inside a struct (should be portable enough). Tried to get the wxWindows stuff to compile on mingw32, but there still are some problems. Added a "Underworld Adventures features" check to the uaconfig program.
</p>

<span class="devlogHeading">2003-01-28</span>
<p class="devlogBody">
Added support for translation, rotation and scale keywords from vrml. Don't know if I handle them properly. Enough with 3d models for now.
</p>

<span class="devlogHeading">2003-01-20</span>
<p class="devlogBody">
Introduced a static library for the tools where all needed files from uwadv get in. I already had such a thing for msvc, but added it for mingw32 (and Linux), too. Added the 3ds import code used for testing, but it's not used in uwadv (it needs lib3ds). Added some more objects from Servus to the CVS, as well as a .wrl file viewer. Added decoding to tga images into animview (used in another remake project). And finally fixed a tiny resource leak in the code loader (file was not closed).
</p>

<span class="devlogHeading">2003-01-17</span>
<p class="devlogBody">
More work on the model importing. Added a tga file importer from one of my previous projects.
</p>

<span class="devlogHeading">2003-01-16</span>
<p class="devlogBody">
Wrote a rather limited <a href="http://astro.temple.edu/~john43/flex/" target="_blank">flex</a> lexer to read in .wrl (vrml97) files and added derived ua_model3d class. Added some of Servus' models to CVS to incorporate into uwadv. Also moved the debug interface pointer to the game core class, to have it on a more centralized place. Should come up with a new debugger access api and uadebug application soon.
</p>

<span class="devlogHeading">2003-01-13</span>
<p class="devlogBody">
More fixes for generic conversations. More npc attributes are stored in the data array, too. Npc attitude is now output when looking at one.
</p>

<span class="devlogHeading">2003-01-08</span>
<p class="devlogBody">
Fixed generic portraits in conversations (there is on portrait for every item_id). The conversation screen now gets level and object list pos of npc to talk to (needed for all sorts of things, e.g. inventory of npc, remove_talker, etc.). Fixed wall/ceiling/floor textures after conversation (texture manager reloaded wrong set of textures). Thought about how to import Blender3d files and found blend2cs (blender to crystal space converter) on <a href="http://projects.blender.org/" target="_blank">http://projects.blender.org/</a>. Servus joined #uwadv and later modeled some models for uwadv and low. <a href="http://www.geocities.com/russell_figowitz/images/" target="_blank">Here</a> are some of his images.
</p>

<span class="devlogHeading">2003-01-07</span>
<p class="devlogBody">
Some changes on the uninstaller (stdout.txt was not deleted). Added decoding of weapons.dat in th hacking folder, but don't know yet what it all means.
</p>

<span class="devlogHeading">2003-01-04</span>
<p class="devlogBody">
Telemachos changed gravity a bit (first CVS commit in the new year!).
</p>

<span class="devlogHeading">2003-01-03</span>
<p class="devlogBody">
Changed uwadv-devel list configuration so that to post one have to be subscribed. Should prevent spam mails on the list.
</p>

<span class="devlogHeading">2003-01-01</span>
<p class="devlogBody">
Dark-Star, a regular visitor of #uwadv started <a href="http://sourceforge.net/projects/exfinity/" target="_blank">Exfinity</a>, a remake of Baldur's Gate's (and other games') Infinity engine. He also has a channel on freenode, #exfinity.
</p>

<span class="devlogHeading">2002-12-25</span>
<p class="devlogBody">
wjp found out there is a bug in automake 1.6.1 that installs the ua-lowercase-all.pl script to the wrong place. Using 1.6.3 should fix that.
</p>

<span class="devlogHeading">2002-11-30</span>
<p class="devlogBody">
Fixed the map3ds conversion program today, and also did a nice "all-levels-in-one" 3ds which looks quite cool. Here's the <a href="http://uwadv.sourceforge.net/index.php?page=image&amp;image=devel/all_levels.jpg">image</a>. Wrote a test program to show the uw models extracted from the exe, but the polygon subdivision is wrong for now. Moved some non-sourcecode files from the "linux" and "win32" to separate folders in the main project dir. Also found out that the last texture in the ceiling texture mapping in lev.ark actually is the ceiling texture.
</p>

<span class="devlogHeading">2002-11-29</span>
<p class="devlogBody">
Fixed the conversation screen bugs. Added more imported function implementations. Sethar strongarm should work now (he can be asked for some keywords). Conv. text replacements like @SS1 or @GS8 should work, too. Added music playback for conversations, so that QQtis can work on Maps &amp; Legends (it's also used here).
</p>

<span class="devlogHeading">2002-11-27</span>
<p class="devlogBody">
Found out more flags and values in the NPC extra data, thanks to Coren. Added quest flags to the underworld object (basic quests like Ketchaval / Retichall) work now. Fixed BRA opcode (target address off-by-one error).
</p>

<span class="devlogHeading">2002-11-26</span>
<p class="devlogBody">
Started working on conversation screen. Conv. VM needed some fixes and most imported functions are missing, but basic conversations via the main "babl_menu" function works. Used the ua_image_quad class in ingame screen, too. Underworld Adventures is now mentioned in the rec.games.computer.ultima.* FAQ.
</p>

<span class="devlogHeading">2002-11-25</span>
<p class="devlogBody">
Added ua_image_quad class to encapsulate rendering of big images using only textures up to 256x256 (functionality was moved from the ua_textscroll class). Used the class for 2d images in text scroll, splash screen, as base class for ua_cutscene, acknowledgement screen and the start menu. uwadv should be Voodoo3-safe now. Also tried to unpack uw2 conversations (they used a simple compression scheme this time).
</p>

<span class="devlogHeading">2002-11-23</span>
<p class="devlogBody">
There weren't as much hits from news pages as expected, but oh well. Decided to use shorter release periods, while working on single features at a time.
</p>

<span class="devlogHeading">2002-11-21</span>
<p class="devlogBody">
Today's release day! We chose "rotworm stew" (item_id=0x011b) as code name, to have a somewhat more ultima related release name this time (Michael Maggio once complained on his news site <a href="http://reconstruction.voyd.net/" target="_blank">Ultima: The Reconstruction</a> about that :-). uwadv is also announced at freshmeat.net, which I expect to come the most site hits from.
</p>

<span class="devlogHeading">2002-11-20</span>
<p class="devlogBody">
Added looking at objects in 3d world and fixed some last bugs. Fixed rendering of 2d critter sprites. Added some code to read in the models from the executable (not used for release). Fixed doxygen documentation generation and updated the uw-formats.txt with 3d models info. FMOD support was ripped away.
</p>

<span class="devlogHeading">2002-11-18</span>
<p class="devlogBody">
Ripped away the old 2d collision detection code. wjp fixed last things on the Linux build system.
</p>

<span class="devlogHeading">2002-11-15</span>
<p class="devlogBody">
Fixed conversation graph stuff. Added Lua callback function to retrieve all object properties. Added first try on looking objects in the underworld.
</p>

<span class="devlogHeading">2002-11-13</span>
<p class="devlogBody">
Fixed the tools today, made some changes related to the player height and added player repositioning when changing the level with Alt+PageUp or Alt+PageDown, so that users can see more of the underworld in the next release. Fixed some minor problems and glitches.
</p>

<span class="devlogHeading">2002-11-12</span>
<p class="devlogBody">
Merged most of Fabian Pache's additions to the uw-formats.txt file related to uw2 format differences. Fixed uw_demo usage in uwadv and changed the configure script to (hopefully) work under MacOS X (after Fingolfin's remarks).
</p>

<span class="devlogHeading">2002-11-10</span>
<p class="devlogBody">
QQtis redid "Descent" (which sounds much better now) and started with "Wanderer". Always available at <a href="http://www.wam.umd.edu/~mshetty/music/" target="_blank">QQtis music folder</a>.
</p>

<span class="devlogHeading">2002-11-08</span>
<p class="devlogBody">
Telemachos improved the game physics again and tweaked gravity to behave more realistic.
</p>

<span class="devlogHeading">2002-11-07</span>
<p class="devlogBody">
Found a neat and easy way to fix those thin black lines at OpenGL texture borders by using GL_CLAMP_TO_EDGE as wrapping parameter. Also wrote man pages for uwadv and ua--lowercase-all.pl and packaged them into the rpm.
</p>

<span class="devlogHeading">2002-11-05</span>
<p class="devlogBody">
Incorporated the conversation graph code into cnvdasm and removed the old one (was much too crappy :) Also redid the disassembler, which was an easy task.
</p>

<span class="devlogHeading">2002-11-04</span>
<p class="devlogBody">
Started to rewrite decompiling the uw1 conv. code after glancing through a book about compiler building ("Übersetzerbau"). Decompiled code also helps to understand meaning of most imported functions. The code is seen as a directed graph of opcodes that can be folded to C-like functions, statements and expressions. In the far future decompiled code text could be used in a visual conv. code debugger.
</p>

<span class="devlogHeading">2002-10-30</span>
<p class="devlogBody">
Found a bug (in the OpenGL driver or somewhere) that prevents using mipmapped textures together with alpha blending. Used alpha testing (via glAlphaFunc()) to ignore rendering order. "Descent" is now played when entering the Abyss.
</p>

<span class="devlogHeading">2002-10-28</span>
<p class="devlogBody">
Added critter rendering, although only the first frame is rendered. Disabled the savegame related features for next release. Also fixed some stuff.
</p>

<span class="devlogHeading">2002-10-27</span>
<p class="devlogBody">
Telemachos fixed collision code and also introduced gravity (which is way too slow yet), but nice work! QQtis digitally rendered two more music pieces, "Descent" and "Maps &amp; Legends", available <a href="http://www.wam.umd.edu/~mshetty/music/" target="_blank">here</a>.
</p>

<span class="devlogHeading">2002-10-26</span>
<p class="devlogBody">
Fixed a few bugs and glitches, added fading out for music and discovered that the Linux SDL_mixer midi driver doesn't play anything since the code was missing to write a temporary midi file (very ugly!). Also moved the gamestrings object to the underworld (needed by lua script bindings), and custom keymaps are loaded from the %uahome% folder now (when available). QQtis was added to the "about us" page and is member of the team now.
</p>

<span class="devlogHeading">2002-10-25</span>
<p class="devlogBody">
Corey Day started to do some 3d modeling for the uwadv project.
</p>

<span class="devlogHeading">2002-10-21</span>
<p class="devlogBody">
Jim visited the irc channel and we discussed model loading a bit, which helped my understanding of the models greatly. We'll see what is included in the next release, though. For the msvc project files I moved some source files (critloader.cpp and gamestrings.cpp) to a separate static linked library that has optimizations switched on, which gives some speed improvements during debugging.
</p>

<span class="devlogHeading">2002-10-20</span>
<p class="devlogBody">
Found out that using a std::list is much more performant when doing a push_back() than using a std::vector, but loading times only went down to 20 seconds in debug mode. Unfortunately, MSVC cannot be configured to partially use optimizations in debug builds.
</p>

<p class="devlogBody">
Also found a new interesting site, <a href="http://www.salikon.dk/quillframe.html" target="_blank">Quill Dragon's</a>. Above other things, he has some manuals, and you can play Nim with Dr. Cat (and even beat him!). And there were some good laughs on the <a href="http://home.hiwaay.net/~rgregg/ultima/literal/" target="_blank">U7 book transcripts</a> from Underworld Dragon. And found an implementation of OpenGL that could world on handhelds: <a href="http://fabrice.bellard.free.fr/TinyGL/" target="_blank">TinyGL</a>. Whew, what a load-o-stuff :)
</p>

<span class="devlogHeading">2002-10-18</span>
<p class="devlogBody">
Added a %uahome% variable to have a common placeholder for all systems. Added critter loading. Strange thing is that it needs 48 seconds in debug mode, and only 4 seconds in release mode!
</p>

<span class="devlogHeading">2002-10-17</span>
<p class="devlogBody">
Added resampling code for the .voc files to resample from 12048 Hz to 22050 Hz (actually, 22088 Hz). The reason for this was that the SDL_mixer base sample rate was at 12000 Hz, which caused ogg vorbis files to play back too slow. Selecting 22050 Hz as sample rate (a recommended default) plays the .voc files too fast (internal resample routines of SDL_mixer only worked when resampling with integer factors, not with fractional values).
</p>

<p class="devlogBody">
I chose a resampling factor of 6 : 11 that gave almost the exact target bitrate (off by 38 Hz) and kept the resampling filter coefficients short. The algorithm used was taken from <a href="http://www.dspguru.com/sw/opendsp/alglib.htm" target="_blank">dspguru.com's algorithm library</a>. The lowpass filter was designed with FIRfilt, a tool from the uni's dsp lab. At last it pays off to have digital signal processing as a subject :)
</p>

<span class="devlogHeading">2002-10-16</span>
<p class="devlogBody">
Invented a underworld access API for access of uadebug to the ua_underworld object and its members. Player Info in the debugger is filled now (unfortunately, the uadebug.dll can't be used with mingw32). Also added usage of precompiled headers to uadebug, which speeds up compilation quite a bit. Mailed the author of the <a href="http://www.anthemion.co.uk/wxworkshop/docs/html/fl/fl.htm" target="_blank">fl library</a> (it's part of the new wxWindows beta release) used about a possible bug.
</p>

<span class="devlogHeading">2002-10-13</span>
<p class="devlogBody">
Jim found out more about the tilemap format, especially about enchantments and "free lists" for the master object list.
</p>

<span class="devlogHeading">2002-10-11</span>
<p class="devlogBody">
Rewrote collision detection to use real 3d, as in the <a href="http://www.peroxide.dk/download/tutorials/tut10/pxdtut10.html" target="_blank">peroxide tutorial</a> but it doesn't work yet, so I disabled it again. Also fixed item combining.
</p>

<span class="devlogHeading">2002-10-10</span>
<p class="devlogBody">
Restricted mouse movement in "mouse movement mode" to the 3d view window. Also fixed a bug with the mode.
</p>

<span class="devlogHeading">2002-10-07</span>
<p class="devlogBody">
Martin Shetty started to rearrange uw1's midi tracks using software synthesizer and converted it to mp3. A bug in uwadv prevents to play back external music yet.
</p>

<span class="devlogHeading">2002-10-06</span>
<p class="devlogBody">
Added sliding left/right and added mouse movement (and better than in the original :-). Also hacked a bit more on the critters decoding.
</p>

<span class="devlogHeading">2002-10-05</span>
<p class="devlogBody">
Added makeshift savegame saving screen and got loading and saving to work. Used zlib deflate compression for the savegames, which decreases file size from about 870kb to 66kb.
</p>

<span class="devlogHeading">2002-10-02</span>
<p class="devlogBody">
Moved some movement code to the physics class, preparing for a collision detection remake. Added keymap loading from file. Also fixed all tools apart from the mapdisp.
</p>

<span class="devlogHeading">2002-10-01</span>
<p class="devlogBody">
Finally got the ua_underworld independent of the user interface classes. Also finished the makeshift savegame loading screen. Also worked on the key mapping a bit.
</p>

<span class="devlogHeading">2002-09-30</span>
<p class="devlogBody">
Telemachos found some <a href="http://sourceforge.net/mailarchive/forum.php?thread_id=1129269&amp;forum_id=10460" target="_blank">old mails</a> from Doug Church (chief? developer of uw1) that explain some stuff with uw1's renderer and 3d model system. He also found out his current email address, as well as <a href="http://www.dfan.org/" target="_blank">Dan Schmidt's home page</a> (along with a nice little <a href="http://www.dfan.org/writing/comment.html" target="_blank">uw1 story</a>).
</p>

<span class="devlogHeading">2002-09-28</span>
<p class="devlogBody">
dforger introduced his <a href="http://sourceforge.net/projects/uw2rev/" target="_blank">uw2 remake project</a> to me today. Dirk pointed me to another <a href="http://screwage.com/~marc/" target="_blank">uw2 project</a> that also looks promising.
</p>

<span class="devlogHeading">2002-09-27</span>
<p class="devlogBody">
fixed the small lines problem, at least for the message scroll class.
</p>

<span class="devlogHeading">2002-09-26</span>
<p class="devlogBody">
Jim found out most stuff about enchantments on weapons and other items, which is a great find!
</p>

<span class="devlogHeading">2002-09-24</span>
<p class="devlogBody">
wjp fixed an inventory bug that I seemed to have created when moving some lua scripts around. texture wrapping parameters are stored in the texture class now. Oh, and we are mentioned <a href="http://www.martinmanning.com/interpreters.htm#underworldadventures" target="_blank">here</a>; they claim we even support uw2 in the future :-))
</p>

<span class="devlogHeading">2002-09-22</span>
<p class="devlogBody">
found a way how uwadv could be finished faster: <a href="http://www.kuro5hin.org/story/2002/4/15/103358/720" target="_blank">Uberman's sleep schedule</a> :)
</p>

<span class="devlogHeading">2002-09-20</span>
<p class="devlogBody">
implemented debugger interface class and added uadebug project to build the dll (or shared library) that contains the debugger. for now it only opens a MDI frame window. The whole stuff uses the latest wxWindows libraries.
</p>

<span class="devlogHeading">2002-09-16</span>
<p class="devlogBody">
Trying around with <a href="http://www.wxwindows.org/" target="_blank">wxWindows</a> a bit, a platform independent UI library. Seems to be a nice thing for the Underworld Debugger I'm planning to do.
</p>

<span class="devlogHeading">2002-09-15</span>
<p class="devlogBody">
The uw2 I ebayed earlier this month arrived, so uw2 support should be in soon ... eerm, joke! :-)
</p>

<span class="devlogHeading">2002-09-11</span>
<p class="devlogBody">
Posted the "3d modeler" job offer today, and also on <a href="http://www.planetquake.com/polycount/">Polycount</a>. Several people applied but they didn't reply to my introduction ... yet.
</p>

<span class="devlogHeading">2002-09-10</span>
<p class="devlogBody">
Worked on the Lua script bindings, and all needed functions should be there now, as well as a script callback interface class.
</p>

<span class="devlogHeading">2002-09-08</span>
<p class="devlogBody">
Added a text scroll class to handle all text scroll things needed in ingame and conversation screens.
</p>

<span class="devlogHeading">2002-09-07</span>
<p class="devlogBody">
Reworked the ingame_orig screen to use separate textures for different UI elements. Texture sizes for the ingame_orig, the start_splash and the ack'ments screens are max. 256x256, to get older voodoo cards to work.
</p>

<span class="devlogHeading">2002-09-06</span>
<p class="devlogBody">
Moved all underworld OpenGL rendering code to own class, to keep it separated from underworld data. Object and wall selection works, too. Also used Telemachos advice to adjust texture wrap mode to GL_CLAMP for some objects. Willem added a configure script option to exclude building the tools (they are broken since the renderer changes).
</p>

<span class="devlogHeading">2002-09-05</span>
<p class="devlogBody">
Modified gamestrings loader to use SDL_RWops to load game strings from uadata resources, too (for the future). More minor modifications.
</p>

<span class="devlogHeading">2002-09-03</span>
<p class="devlogBody">
Discussions on the mailing list about the savegame implementation. I've implemented a savegames manager class after that. Also added a levelmap to 3ds converter tool that uses <a href="http://lib3ds.sourceforge.net/" target="_blank">lib3ds</a>. Also had contact with Peter Jay Salzman from <a href="http://www.tldp.org/HOWTO/Linux-Gamers-HOWTO/index.html" target="_blank">The Linux Gamers' HOWTO</a> and we now have an <a href="http://www.tldp.org/HOWTO/Linux-Gamers-HOWTO/interpreters.html#AEN793" target="_blank">entry</a> in the "Interpreter" section. And found a good article: <a href="http://www.tiscali.co.uk/games/features/2002/04/16/gamesthatchangedtheworldultimaunderworld.html" target="_blank">Games that changed the world: Ultima Underworld</a>.
</p>

<span class="devlogHeading">2002-08-27</span>
<p class="devlogBody">
I've added a rather long <a href="http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/uwadv/uwadv/docs/ua-todo-list.txt?rev=HEAD&amp;content-type=text/vnd.viewcvs-markup">todo-list</a> with various tasks to the CVS.
</p>

<span class="devlogHeading">2002-08-25</span>
<p class="devlogBody">
Dirk set up a channel for general ultima remake discussion, #ultimaremakes, on irc.freenode.net.
</p>

<span class="devlogHeading">2002-08-22</span>
<p class="devlogBody">
released another package, codename <a href="http://www.simpsonslibrary.com/butter.html" target="_blank">butterfinger</a>. For Linux, a RPM package (that needs the latest SDL_mixer-1.2.4 libraries) are available, too.
</p>

<span class="devlogHeading">2002-08-20</span>
<p class="devlogBody">
Dirk finished the character creation screen! Yay! besides that, Linux and mingw32 makefiles were unified, so that only a Makefile.common has to be adjusted when new files are added. Telemachos and I decided to re-implement collision detection using the 3d version from his tutorial (previous one was just a 2d stripped down version). That also would allow for real object physics.
</p>

<span class="devlogHeading">2002-08-18</span>
<p class="devlogBody">
added a screen resolution config setting; allows for other fullscreen resolutions. also updated zziplib to 0.10.65. Telemachos was added to the project to help out with the collision detection.
</p>

<span class="devlogHeading">2002-08-16</span>
<p class="devlogBody">
started writing a settings config program for use with the nullsoft installer and added a "make installer" target to build the installer; the way objects in the object list are represented was changed to simplify the list.
</p>

<span class="devlogHeading">2002-08-15</span>
<p class="devlogBody">
plenty of changes yesterday and today; added fading in/out for ingame screen, added using of m3u playlist for ingame music; added an experimental quicksave/quickload function, and many other internal changes.
</p>

<span class="devlogHeading">2002-08-14</span>
<p class="devlogBody">
Dirk completed the user interface part of the character creation, so I took a screenshot <a href="index.php?page=screenshots">here</a>. He also <a href="http://www.surfing.net/ultima/ucg/uw1_pc.html" target="_blank">did</a> <a href="http://www.netassoc.net/ultima/japanese.htm" target="_blank">some</a> <a href="http://www17.big.or.jp/~kitaji/ultima/jultima/developer.htm" target="_blank">research</a> on the japanese playstation version of Ultima Underworld 1. There also exists a short <a href="http://uwadv.sourceforge.net/devel/psxuw.zip">quicktime video</a>.
</p>

<span class="devlogHeading">2002-08-12</span>
<p class="devlogBody">
a source only package was released today; the Linux build system should be almost complete now. on the same day the exultbot disconnected from #uwadv, while Willem is on vacation until Friday, so no logs are produced.
</p>

<span class="devlogHeading">2002-08-08</span>
<p class="devlogBody">
the project now has an official IRC channel; Willem registered it and set up exultbot to produce channel logs. the config settings part of uwadv was redone, Dirk continues work on the character creation, and Willem works on the Linux build system (rpms are possible now).
</p>

<span class="devlogHeading">2002-08-06</span>
<p class="devlogBody">
added script bindings for Lua and implemented object combining and inventory object classification (for paperdoll item dropping). configuration files are now read dependent on the platform (either plain uwadv.cfg or user-based config files for systems that support home directories.
</p>

<span class="devlogHeading">2002-08-05</span>
<p class="devlogBody">
back from vacation; we decided to convert all dos-style line endings to unix-style ones. tried <a href="http://www.tortoisecvs.org/" target="_blank">tortoisecvs</a> today, which is rather nice (it also handles the cr-lf conversions properly). during the week wjp fixed a bug with the inventory and Dominus found a bluescreen-bug that may result from faulty SB Live drivers together with some midi devices. wjp also hacked apart the Linux makefiles.
</p>

<span class="devlogHeading">2002-07-30</span>
<p class="devlogBody">
release day today! fixed some last inventory related things and the linux makefiles. btw, cheesecake is phlask's favourite food :). also added wjp (of exult project) as linux porter. and we have a new mailing list, uwadv-devel. oh, and I'm on a vacation until sunday.
</p>

<span class="devlogHeading">2002-07-29</span>
<p class="devlogBody">
implemented inventory item interaction. Dirk already has some showable in the character creation.
</p>

<span class="devlogHeading">2002-07-24</span>
<p class="devlogBody">
added some more stuff to the orig. ingame interface, e.g. the "command" buttons on the left. Dirk has started creating the character creation.
</p>

<span class="devlogHeading">2002-07-23</span>
<p class="devlogBody">
changed the texture class interfaces (something that I wanted to do for long). also seemed to found the bug in the collision detection (but I think it created another).
</p>

<span class="devlogHeading">2002-07-19</span>
<p class="devlogBody">
Dirk added settings for subtitle and/or speech to appear in cutscenes.
</p>

<span class="devlogHeading">2002-07-18</span>
<p class="devlogBody">
Dirk completed the introduction cutscene script today, and it's amazing! the speech is almost lip synchronous with the animation.
</p>

<span class="devlogHeading">2002-07-16</span>
<p class="devlogBody">
worked on the inventory class today (which isn't finished yet). also implemented the "acknowledgements" sequence, with a cool blend-over effect.
</p>

<span class="devlogHeading">2002-07-15</span>
<p class="devlogBody">
added zziplib to have easy access to zip files. all uadata files will be stored in resource zip files for binary releases to keep file count small. it already works for loading the cutscene Lua script.
</p>

<span class="devlogHeading">2002-07-14</span>
<p class="devlogBody">
finally found and fixed that linker error when using gcc-3.1 and libstdc++ together (STLport worked) by simply moving class methods that had the offending statement to the front of the cpp file.
</p>

<span class="devlogHeading">2002-07-13</span>
<p class="devlogBody">
extended the cutscene control interface for Lua a bit, so that cutscenes that match the original ones could be implemented via Lua.
</p>

<span class="devlogHeading">2002-07-12</span>
<p class="devlogBody">
added Dirk to the developer's list. for now he's working on the introduction cutscene Lua script.
</p>

<span class="devlogHeading">2002-07-11</span>
<p class="devlogBody">
cutscene control via the Lua script is complete. I already wrote some timetable entries for the intro cutscene, but that is far from complete. took a screenshot from the intro. also started to write developers guide to the gal.. eerm source code.
</p>

<span class="devlogHeading">2002-07-10</span>
<p class="devlogBody">
added the first Lua script, a script to control cutscenes. it should act more as a timetable for animation, speech sound and subtitle text. also discussed the new game interface uwadv additionally gets with Dirk.
</p>

<span class="devlogHeading">2002-07-08</span>
<p class="devlogBody">
hooray! collision detection works! with the help of Telemachos (of <a href="http://www.peroxide.dk/ultima/" target="_blank">U1:R</a> fame), I finally got it to work. Dirk Manders sent in a patch for .voc file playback using <a href="http://www.libsdl.org/projects/SDL_mixer/" target="_blank">SDL_mixer</a> library. that one gets in after the next release of uwadv named ...
</p>

<p class="devlogBody">
<a href="http://www.vosen.com/English/Pastries/Poppy%20Pastry/poppy_pastry.htm" target="_blank">poppy pastries</a>! yes, another silly name, but that one actually tastes very good. a few bugs remain in uwadv, though. happy bug chasing! (there are at least 2 that I know of). after the CVS tagging, I noticed that animview isn't working anymore. I took the binary from the last release.
</p>

<span class="devlogHeading">2002-07-05</span>
<p class="devlogBody">
tinkering with collision detection, but nothing to commit yet. improved underworld object and uw1 game importing while working on the game physics. today I also noticed that Underworld Adventures is part of <a href="http://sourceforge.net/foundry/games/" target="_blank">SourceForge's Games Foundary</a>.
</p>

<span class="devlogHeading">2002-07-03</span>
<p class="devlogBody">
added working compass and player image to the user interface. rendering is even slower, now that the user interface image is uploaded to the graphics card every time the screen is rendered.
</p>

<span class="devlogHeading">2002-07-01</span>
<p class="devlogBody">
enabled scissor testing in the ingame screen, so that the 3d world actually stays in the little window. also added the start menu (although only "journey onward" works at the moment). sorted out the screenshots so that they are a bit more representative. really should tackle that collision detection now.
</p>

<span class="devlogHeading">2002-06-27</span>
<p class="devlogBody">
yay! I never thought that would work. I fixed the last bugs in the game strings tool and translated a bit of conversation (from the uw_demo) with bragit to german. I put a screenshot of the result in the screenshots section. hmm, maybe someone wants to do a uw1 translation project ...
</p>

<span class="devlogHeading">2002-06-26</span>
<p class="devlogBody">
wrote a tool to unpack and repack the game strings. during that, I found a major bug that changed a lot of things how game strings are treated ingame. although, replacing the game strings with repacked ones in the original games doesn't work for conversations, yet. translations for uwadv for other languages will be possible, though.
</p>

<span class="devlogHeading">2002-06-25</span>
<p class="devlogBody">
finished the starting splash screens that shows two still images ("origin presents" and "a blue sky production") and the intro animation with the burning Ultima Underworld logo. had to optimize texture upload to the graphics card a bit. Also put the main game user interface over the screen, to see how it looks. I've put two images into the screenshots.
</p>

<span class="devlogHeading">2002-06-24</span>
<p class="devlogBody">
changed animview so that OpenGL is used to show the animation frames. works nicely, and I don't have to use a <a href="http://elektron.its.tudelft.nl/~dalikifa/" target="_blank">scaler</a>.
</p>

<span class="devlogHeading">2002-06-21</span>
<p class="devlogBody">
delayed release of grainroll one day, to find a rather nasty bug. in the last few days it simply were too hot to do much on the computer (until I found <a href="http://vcool.occludo.net/" target="_blank">VCool</a>). next on my list to do is game physics, e.g. wall collision and such. I already have some ideas how to do this.
</p>

<span class="devlogHeading">2002-06-17</span>
<p class="devlogBody">
added animview, a deluxe paint animation viewer (for the cutscenes in uw1/uw2) to the tools. maybe should do a release in the next few days. also fighted with the linux makefiles again. this time, it even compiles on MacOS X on SourceForge's compile farm.
</p>

<span class="devlogHeading">2002-06-15</span>
<p class="devlogBody">
fixed wall texture drawing, the abyss door (among other textures) is now drawn correctly. also saw an article about the <a href="http://www.ziosoft.com/product.html?n=9" target="_blank">uw1 for pocket pc</a> in the german magazine <a href="http://www.heise.de/ct/" target="_blank">c't</a>.
</p>

<p class="devlogBody">
later in the day: just wrote a cutscenes animation viewer for the "hacking" folder in CVS.
</p>

<span class="devlogHeading">2002-06-14</span>
<p class="devlogBody">
more or less implemented object drawing. mapdisp doesn't show the objects yet. also added some more linux makefile stuff, suggested by Larry Reeder. also fixed a bug where a wall next to a diagonal tile wasn't drawn. as a totally unrelated note, farbrausch's newest 64k-demo <a href="http://www.theproduct.de/" target="_blank">poemtoahorse</a> was a iotd on flipcode. and it's awesome!
</p>

<span class="devlogHeading">2002-06-12</span>
<p class="devlogBody">
I took a few days off from uwadv, to finish <a href="http://winLAME.sourceforge.net/" target="_blank">another project</a>. I also thought about how to integrate the scripting language <a href="http://www.lua.org/" target="_blank">Lua</a> into uwadv, to manage ingame actions and NPC behavior, as well as character creation or cutscenes control.
</p>

<span class="devlogHeading">2002-05-28</span>
<p class="devlogBody">
today I did more object list loading and object rendering. I just rendered a box for every object in the map. I took a nice screenshot in the green goblins area.
</p>

<span class="devlogHeading">2002-05-25</span>
<p class="devlogBody">
hopefully finished Linux (or FreeBSD or MacOS X) configure/make files. for now.
</p>

<span class="devlogHeading">2002-05-23</span>
<p class="devlogBody">
a friend with a FreeBSD box got me shell access to try out compiling uwadv on it. first we had to install SDL-1.2.5 from CVS and do some nasty symlinks to get ./configure to work. it finally worked and we made a screenshot of the mapdisp program. it ran at impressive 2 fps on the 266mhz box. also got it (more or less) on MacOS X on SourceForge's compile farm. had a hard time to run it, though.
</p>

<span class="devlogHeading">2002-05-22</span>
<p class="devlogBody">
released win32 binary compile of the tools, mapdisp, cnvdbg and xmi2mid, codename "whoopie". whoopie!
</p>

<span class="devlogHeading">2002-05-16</span>
<p class="devlogBody">
made a map display program (mapdisp) in an hour, using components of uwadv. it shows the level maps, and they can be moved with the mouse.  looks nice!
</p>

<span class="devlogHeading">2002-05-12</span>
<p class="devlogBody">
uwadv is mentioned on http://www.stygianabyss.com/, thanks Elenkis!
</p>

<span class="devlogHeading">2002-05-09</span>
<p class="devlogBody">
Tels contacted me on how to build uwadv for Linux. from there on improved my knowledge about automake/autoconf, and after more annoyances installed the last versions of the autotools, resulting in a hopefully working set of Linux build tools.
</p>

<span class="devlogHeading">2002-05-08</span>
<p class="devlogBody">
finished my quadtree frustum culling code (tough thing!) and added it to uwadv. speedup from ~70 to 150 fps on my Duron 600MHz / GeForce2 MX.
</p>

<span class="devlogHeading">2002-05-05</span>
<p class="devlogBody">
made my first pre-alpha release. I didn't liked the idea of boring version numbers, and a friend came up with the first release codename, so I took it. "ricecraecker" lets you run through the underworld map (and through walls) and plays some midi in the background. I also remade the project home page.
</p>

<span class="devlogHeading">2002-04-26</span>
<p class="devlogBody">
made my first pre-alpha release. I didn't liked the idea of boring version numbers, and a friend came up with the first release codename, so I took it. "ricecraecker" lets you run through the underworld map (and through walls) and plays some midi in the background. I also remade the project home page.
</p>

<span class="devlogHeading">2002-04-26</span>
<p class="devlogBody">
started a uw1 conversation decompiler, which even seems to work, more or less. the conversation code seems to be compiled from a higher language that is C-like. there are function parameters and return values, and even a switch statement.
</p>

<span class="devlogHeading">2002-04-16</span>
<p class="devlogBody">
Dominus Dragon reports the first bug, a crash bug, which is fixed now. was in the audio part.
</p>

<span class="devlogHeading">2002-04-10</span>
<p class="devlogBody">
the "Underworld Adventures" project is registered at SourceForge. I'm starting to set up things, as well as checking in code already written. a cvs checkins mailing list is also created (might be useful later on).
</p>

<span class="devlogHeading">2002-04-08</span>
<p class="devlogBody">
adapted the Exult xmidi playing code written by Colourless Dragon for my project to play the xmi files. works rather nice. also made a zip file with a bunch of files that shows what uwadv already can do.
</p>

<span class="devlogHeading">2002-03-later</span>
<p class="devlogBody">
started some hacking projects to find out more infos about the files, as well as testing to extract needed data for the game. ongoing discussions with Jim about various aspects of the game's files.
</p>

<span class="devlogHeading">2002-03-11</span>
<p class="devlogBody">
Telemachos of u1 remake fame (he did the Underworld Viewer) answered my mail pointing me to the TSSHP project. Jim and the others already found out much of ultima underworld's file formats.
</p>
