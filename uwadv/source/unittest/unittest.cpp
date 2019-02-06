/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2004,2005,2006 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file unittest.cpp

   \brief unit test implementation

*/

// needed includes
#include "unittest.hpp"
#include "settings.hpp"
#include "filesystem.hpp"

// global variables

//! settings common for all unit tests
static Base::Settings g_settings;

/// indicates if settings were already initialized
static bool g_settingsInitialized = false;

Base::Settings& UnitTest::GetTestSettings()
{
   if (!g_settingsInitialized)
   {
      Base::LoadSettings(g_settings);
      g_settingsInitialized = true;
   }

   // check settings if they are right
   std::string strFilename = g_settings.GetString(Base::settingUw1Path) + "/uw.exe";
   if (!Base::FileSystem::FileExists(strFilename))
   {
      UaTrace("The Ultima Underworld 1 files cannot be found in %s\n"
         "Make sure to have a properly configured uwadv.cfg file!\n",
         strFilename.c_str());
      throw std::runtime_error("Couldn't find Ultima Underworld 1 files");
   }

   strFilename = g_settings.GetString(Base::settingUw2Path) + "/uw2.exe";
   if (!Base::FileSystem::FileExists(strFilename))
   {
      UaTrace("The Ultima Underworld 2 files cannot be found in %s\n"
         "Make sure to have a properly configured uwadv.cfg file!\n",
         strFilename.c_str());
      throw std::runtime_error("Couldn't find Ultima Underworld 1 files");
   }

   return g_settings;
}

int main()
{
   return 42;
}
