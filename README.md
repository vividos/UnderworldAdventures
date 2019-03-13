# Underworld Adventures

Underworld Adventures is a
project to recreate Ultima Underworld 1 on modern operating systems (e.g.
Windows, Linux or macOS), using the original game files. It uses OpenGL for 3D
graphics, the SDL library for platform specific tasks and Lua for scripting.
It is developed as Open Source and is licensed under the
[GNU General Public License](uwadv/LICENSE).

For the official Underworld Adventures home page, head over to:
https://vividos.github.io/UnderworldAdventures/

# Git Repository

This is the source code repository. The folders contain the following:

* docs: Contains the official home page source code.
* [uwadv](uwadv/README.md): Contains the Underworld Adventures source code.
* webpages: Contains the old home page that was hosted on SourceForge.net.

The repository also contains the latest version of the
[uw-formats.txt](https://github.com/vividos/UnderworldAdventures/blob/master/uwadv/docs/uw-formats.txt) file,
the file formats descriptions of the Ultima Underworld 1 and 2 games.

# Build

The project has an automated build provided by AppVeyor that always hosts the
binaries of the latest build:

[![Build status](https://ci.appveyor.com/api/projects/status/pi4hkego4ja858u4?svg=true)](https://ci.appveyor.com/project/vividos/underworldadventures)

(click on Artifacts to download the uwadv.zip archive)

The project is occasionally checked with SonarCloud:

[![Quality Gate](https://sonarcloud.io/api/badges/gate?key=UnderworldAdventures)](https://sonarcloud.io/dashboard?id=UnderworldAdventures)
