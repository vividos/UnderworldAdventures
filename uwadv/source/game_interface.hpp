//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file game_interface.hpp
/// \brief game interface class
//
#pragma once

namespace Base
{
   class Settings;
   class ResourceManager;
   class SavegamesManager;
}
namespace Audio
{
   class AudioManager;
}
namespace Underworld
{
   class Underworld;
   class GameLogic;
}
class ImageManager;
class ua_renderer;
class IScripting;
class ua_debug_server;
class ua_screen;
class GameStrings;
class IUserInterface;


// enums

/// game events that can be sent to the event queue via SDL_PushEvent using SDL_USEREVENT
enum ua_game_events
{
   /// sent to destroy the current screen, pop the last one off the stack as new current screen
   ua_event_destroy_screen = 0,

   /// event sent when a screen resumes processing, e.g. when getting popped off the screen stack
   ua_event_resume_screen,

   /// sent to current screen when an text edit window finished editing
   ua_event_textedit_finished,

   /// sent to screen when user aborted text editing
   ua_event_textedit_aborted,

   /// last (dummy) event
   ua_event_last
};


// classes

/// basic game interface class, without ui stuff
class IBasicGame
{
public:
   /// returns game tickrate
   virtual double get_tickrate() = 0;

   /// pauses or unpauses game
   virtual bool pause_game(bool pause) = 0;

   /// returns settings object
   virtual Base::Settings& get_settings() = 0;

   /// returns settings object
   virtual Base::ResourceManager& GetResourceManager() = 0;

   /// returns savegames manager object
   virtual Base::SavegamesManager& get_savegames_manager() = 0;

   /// returns scripting object
   virtual IScripting& get_scripting() = 0;

   /// returns debugger object
   virtual ua_debug_server& get_debugger() = 0;

   /// returns game strings object
   virtual GameStrings& GetGameStrings() = 0;

   /// returns underworld object
   virtual Underworld::Underworld& GetUnderworld() = 0;

   virtual Underworld::GameLogic& GetGameLogic() = 0;

   virtual IUserInterface* GetUserInterface() = 0;

   /// initializes game; only called after all stuff is initialized and ready
   virtual void init_game() = 0;

   /// cleans up game
   virtual void done_game() = 0;
};


/// game interface class
class IGame : public IBasicGame
{
public:
   /// returns audio manager
   virtual Audio::AudioManager& get_audio_manager() = 0;

   /// returns image manager object
   virtual ImageManager& get_image_manager() = 0;

   /// returns renderer object
   virtual ua_renderer& get_renderer() = 0;

   /// replaces current screen with new one; saves current on a screen stack when selected
   virtual void replace_screen(ua_screen* new_screen, bool save_current) = 0;

   /// removes current screen at next event processing
   virtual void remove_screen() = 0;

   /// returns screen x resolution
   virtual unsigned int get_screen_xres() = 0;
   /// returns screen y resolution
   virtual unsigned int get_screen_yres() = 0;

protected:
   /// ctor
   IGame() {}
};
