/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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
/*! \file audio.hpp

   \brief audio interface definition

   the audio interface lets the user start and stop playing music tracks and
   sound effects in the background. sound effects are played once, until they
   end.

   a new audio 'object' must be created with

      ua_audio_interface::get_audio_interface();

   which returns a pointer to the audio interface. when done, the interface
   pointer must be destroyed with the delete operator.

*/

// include guard
#ifndef __uwadv_audio_hpp_
#define __uwadv_audio_hpp_

// needed includes
#include "settings.hpp"
#include "files.hpp"


// classes

//! audio interface class
class ua_audio_interface
{
public:
   //! returns an audio interface
   static ua_audio_interface *get_audio_interface();

   //! initializes audio
   virtual void init(ua_settings &settings, ua_files_manager &filesmgr)=0;

   //! plays a sound; stops when finished
   virtual void play_sound(const char *soundname)=0;

   //! stops sound playback
   virtual void stop_sound()=0;

   //! starts music playback
   virtual void start_music(unsigned int music, bool repeat)=0;

   //! fades out currently playing music track; fadeout time in seconds
   virtual void fadeout_music(double time)=0;

   //! stops music playback
   virtual void stop_music()=0;

   //! dtor
   virtual ~ua_audio_interface(){}

protected:
   //! ctor
   ua_audio_interface(){}

   //! copy ctor
   ua_audio_interface(const ua_audio_interface &ai){ ai; }

   //! assign operator
   ua_audio_interface &operator =(const ua_audio_interface &ai){ ai; return *this; }
};

#endif
