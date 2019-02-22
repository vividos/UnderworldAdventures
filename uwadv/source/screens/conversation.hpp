//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file conversation.hpp
/// \brief conversation screen
//
#pragma once

#include "screen.hpp"
#include "imgquad.hpp"
#include "textscroll.hpp"
#include "mousecursor.hpp"
#include "conv.hpp"
#include "textedit.hpp"
#include "fading.hpp"

/// conv screen state values
enum ua_conv_screen_state
{
   ua_state_running=0,  ///< vm code can run
   ua_state_wait_menu,  ///< waiting for menu selection
   ua_state_wait_end,   ///< waiting for a key to end conversation
   ua_state_text_input, ///< user currently can enter input text
   ua_state_fadein,     ///< fading in screen
   ua_state_fadeout,    ///< fading out screen
};

/// conversation screen class
class ua_conversation_screen: public ua_screen,
   public Conv::ICodeCallback
{
public:
   /// ctor
   ua_conversation_screen(IGame& game, Uint16 conv_objpos);
   /// dtor
   virtual ~ua_conversation_screen(){}

   // virtual functions from ua_screen
   virtual void init() override;
   virtual void destroy() override;
   virtual void draw() override;
   virtual bool process_event(SDL_Event& event) override;
   virtual void tick() override;

   // virtual functions from ICodeCallback
   virtual void Say(Uint16 index) override;
   virtual Uint16 BablMenu(const std::vector<Uint16>& answerStringIds) override;
   virtual Uint16 ExternalFunc(const char* funcname, Conv::ConvStack& stack) override;

protected:
   /// allocates new local string
   Uint16 alloc_string(const char* the_str);

protected:
   // constants

   /// time to fade in / out screen
   static const double fade_time;

   /// time to wait before conversation partner answers
   static const double answer_wait_time;

   //ua_debug_conversation conv;

   /// conversation code virtual machine
   Conv::Conversation code_vm;

   // ui elements

   /// background image
   ua_image_quad img_back;

   /// conversation scroll
   ua_textscroll scroll_conv;

   /// menu text scroll
   ua_textscroll scroll_menu;

   /// text edit window for input
   ua_textedit_window textedit;

   /// mouse cursor
   ua_mousecursor mousecursor;

   /// fading helper
   ua_fading_helper fader;

   /// font for panel names
   ua_font font_normal;

   /// counter to wait some ticks
   unsigned int wait_count;

   // conversation stuff

   /// position in master object list of npc object
   Uint16 objpos;

   /// screen state
   ua_conv_screen_state state;

   /// possible values for answer to push on stack after selection
   std::vector<Uint16> answer_values;

   /// answer string id's
   std::vector<Uint16> answer_string_ids;
};
