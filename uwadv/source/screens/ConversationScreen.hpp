//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2021,2022 Underworld Adventures Team
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
/// \file ConversationScreen.hpp
/// \brief conversation screen
//
#pragma once

#include "ImageScreen.hpp"
#include "ImageQuad.hpp"
#include "TextScroll.hpp"
#include "MouseCursor.hpp"
#include "Conversation.hpp"
#include "ConversationDebugger.hpp"
#include "TextEditWindow.hpp"

/// conv screen state values
enum ConversationScreenState
{
   convScreenStateRunning = 0, ///< vm code can run
   convScreenStateWaitMenu,   ///< waiting for menu selection
   convScreenStateWaitEnd,    ///< waiting for a key to end conversation
   convScreenStateTextInput,  ///< user currently can enter input text
   convScreenStateFadeIn,     ///< fading in screen
   convScreenStateFadeOut,    ///< fading out screen
};

/// conversation screen class
class ConversationScreen : public ImageScreen,
   public Conv::ICodeCallback
{
public:
   /// ctor
   ConversationScreen(IGame& game, Uint16 convObjectPos);
   /// dtor
   virtual ~ConversationScreen() {}

   // virtual functions from Screen
   virtual void Init() override;
   virtual void Destroy() override;
   virtual void Draw() override;
   virtual bool ProcessEvent(SDL_Event& event) override;
   virtual void Tick() override;

   // virtual functions from ImageScreen
   virtual void OnFadeInEnded() override;

   // virtual functions from ICodeCallback
   virtual void Say(Uint16 index) override;
   virtual Uint16 BablMenu(const std::vector<Uint16>& answerStringIds) override;
   virtual Uint16 ExternalFunc(const char* funcname, Conv::ConvStack& stack) override;

protected:
   /// allocates new local string
   Uint16 AllocString(const char* text);

protected:
   /// time to fade in / out screen
   static const double s_fadeTime;

   /// time to wait before conversation partner answers
   static const double s_answerWaitTime;

   /// conversation code virtual machine
   Conv::Conversation m_codeVM;

   /// conversation code debugger
   Conv::ConversationDebugger m_convDebugger;

   // UI elements

   /// conversation scroll
   TextScroll m_conversationScroll;

   /// menu text scroll
   TextScroll m_menuScroll;

   /// text edit window for input
   TextEditWindow m_textEdit;

   /// mouse cursor
   MouseCursor m_mouseCursor;

   /// font for panel names
   Font m_normalFont;

   /// counter to wait some ticks
   unsigned int m_waitCount;

   // conversation stuff

   /// position in object list of npc object
   Uint16 m_convObjectPos;

   /// screen state
   ConversationScreenState m_state;

   /// possible values for answer to push on stack after selection
   std::vector<Uint16> m_answerValues;

   /// answer string id's
   std::vector<Uint16> m_answerStringIds;
};
